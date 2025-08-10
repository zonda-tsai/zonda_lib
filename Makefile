PREFIX = /usr/local
DIR_NAME = zonda_unicode_lib
INSTALL_PATH = $(PREFIX)/include/$(DIR_NAME)
FILES = $(wildcard include/*.h)


.PHONY: install uninstall
install:
	printf "Installing...\n"
	@mkdir -p $(INSTALL_PATH)
	@cp -v $(HEADERS) $(INSTALL_PATH)
	printf "Done\n"
	@echo "Files location: $(INSTALL_PATH)"
	@echo "To use the headerfiles, using '#include <$(INSTALL_DIR_NAME)/[name].h>'"

uninstall:
	printf "Uninstalling...\n"
	@rm -rf $(INSTALL_PATH)
	printf "Done\n"
