#
# @version 250103
#
PROJECT_NAME = ctags_snippet
SRC_EXT = c
EXEC_EXT = .so

#
SRC_DIR = src
INC_DIR = include
LIB_DIR = lib
BUILD_DIR = build
INST_DIR = $(HOME)/.local/usr/lib/geany/plugin

#
CFLAGS = -I$(INC_DIR) -fPIC $(shell pkg-config --cflags geany)
LDFLAGS =  -shared $(shell pkg-config --libs geany)

#
SRCS = $(wildcard $(SRC_DIR)/*.$(SRC_EXT))
LIB_SRCS = $(wildcard $(LIB_DIR)/*.$(SRC_EXT))
ALL_SRCS = $(SRCS) $(LIB_SRCS)
OBJS = $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(ALL_SRCS:.$(SRC_EXT)=.o))

#
TARGET = $(BUILD_DIR)/$(PROJECT_NAME)$(EXEC_EXT)

#
all: $(TARGET)

#
$(TARGET): $(OBJS)
	@echo "リンク中: $@"
	$(CC) -o $@ $^ $(LDFLAGS)

#
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo "srcコンパイル中: $<"
	$(CC) $(CFLAGS) -c $< -o $@

#
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo "libコンパイル中: $<"
	$(CC) $(CFLAGS) -c $< -o $@

#
install:
	@echo "インストール中..."
	mkdir -p $(INST_DIR)
	cp $(TARGET) $(INST_DIR)

#
uninstall:
	@echo "アンインストール中..."
	rm -rf $(INST_DIR)/$(PROJECT_NAME)$(EXEC_EXT)

#
clean:
	@echo "クリーン中..."
	rm -rf $(BUILD_DIR)/* $(TARGET)

#
help:
	@echo "使用方法:"
	@echo "  make"
	@echo "  make install"
	@echo "  make uninstall"
	@echo "  make clean"
	@echo "  make help"

#
.PHONY: all install uninstall clean help
