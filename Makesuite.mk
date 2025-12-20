#
# ------------
# Makesuite.mk
# ------------
# Description: Makefile build, test function suite for C projects
# Repo:		   
# Author:      jvalcher
# License: 	   MIT
#



###########
# Private #
###########

define CHECK_BUILD
$(if $(filter dev prod test,$(1)),,\
  $(error Missing 'build=prod|dev|test'))
endef

define CHECK_GLOBAL_DEF
$(if $(filter undefined,$(origin $(1))),\
  	$(error Empty '$(1)' global variable not found, '$(1) :='))
endef

define CHECK_GLOBAL_SET
$(if $(filter undefined,$(origin $(1))),\
	$(error Global variable '$(1)' not set, e.g. '$(1) := $(2)'))
endef

# Check if custom module variable defined, if not, set to default
define RESOLVE_VAR
$(eval $(1)_$(2) := \
  $(if $(filter undefined,$(origin $(1)_$(2))), \
      $($(2)), \
      $($(1)_$(2))))
endef
define RESOLVE_BUILD_CFLAGS
$(eval $(1)_cflags += \
  $(if $(filter undefined,$(origin $(1)_$(2))), \
      $($(2)), \
      $($(1)_$(2))))
endef

define GET_GLOBALS_EXT_CC_CFLAGS
$(eval $(call CHECK_GLOBAL_SET,ext,c))
$(eval $(call CHECK_GLOBAL_SET,cc,gcc))

$(eval $(call RESOLVE_VAR,$(1),ext))
$(eval $(call RESOLVE_VAR,$(1),cc))
$(eval $(call RESOLVE_VAR,$(1),cflags))
endef

define GET_GLOBALS_BUILD_CFLAGS
$(if $(filter prod,$(2)),\
	$(eval $(call RESOLVE_BUILD_CFLAGS,$(1),cflags_prod)))
$(if $(filter dev,$(2)),\
	$(eval $(call RESOLVE_BUILD_CFLAGS,$(1),cflags_dev)))
endef

define INC_DEPS
ifndef __DEPS_INCLUDED
__DEPS_INCLUDED := 1
-include $(wildcard ../obj/$(1)/*.d)
-include $(wildcard ../obj/$(1)/*/*.d)
endif
endef

define TEST_ALL_CLEAN_RULES
.PHONY: $(1) $(2)
$(1): $(test_targs)
$(2): $(test_clean_targs)
endef

define SET_VERBOSITY
ifndef __VERBOSITY_CHECKED
__VERBOSITY_CHECKED := 1
v ?= 1
ifeq ($(v),0)
MAKEFLAGS += --no-print-directory
q := @
else
q :=
endif
endif
endef



##########
# Public #
##########


# ------------------------------------
# BUILD_SRC - build source file module
# ------------------------------------
#
# Parameters:
#
# 	$(1) = module name  (src/$(1).$(ext))
#  	$(2) = module build type  (dev, prod)
#
# Globals:
#
#	# Default
#   ext := c
# 	cc := gcc
#	cflags := -Wall -Wextra
#	cflags_prod := -O2
#	cflags_dev := -g
#
#	# Per-module
#	$(1)_ext := cpp
#	$(1)_cc := clang
#	$(1)_cflags := -Wall -Wextra -Wpedantic
#	$(1)_cflags_prod := -O1
#	$(1)_cflags_dev := -Og -g3
#
#	# all:, clean: prerequisites
#	build_targs :=
#	clean_targs :=
#
#
define BUILD_SRC

$(eval $(call CHECK_BUILD,$(2)))
$(eval $(call GET_GLOBALS_EXT_CC_CFLAGS,$(1),$(2)))
$(eval $(call GET_GLOBALS_BUILD_CFLAGS,$(1),$(2)))
$(eval $(call SET_VERBOSITY))

$(eval build_targs += $(1))
$(eval clean_targs += clean_$(1))

$(eval $(1)_inc_dir := ../inc/src)
$(eval $(1)_obj_root := ../obj)
$(eval $(1)_obj_dir := $$($(1)_obj_root)/$(2))

$(eval $(call INC_DEPS,$(2)))

$(eval $(1)_src := $(1).$(ext))
$(eval $(1)_obj := $$($(1)_obj_dir)/$(1).o)
$(eval $(1)_d := $$($(1)_obj:.o=.d))

.PHONY: $(1)
$(1): $$($(1)_obj)

$$($(1)_obj): $$($(1)_src) | $$($(1)_obj_dir)
	$(q)$$($(1)_cc) $$($(1)_cflags) -I$$($(1)_inc_dir) -MMD -MP -c $$< -o $$@

$$($(1)_obj_dir):
	$(q)mkdir -p $$@

.PHONY: clean_$(1)
clean_$(1):
	$(q)rm -f $$($(1)_obj) 
	$(q)rm -f $$($(1)_d)

endef



# ----------------------------------
# BUILD_DIR - build directory module
# ----------------------------------
#
# Parameters:
#
# 	$(1) = module name  (src/$(1)/*.$(ext))
#  	$(2) = module build type  (dev, prod)
#
# Globals:
#
#	# Default
#   ext := c
# 	cc := gcc
#	cflags := -Wall -Wextra
#	cflags_prod := -O3
#	cflags_dev := -g
#
#	# Custom
#	$(1)_ext := cpp
#	$(1)_cc := clang
#	$(1)_cflags := -Wall -Wextra -Wpedantic
#	$(1)_cflags_prod := -O1
#	$(1)_cflags_dev := -Og -g3
#
#	# all:, clean: prerequisites
#	build_targs :=
#	clean_targs :=
#
define BUILD_DIR

$(eval $(call CHECK_BUILD,$(2)))
$(eval $(call GET_GLOBALS_EXT_CC_CFLAGS,$(1),$(2)))
$(eval $(call GET_GLOBALS_BUILD_CFLAGS,$(1),$(2)))
$(eval $(call SET_VERBOSITY))

$(eval build_targs += $(1))
$(eval clean_targs += clean_$(1))

$(eval $(1)_inc_dir := ../inc/src)
$(eval $(1)_obj_root := ../obj)
$(eval $(1)_obj_dir := $$($(1)_obj_root)/$(2))
$(eval $(1)_objs_dir := $$($(1)_obj_dir)/$(1))

$(eval $(call INC_DEPS,$(2)))

$(eval $(1)_srcs := $$(wildcard $(1)/*.$(ext)))
$(eval $(1)_objs := $$(patsubst $(1)/%.$(ext),$$($(1)_objs_dir)/%.o,$$($(1)_srcs)))
$(eval $(1)_obj := $$($(1)_obj_dir)/$(1).o)
$(eval $(1)_ds := $$($(1)_objs:.o=.d))
$(eval $(1)_d := $$($(1)_obj:.o=.d))

.PHONY: $(1)
$(1): $$($(1)_obj)

$$($(1)_obj): $$($(1)_objs)
	$(q)$$($(1)_cc) -r $$^ -o $$@

$$($(1)_objs_dir)/%.o: $(1)/%.$(ext) | $$($(1)_objs_dir)
	$(q)$$($(1)_cc) $$($(1)_cflags) -I$$($(1)_inc_dir) -MMD -MP -c $$< -o $$@

$$($(1)_objs_dir):
	$(q)mkdir -p $$@

.PHONY: clean_$(1)
clean_$(1):
	$(q)rm -f $$($(1)_objs)
	$(q)rm -f $$($(1)_obj)
	$(q)rm -f $$($(1)_ds)
	$(q)rm -f $$($(1)_d)
	$(q)rm -fd $$($(1)_objs_dir)

endef



# ------------------------------------
# TEST_MODULE - build, run module test
# ------------------------------------
#
# Parameters:
#
#	$(1) - module name  (src/$(1).$(ext) or src/$(1)/*.$(ext))
#  	$(2) = module build type  (dev, prod)
#
# Globals:
#
#	# Default
#   ext := c
# 	cc := gcc
#	cflags := -g -Wall -Wextra
#
#	# Per-test file
#	$(1)_ext := cpp
#	$(1)_cc := clang
#	$(1)_cflags := -g -Wall -Wextra -Wpedantic
#
#	# src/, test/ module dependencies
#	$(1)_mod_deps := file subproc
#	$(1)_test_deps := tools random
#
define TEST_MODULE

$(eval $(call CHECK_BUILD,$(2)))
$(eval $(call GET_GLOBALS_EXT_CC_CFLAGS,$(1),$(2)))
$(eval $(call SET_VERBOSITY))

$(eval test_targs += $(1))
$(eval test_clean_targs += clean_$(1))

$(eval $(1)_inc_dir := ../inc/src)
$(eval $(1)_test_inc_dir := ../inc/test)
$(eval $(1)_obj_root := ../obj)
$(eval $(1)_obj_dir := ../obj/$(2))
$(eval $(1)_src_dir := ../src)

$(eval $(1)_bin_dir := ../bin/test)
$(eval $(1)_bin := $$($(1)_bin_dir)/$(1))

$(eval $(1)_mod_obj := $$($(1)_obj_dir)/$(1).o)
$(eval $(1)_mod_dep_objs := \
  $(addprefix $($(1)_obj_dir)/, \
    $(addsuffix .o, $($(1)_mod_deps))))
$(eval $(1)_test_dep_objs := \
  $(addprefix $($(1)_obj_root)/test/, \
    $(addsuffix .o, $($(1)_test_deps))))

.PHONY: $(1)
$(1): $$($(1)_bin_dir)
	$(q)$(cc) $$($(1)_cflags) -I$$($(1)_inc_dir) -I$$($(1)_test_inc_dir) \
		$(1)_test.$(ext) \
		$$($(1)_mod_obj) \
		$$($(1)_mod_dep_objs) \
		$$($(1)_test_dep_objs) \
		-o $$($(1)_bin)
	$(q)./$$($(1)_bin)

.PHONY: clean_$(1)
clean_$(1):
	$(q)rm -f $$($(1)_bin)

$$($(1)_bin_dir):
	$(q)mkdir -p $$@

endef

