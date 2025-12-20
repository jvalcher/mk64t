#
# src/Makefile
#
ext := c
cc := gcc
cflags := -Wall -Wextra
cflags_dev := -g3
cflags_prod := -O2

build_targs :=
clean_targs :=

include ../Makesuite.mk

$(eval $(call BUILD_SRC,mupen64plus,$(build)))

$(eval $(call BUILD_DIR,ghost_data,$(build)))
$(eval $(call BUILD_DIR,utils,$(build)))

.PHONY: all clean
all: $(build_targs)
clean: $(clean_targs)

