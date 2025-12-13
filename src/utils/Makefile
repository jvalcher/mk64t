#
# src/utils/Makefile
#

cc := gcc
cflags := -Wall -Wextra -I../../inc
srcs := $(wildcard *.c)

#
# Test build
#

test_objs_dir := ../../obj/test
test_objs := $(patsubst %.c,$(test_objs_dir)/%.o,$(srcs))
test_obj := $(test_objs_dir)/utils.o

.PHONY: test_utils
test_utils: $(test_obj)

$(test_obj): $(test_objs)
	$(cc) -r $^ -o $@

$(test_objs_dir)/%.o: %.c
	$(cc) $(cflags) -c $< -o $@

.PHONY: test_clean
test_clean:
	rm -f $(test_objs)
	rm -f $(test_obj)
