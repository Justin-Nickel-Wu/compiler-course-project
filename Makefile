# 编译器
CXX     = g++
CXXFLAGS = -Iinclude -Igenerated -Wall -g

# 目录
SRC_DIR = src
GEN_DIR = generated
OBJ_DIR = obj
BIN_DIR = bin

# 输入文件
BISON_SRC = $(SRC_DIR)/bison_input.y
FLEX_SRC  = $(SRC_DIR)/flex_input.l
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)

# 输出文件
BISON_C = $(GEN_DIR)/bison.c
BISON_H = $(GEN_DIR)/bison.h
LEXER_C = $(GEN_DIR)/lexer.c
TARGET  = $(BIN_DIR)/compiler

# 把 src/*.cpp 映射为 obj/*.o
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

# OBJS
OBJS = $(OBJ_DIR)/bison_input.tab.o \
       $(OBJ_DIR)/lexer.o \
	   $(OBJ_FILES)

# 默认目标
all: $(TARGET)

# ======== 生成 Bison 文件：tab.c + tab.h ========
$(BISON_C) $(BISON_H): $(BISON_SRC)
	@mkdir -p $(GEN_DIR)
	bison -d -o $(BISON_C) $(BISON_SRC)

# ======== 生成 Flex 文件：lexer.c（依赖 Bison 头文件） ========
$(LEXER_C): $(FLEX_SRC) $(BISON_H)
	@mkdir -p $(GEN_DIR)
	flex -o $(LEXER_C) $(FLEX_SRC)

# ======== 编译 Bison 输出的 .c 文件（用 C 编译器） ========
$(OBJ_DIR)/bison_input.tab.o: $(BISON_C) $(BISON_H)
	@mkdir -p $(OBJ_DIR)
	$(CXX) -Iinclude -Igenerated -Wall -g -c $(BISON_C) -o $@

# ======== 编译 lexer.c（依赖 bison_input.tab.h） ========
$(OBJ_DIR)/lexer.o: $(LEXER_C) $(BISON_H)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $(LEXER_C) -o $@

# ======== 编译 src下文件（依赖 bison_input.tab.h） ========
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(BISON_H)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -Iinclude -Igenerated -c $< -o $@

# ======== 链接 ========
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJS) -o $(TARGET)

# ======== 清理 ========
clean:
	rm -rf $(OBJ_DIR) $(GEN_DIR) $(BIN_DIR)


run: all
	$(TARGET)