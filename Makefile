#
# Makefile
#

#
# Modules
#
build_modules:
	make -C src all build=$(build)
clean_modules:
	make -C src clean build=$(build)

build_ghost_data:
	make -C src ghost_data build=$(build)
build_utils:
	make -C src utils build=$(build)
build_mupen64plus:
	make -C src mupen64plus build=$(build)

#
# Tests
#
run_tests: build_modules
	make -C test run_all build=$(build) v=$(v)
build_test_mods:
	make -C test build_mods build=test
clean_test:
	make -C test clean_bins clean_mods build=test

build_misc_test_utils:
	make -C test misc_test_utils build=test

ghost_data_test: build_ghost_data build_utils
	make -C test ghost_data build=$(build) v=$(v)
mupen64plus_test: build_mupen64plus build_utils build_misc_test_utils
	make -C test mupen64plus build=$(build) v=$(v)

