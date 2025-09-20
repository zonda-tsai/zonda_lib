PREFIX = /usr/local
DIR_NAME = zonda_lib
INSTALL_PATH = $(PREFIX)/include

# C 編譯器和旗標
CC = gcc
# -I. 告訴編譯器，也請從目前目錄(.)開始尋找標頭檔
# 這樣 #include "glyph/glyph_lib.h" 就能運作
CFLAGS = -Wall -Wextra -I.

# 自動尋找所有 .c 原始碼檔案
SRCS := $(wildcard glyph/*.c) $(wildcard layout/*.c) $(wildcard file_system/*.c)
# 自動將 .c 檔名轉換為 .o 目的檔名
OBJS := $(SRCS:.c=.o)

# 函式庫名稱
LIB_NAME = libzonda.a

# --- 主要指令 ---

.PHONY: all clean install uninstall

# 預設指令 (例如執行 make)
all: $(LIB_NAME)

# 連結所有 .o 檔案，建立一個靜態函式庫
$(LIB_NAME): $(OBJS)
	@echo "AR  $@"
	@ar rcs $@ $^

# 編譯 .c 檔案的通用規則
# $< 代表第一個依賴項目 (例如 a.c)
# $@ 代表目標 (例如 a.o)
%.o: %.c
	@echo "CC  $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# 清除所有編譯產生的檔案
clean:
	@echo "Cleaning project..."
	@rm -f $(OBJS) $(LIB_NAME)

# 安裝指令
install:
	@echo "Installing header files..."
	@mkdir -p $(INSTALL_PATH)/glyph
	@mkdir -p $(INSTALL_PATH)/layout
	@mkdir -p $(INSTALL_PATH)/file_system
	@cp glyph/*.h $(INSTALL_PATH)/glyph
	@cp layout/*.h $(INSTALL_PATH)/layout
	@cp file_system/*.h $(INSTALL_PATH)/file_system
	@echo "Installation complete."
	@echo "Headers are in: $(INSTALL_PATH)"

# 解除安裝指令
uninstall:
	@echo "Uninstalling header files..."
	@rm -rf $(INSTALL_PATH)
	@echo "Uninstallation complete."
