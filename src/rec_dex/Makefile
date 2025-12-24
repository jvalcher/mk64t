#
# Ghost data generation
#
dex_dir := ../../data/ghost/dex
dex_files := $(wildcard $(dex_dir)/*.dex)
dex_src_files  := $(patsubst $(dex_dir)/%.dex,dex_%.c,$(dex_files))

.PHONY: create_dex_sources
create_dex_sources: $(dex_src_files)

dex_%.c: $(dex_dir)/%.dex | clean_dex_sources
	@echo "$<  ->  $@"
	@echo "/* Auto-generated from $< */" > $@
	@echo "#include <stdint.h>" >> $@
	@echo "const uint8_t dex_$*[] = {" >> $@
	@xxd -i $< | sed '1,2d;$$d;$$d' >> $@
	@echo "const unsigned int dex_$*_size = sizeof(dex_$*);" >> $@

clean_dex_sources:
	rm -f ./dex_*.c

