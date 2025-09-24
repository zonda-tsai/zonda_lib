PREFIX = /usr/local
DIR_NAME = zonda_lib
INSTALL_PATH = $(PREFIX)/include

# C 編譯器和旗標
CC = gcc
CFLAGS = -Wall -Wextra -I.

# 自動尋找所有 .c 原始碼檔案
SRCS := $(wildcard glyph/*.c) $(wildcard layout/*.c) $(wildcard file_system/*.c) $(wildcard common/*.c)
# 自動將 .c 檔名轉換為 .o 目的檔名
OBJS := $(SRCS:.c=.o)

# 函式庫名稱
LIB_NAME = libzonda.a

# --- 主要指令 ---

.PHONY: all clean install uninstall

# 預設指令 (例如執行 make)
all: $(LIB_NAME)

$(LIB_NAME): $(OBJS)
	@echo "AR  $@"
	@ar rcs $@ $^

%.o: %.c
	@echo "CC  $@"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "Cleaning project..."
	@rm -f $(OBJS) $(LIB_NAME)

# 安裝指令
install: all
	@echo "Installing header files and library..."
	@mkdir -p $(INSTALL_PATH)/glyph
	@mkdir -p $(INSTALL_PATH)/layout
	@mkdir -p $(INSTALL_PATH)/file_system
	@mkdir -p $(INSTALL_PATH)/common
	@mkdir -p $(PREFIX)/lib
	@cp glyph/*.h $(INSTALL_PATH)/glyph
	@cp layout/*.h $(INSTALL_PATH)/layout
	@cp file_system/*.h $(INSTALL_PATH)/file_system
	@cp common/*.h $(INSTALL_PATH)/common
	@cp $(LIB_NAME) $(PREFIX)/lib/

	@echo "Installation complete."
	@echo "Headers are in: $(INSTALL_PATH)"
	@echo "Library is in: $(PREFIX)/lib"

# 解除安裝指令
uninstall:
	@echo "Uninstalling header files and library..."
	@rm -rf $(INSTALL_PATH)/glyph $(INSTALL_PATH)/layout $(INSTALL_PATH)/file_system $(wildcard common/*.c)
	@rm -f $(PREFIX)/lib/$(LIB_NAME)
	@echo "Uninstallation complete."
