#
# src/Makefile
#
cc := gcc
cflags := -Wall -Wextra -I../../inc

test_objs_dir := ../obj/test
utils_srcs := $(wildcard utils/*.c)

#
# mupen64plus
#
test_build_mupen64plus: $(test_objs_dir)/mupen64plus.o

$(test_objs_dir)/mupen64plus.o: mupen64plus.c
	$(cc) $(cflags) -g -c $< -o $@

#
# utils/
#
test_build_utils: $(utils_srcs)
	make -C utils test_utils

