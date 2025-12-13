#
# test/Makefile
#
cc := gcc
cflags := -g -Wall -Wextra -I../inc

src_dir := ../src
test_bin_dir := ../bin/test
test_obj_dir := ../obj/test
utils_srcs := $(wildcard $(src_dir)/utils/*c)


all: \
	test_mupen64plus \
	test_utils

#
# mupen64plus
#
test_mupen64plus: test_mupen64plus.c \
				  $(test_obj_dir)/utils.o \
				  $(test_obj_dir)/mupen64plus.o
	$(cc) $(cflags) $^ -o $(test_bin_dir)/test_mupen64plus
	./$(test_bin_dir)/test_mupen64plus

$(test_obj_dir)/mupen64plus.o: $(src_dir)/mupen64plus.c
	make -C $(src_dir) test_build_mupen64plus

#
# utils
#
test_utils: test_utils.c $(test_obj_dir)/utils.o
	$(cc) $(cflags) $^ -o $(test_bin_dir)/test_utils
	./$(test_bin_dir)/test_utils

$(test_obj_dir)/utils.o: $(utils_srcs)
	make -C $(src_dir) test_build_utils
