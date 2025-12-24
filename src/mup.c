/*
    src/mupen64plus.c
*/
#ifdef _WIN32
#else
#define _GNU_SOURCE
#endif

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>
#include <SDL2/SDL.h>
#include <ctype.h>

#ifdef _WIN32
#else
#include <spawn.h>
#include <signal.h>
#include <wait.h>
#include <pty.h>
#endif

#include "utils.h"
#include "path.h"
#include "save_dirs.h"
#include "mup.h"

#ifdef _WIN32
#else
#define MUP_CONFIG_DIR_PATH  "/.config/mk64tt/mupen64plus/config"
#endif

#define R         "\033[1;0m"  // reset to default
#define RED       "\033[1;31m"
#define CYAN      "\033[1;36m"
#define GREEN     "\033[1;32m"
#define YELLOW    "\033[1;33m"
#define PURPLE    "\033[1;35m"

#define MUP_OUTPUT_BUF_SIZE  2048
#define MUP_ERR_BUF_SIZE  256
#define ROM_INFO_BUF_SIZE  48

static char rom_goodname_buf[ROM_INFO_BUF_SIZE];
static char rom_md5_buf[ROM_INFO_BUF_SIZE];
static char rom_country_buf[ROM_INFO_BUF_SIZE];
static char rom_imagetype_buf[ROM_INFO_BUF_SIZE];
static char rom_mup_id_buf[ROM_INFO_BUF_SIZE];
static bool mup_err_occurred = false;
static char mup_err_buf[MUP_ERR_BUF_SIZE];

static bool mup_initialized = false;
static int mup_pty_fd;
static pid_t mup_pid;
static char **mup_cmd = NULL;
static int rom_index;
static char categ[12];

typedef struct mup_cmd_arg {
    char *str;
    struct mup_cmd_arg *next;
} mup_cmd_list_arg_t;

typedef struct {
    char *bin
    mup_cmd_list_arg_t *arg_head;
    mup_cmd_list_arg_t *arg_tail;
    char *rom;
    int arg_count;
} mup_cmd_list_t;

static mup_cmd_list_t mup_cmd_list = { 
    .bin: NULL, 
    .arg_head: NULL, 
    .arg_tail: NULL, 
    .rom: NULL, 
    .arg_count: 0 
};

static int get_screen_resolution(int *w, int *h);
static void destroy_mup_cmd_list(void);
static mup_arg_t* mup_cmd_list_arg_create(const char *str);
static bool mup_cmd_list_arg_is_set(const char *str);
static void destroy_mup_cmd(void);
static int mup_start_impl(void);
static int get_rom_key_val(const char *buf, const char *key, char *ret_buf);
static int get_rom_info(void);



int mup_add_arg(const char *str)
{
    mup_cmd_list_arg_t *arg = NULL;

    if (strstr(str, CORE_SRAM_EQ) != NULL) {
        _logp("Setting \"%s\" is not allowed", CORE_SRAM_EQ);
        return -1;
    }

    arg = mup_cmd_list_arg_create(str);
    if (!arg)
        return -1;

    if (mup_cmd_list.arg_head == NULL) {
        mup_cmd_list.arg_head = arg;
        mup_cmd_list.tail = arg;
    } else {
        mup_cmd_list.tail->next = arg;
        mup_arg_list.tail = arg;
    }

    mup_cmd_list.arg_count += 1;

    return 0;
}

bool mup_running(void)
{
    return mup_pid > 0;
}

int mup_stop(void)
{
    _logp("Stopping mupen64plus process...");

    if (mup_pid <= 0) {
        _logp("Mupen64plus process already stopped");
        return 0;
    }

    if (kill(-mup_pid, SIGKILL)) {
        if (errno == ESRCH)
            return 0;
        _logpe("Mupen64plus process kill failed");   
        return -1;
    }

    if (mup_wait())
        return -1;

    return 0;
}

int mup_get_rom_info(void)
{
    if (mup_running())
        if (mup_stop()) 
            return -1;
    if (mup_start_impl())
        return -1;
    if (get_rom_info())
        return -1;
    if (mup_stop())
        return -1;

    return 0;
}

int mup_init(const char *categ, int type)
{
    int i, w, h;
    char *tmp = NULL;
    char *path = NULL;
    mup_cmd_list_arg_t *curr_arg;

    mup_initialized = false;

    destroy_mup_cmd_list();
    destroy_mup_cmd();

    // Verify binary, ROM set
    if (!mup_cmd_list.bin) {
        _logp("Mupen64plus binary not set");
        return -1;
    }
    if (!mup_cmd_list.rom) {
        _logp("Mupen64plus ROM path not set");
        return -1;
    }

    // Create config directory
    asprintf(&mup_path, "%s%s", getenv("HOME"), MUP_CONFIG_DIR_PATH);
    if (mkdir_p(mup_path, MK64T_DIR_MODE))
        return -1;

    // Add, create config directory if not set
    if (!mup_cmd_list_arg_is_set("--configdir")) {
        mup_arg_add("--configdir");
        asprintf(&tmp, "%s%s", getenv("HOME"), MUP_CONFIG_DIR_PATH);
        if (mkdir_p(tmp, MK64T_DIR_MODE))
            return -1;
        mup_arg_add(tmp);
        free(tmp);
    }

    // Get ROM info
    mup_cmd = calloc(sizeof *mup_cmd * 3);
    if (!mup_cmd) {
        _logpe("Failed to create mupen64plus command array");
        return -1;
    }
    mup_cmd[0] = mup_cmd_list.bin;
    mup_cmd[1] = mup_cmd_list.rom;
    mup_cmd[2] = NULL;
    if (mup_get_rom_info())
        return -1;
    destroy_mup_cmd();

    // Add SRAM save directory path
    mup_arg_add("--set");
    switch(type) {
    case CATEG_DEF:
        __attribute__((fallthrough));
    case CATEG_USR:
        if (asprintf(%path, "%s%s%s%s%s", getenv("HOME"), MK64T_BASE_PATH, 
                     get_save_dir_path(categ, type), "/", rom_mup_id_buf) == -1) {
            _logp("Failed to create SRAM path string");
            return -1;
        }
        break;
    case CATEG_REC:
        if (asprintf(%path, "%s%s%s%s", getenv("HOME"), MK64T_BASE_PATH, 
                     get_save_dir_path(categ, type)) == -1) {
            _logp("Failed to create SRAM path string");
            return -1;
        }
        break;
    }
    if (asprintf(%tmp, "%s%s", CORE_SRAM_EQ, path) == -1) {
        _logp("Failed to create SRAM opt string");
        return -1;
    }
    if (type != CATEG_REC && mkdir_p(path, MK64T_DIR_MODE))
        return -1;
    mup_arg_add(tmp);
    free(path);
    free(tmp);
    
    // Create mup_cmd[]
    mup_cmd = calloc(sizeof *mup_cmd * (mup_cmd_list.arg_count + 3));
    if (!mup_cmd) {
        _logpe("Failed to create mupen64plus command array");
        return -1;
    }
    i = 0;
    mup_cmd[i++] = mup_cmd_list.bin;
    curr_arg = mup_cmd_list.head;
    for (; curr_arg; ++i) {
        mup_cmd[i] = curr_arg->str;
        curr_arg = curr_arg->next;
    }
    mup_cmd[i] = mup_cmd_list.rom;
    rom_index = i++;
    mup_cmd[i] = NULL;

    mup_initialized = true;

    return 0;
}

int mup_load_bin(const char *path)
{
    if (strlen(path) == 0) {
        _logp("Error: empty mupen64plus binary path");
        return -1;
    }

    if (mup_cmd_list.bin) {
        free(mup_cmd_list.bin);
        mup_cmd_list.bin = NULL;
    }

    mup_cmd_list.bin = strdup(path);
    if (!mup_cmd_list.bin) {
        _logpe("Failed to create mupen64plus binary path string");
        return -1;
    }

    return 0;
}

int mup_load_rom(const char *path)
{
    if (strlen(path) == 0) {
        _logp("Error: empty ROM path");
        return -1;
    }

    if (mup_cmd_list.rom) {
        free(mup_cmd_list.rom);
        mup_cmd_list.rom = NULL;
    }

    mup_cmd_list.rom = strdup(path);
    if (!mup_cmd_list.rom) {
        _logpe("Failed to create ROM path string");
        return -1;
    }

    if (mup_initialized) {
        free(mup_cmd[rom_index]);
        mup_cmd[rom_index] = mup_cmd_list.rom;
    }

    _log("ROM loaded: %s", path);
    _logf("ROM loaded: " PURPLE "%s" R "\n", path);

    return 0;
}

int mup_wait(void)
{
    int wstatus;
    pid_t wpid;

    do {
        wpid = waitpid(-mup_pid, &wstatus, 0);
    } while (wpid == -1 && errno == EINTR);
    if (wpid == -1) {
        _logpe("Mupen64plus process waitpid failed");
        return -1;
    }

    if (WIFEXITED(wstatus))
        _logp("Mupen64plus process exited (%d)", WEXITSTATUS(wstatus));
    else if (WIFSIGNALED(wstatus))
        _logp("Mupen64plus process exited (SIG%s%s)", 
               sigabbrev_np(WTERMSIG(wstatus)), WCOREDUMP(wstatus) ? ", core dumped" : "");
    else
        _logp("Mupen64plus process exited with unknown status (%d)", wstatus);

    mup_pid = -1;
    close(mup_pty_fd);

    return 0;
}

const char* mup_get_err_msg(void)
{
    return mup_err_buf;
}

bool mup_err(void)
{
    return mup_err_occurred;
}

int mup_start(void)
{
    if (mup_running())
        if (mup_stop()) 
            return -1;
    if (mup_start_impl())
        return -1;
    if (get_rom_info()) {
        if (mup_err())
            if (mup_stop()) 
                return -1;
        return -1;
    }

    return 0;
}

const char* mup_rom_goodname(void)
{
    return rom_goodname_buf;
}

const char* mup_rom_md5(void)
{
    return rom_md5_buf;
}

const char* mup_rom_imagetype(void)
{
    return rom_imagetype_buf;
}

const char* mup_rom_country(void)
{
    return rom_country_buf;
}

const char* rom_mup_id(void)
{
    return rom_mup_id_buf;
}

void mup_cleanup(void)
{
    if (mup_save_dir) {
        free(mup_save_dir);
        mup_save_dir = NULL;
    }
    destroy_mup_cmd_list();
    destroy_mup_cmd();
}



/*********
  Private
 *********/

static int get_screen_resolution(int *w, int *h)
{
    SDL_DisplayMode mode;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        _logp("SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }

    if (SDL_GetCurrentDisplayMode(0, &mode) != 0) {
        _logp("SDL_GetCurrentDisplayMode failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    *w = mode.w;
    *h = mode.h;

    SDL_Quit();

    return 0;
}

static void destroy_mup_cmd_list(void)
{
    mup_cmd_list_arg_t *curr_arg;
    mup_cmd_list_arg_t *next_arg;

    if (mup_cmd_list.bin) {
        free(mup_cmd_list.bin);
        mup_cmd_list.bin = NULL;
    }
    if (mup_cmd_list.rom) {
        free(mup_cmd_list.rom);
        mup_cmd_list.rom = NULL;
    }

    curr_arg = mup_cmd_list.head;
    while (curr_arg) {
        next_arg = curr_arg->next;
        free(curr_arg->str);
        free(curr_arg);
        curr_arg = next_arg;
    }
    mup_cmd_list.arg_head = NULL;
    mup_cmd_list.arg_tail = NULL;

    mup_cmd_list.arg_count = 0;
}

static mup_arg_t* mup_cmd_list_arg_create(const char *str)
{
    if (!str) {
        _logp("Empty mup_cmd_list_arg_t string");
        return NULL;
    }

    mup_cmd_list_arg_t *arg = malloc(sizeof *arg);
    if (!arg) {
        _logpe("Failed to allocate new mup_cmd_list_arg_t");
        return NULL;
    }

    arg->str = strdup(str);
    if (!(arg->str)) {
        _logpe("Failed to create mup_cmd_list_arg_t string \"%s\"", str);
        free(arg);
        return NULL;
    }

    return arg;
}

static bool mup_cmd_list_arg_is_set(const char *str)
{
    mup_cmd_list_arg_t *curr = NULL;

    curr = mup_cmd_list.head;
    while (curr) {
        if (strstr(curr->str, str) != NULL)
            return true;
        curr = curr->next;
    }

    return false;
}

static void destroy_mup_cmd(void)
{
    if (!mup_cmd)
        return;
    free(mup_cmd);
    mup_cmd = NULL;
}

static int mup_start_impl(void)
{
    int arg_tog;
    char arg_color[12];

    if (!mup_initialized) {
        _logp("Failed to start mupen64plus: not initialized");
        return -1;
    }

    _logp("Starting mupen64plus process...");

#ifdef DEBUG
    // Print command
    arg_tog = 0;
    for (int i = 0; mup_cmd[i] != NULL; ++i) {
        if (i == 0)
            _logpf(YELLOW "%s ", mup_cmd[i]);
        else if (mup_cmd[i+1] == NULL)
            _logpf(PURPLE "%s" R "\n", mup_cmd[i]);
        else if (strstr(mup_cmd[i], "--")) {
            if (arg_tog == 0) {
                _logpf(GREEN "%s ", mup_cmd[i]);
                arg_tog = 1;
            } else {
                _logpf(CYAN "%s ", mup_cmd[i]);
                arg_tog = 0;
            }
        } else {
            _logpf("%s ", mup_cmd[i]);
        }
    }
#endif

    // Log command
    _log("Command:\n-----------");
    for (int i = 0; mup_cmd[i] != NULL; ++i)
        _logf("%s ", mup_cmd[i]);
    _logf("\n-----------\n");


    mup_pid = forkpty(&mup_pty_fd, NULL, NULL, NULL);

    if (mup_pid == -1) {
        _logpe("Mupen64plus fork failed");
        return -1;
    } else if (mup_pid == 0) {
        execvp(mup_cmd[0], mup_cmd);
        perror("Mupen64plus execvp failed");
        _exit(127);
    } 

    return 0;
}

static int get_rom_key_val(const char *buf, const char *key, char *ret_buf)
{
    int i;
    char *ptr;
    char *rom_info_not_found = "not found";

    ptr = strstr(buf, key);
    if (ptr != NULL) {
        ptr += strlen(key) + strlen(": ");
        i = 0;
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < ROM_INFO_BUF_SIZE-1)
            ret_buf[i++] = *ptr++;
        ret_buf[i] = '\0';
    } else {
        _logp("Failed to find ROM key \"%s\" in mupen64plus output", key);
        strcpy(ret_buf, rom_info_not_found);
        return -1;
    }

    return 0;
}

/*
    Get info from mupen64plus terminal output
*/
static int get_rom_info(void)
{
    int i;
    ssize_t bytes = 0;
    int buf_offset;
    char buf[MUP_OUTPUT_BUF_SIZE];
    char warn_buf[MUP_ERR_BUF_SIZE];
    char *ptr;

    char *ui_error_key = "UI-Console Error";
    char *ui_warn_key = "UI-Console Warning";
    char *goodname_key = "Goodname";
    char *md5_key = "MD5";
    char *imagetype_key = "Imagetype";
    char *country_key = "Country";
    char *end_key = "UI-Console Status";

    buf_offset = 0;

    for (;;) {
        if (buf_offset >= (int)(sizeof(buf) - 1))
            break;

        bytes = read(mup_pty_fd, buf + buf_offset, sizeof(buf)-buf_offset-1);
        if (bytes == -1 && errno != EAGAIN) {
            _logpe("Failed to read mupen64plus file descriptor");
            return -1;
        }
        if (bytes == 0)
            break;

        buf[buf_offset + bytes] = '\0';
        buf_offset += bytes;

        ptr = strstr(buf, ui_error_key);
        if (ptr != NULL)
            break;
        ptr = strstr(buf, end_key);
        if (ptr != NULL)
            break;
    }

    mup_err_occurred = false;

    // UI-Console Error
    ptr = strstr(buf, ui_error_key);
    if (ptr != NULL) {
        i = 0;
        ptr += strlen(ui_error_key) + strlen(": ");
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < MUP_ERR_BUF_SIZE-1)
            mup_err_buf[i++] = *ptr++;
        mup_err_buf[i] = '\0';
        mup_err_occurred = true;
        _logpf(GREEN "Mupen64plus" R ": " RED "%s" R "\n", mup_err_buf);
        _log("%s", mup_err_buf);
        return -1;
    }

    // UI-Console Warning
    ptr = strstr(buf, ui_warn_key);
    if (ptr != NULL) {
        i = 0;
        ptr += strlen(ui_warn_key) + strlen(": ");
        while (*ptr != '\r' && *ptr != '\n' && *ptr != '\0' && i < MUP_ERR_BUF_SIZE-1)
            warn_buf[i++] = *ptr++;
        warn_buf[i] = '\0';
        _logpf(GREEN "Mupen64plus" R ": " YELLOW "%s" R "\n", warn_buf);
        _log("%s", warn_buf);
    }

    get_rom_key_val(buf, goodname_key, rom_goodname_buf);
    get_rom_key_val(buf, md5_key, rom_md5_buf);
    get_rom_key_val(buf, imagetype_key, rom_imagetype_buf);
    get_rom_key_val(buf, country_key, rom_country_buf);

    sprintf(rom_mup_id_buf, ROM_INFO_BUF_SIZE, "%s-%.*s", 
            rom_goodname_buf, 8, rom_md5_buf);

    return 0;
}

