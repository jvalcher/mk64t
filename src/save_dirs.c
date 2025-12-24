#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "path.h"
#include "categ.h"
#include "save_dirs.h"

#ifdef _WIN32
#else
#define SAVE_DIR_DEF   "/mupen64plus/save"
#define SAVE_DIR_USER  "/mupen64plus/save_user"
#define SAVE_DIR_REC   "/mupen64plus/save_rec"
#endif

#define CATEG_BUF  24

static char *def_dir = NULL;
static char **user_dirs = NULL;
static char **rec_dirs = NULL;



static int get_categ_index(const char *categ)
{
    for (size_t i = 0; i < categ_cnt(); ++i) {
        if (strcmp(categ, categ_name[i]) == 0)
            return (int)i;
    }
    _logp("Category \"%s\" not found", categ);
    return -1;
}

char* get_save_dir_path(const char *categ, int type)
{
    int idx;

    if (!def_dir || !user_dirs || !rec_dirs) {
        _logp("Save directory paths not created");
        return NULL;
    }

    if (categ && type != CATEG_DEF) {
        idx = get_categ_index(categ);
        if (idx == -1)
            return NULL;
    }

    switch(type) {
    case CATEG_DEF:
        return def_dir;
        break;
    case CATEG_REC:
        return rec_dirs[idx];
        break;
    case CATEG_USR:
        return user_dirs[idx];
        break;
    }

    return NULL;
}

void save_dirs_clean(void)
{
    size_t i;

    if (def_dir) {
        free(def_dir);
        def_dir = NULL;
    }

    if (user_dirs) {
        for (i = 0; i < categ_cnt(); ++i)
            if (user_dirs[i])
                free(user_dirs[i]);
        free(user_dirs);
        user_dirs = NULL;
    }

    if (rec_dirs) {
        for (i = 0; i < categ_cnt(); ++i)
            if (rec_dirs[i])
                free(rec_dirs[i]);
        free(rec_dirs);
        rec_dirs = NULL;
    }
}

int create_save_dirs(void)
{
    size_t i;
    char *base_user_dir = NULL;
    char *base_rec_dir = NULL;

    // Save directory for normal, non-ghost play
    if (asprintf(&def_dir, "%s%s%s", getenv("HOME"), MK64T_BASE_PATH, SAVE_DIR_DEF) == -1) {
        _logp("Failed to create default save directory string");
        return -1;
    }
    if (mkdir_p(def_dir, MK64T_DIR_MODE))
        goto save_dirs_destroy;

    //
    // Create user and record-holding ghost path arrays, save directories
    //

    // Base paths
    if (asprintf(&base_user_dir, "%s%s%s", getenv("HOME"), MK64T_BASE_PATH, SAVE_DIR_USER) == -1) {
        _logp("Failed to create user save directory string");
        goto save_dirs_destroy;
    }
    if (asprintf(&base_rec_dir, "%s%s%s", getenv("HOME"), MK64T_BASE_PATH, SAVE_DIR_REC) == -1) {
        _logp("Failed to create record save directory string");
        goto save_dirs_destroy;
    }

    // Arrays to store paths for get_save_dir_path()
    user_dirs = calloc(sizeof *user_dirs, categ_cnt());
    if (!user_dirs) {
        _logpe("Failed to allocate directory array");
        goto save_dirs_destroy;
    }
    rec_dirs = calloc(sizeof *rec_dirs, categ_cnt());
    if (!rec_dirs) {
        _logpe("Failed to allocate directory array");
        goto save_dirs_destroy;
    }

    // User
    for (i = 0; i < categ_cnt(); ++i) {
        if (asprintf(&user_dirs[i], "%s%s%s", base_user_dir, "/", categ_name[i]) == -1) {
            _logp("Failed to create user directory path string");
            goto save_dirs_destroy;
        }
        if (mkdir_p(user_dirs[i], MK64T_DIR_MODE))
            goto save_dirs_destroy;
    }

    // Record
    for (i = 0; i < categ_cnt(); ++i) {
        if (asprintf(&rec_dirs[i], "%s%s%s", base_rec_dir, "/", categ_name[i]) == -1) {
            _logp("Failed to create user directory path string");
            goto save_dirs_destroy;
        }
        if (mkdir_p(rec_dirs[i], MK64T_DIR_MODE))
            goto save_dirs_destroy;
    }

    free(base_user_dir);
    free(base_rec_dir);

    return 0;

save_dirs_destroy:

    if (base_user_dir) free(base_user_dir);
    if (base_rec_dir) free(base_rec_dir);
    save_dirs_clean();

    return -1;
}

