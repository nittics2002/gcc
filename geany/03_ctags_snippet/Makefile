#
#
#
PROJECT_NAME = my_project
SRC_EXT = c
EXEC_EXT = .out

#
SRC_DIR = src
INC_DIR = include
LIB_DIR = lib
BUILD_DIR = build
INST_DIR = $(HOME)/.local/usr/lib/

#
CFLAGS = -Wall -Wextra -I$(INC_DIR)
LDFLAGS =  # ここにリンク時のオプションを追加

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
	@echo "コンパイル中: $<"
	$(CC) $(CFLAGS) -c $< -o $@

#
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo "コンパイル中: $<"
	$(CC) $(CFLAGS) -c $< -o $@

#
install:
	mkdir -p $(INST_DIR)
	cp $(TARGET) $(INST_DIR)

#
uninstall:
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
