#
# test/Makefile
#
ext := c
cc := gcc
cflags := -g3 -Wall -Wextra

test_targs :=
test_clean_targs :=
build_targs :=
clean_targs :=

mupen64plus_mod_deps := utils
mupen64plus_test_deps := misc_test_utils
ghost_data_mod_deps := utils

include ../Makesuite.mk

$(eval $(call TEST_MODULE,mupen64plus,$(build)))
$(eval $(call TEST_MODULE,ghost_data,$(build)))

$(eval $(call BUILD_DIR,misc_test_utils,test))

.PHONY: run_all clean_bins build_mods clean_mods
run_all: $(test_targs)
clean_bins: $(test_clean_targs)
build_mods: $(build_targs)
clean_mods: $(clean_targs)
