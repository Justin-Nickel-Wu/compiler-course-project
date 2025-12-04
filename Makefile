##############################
# Tools
##############################

CXX = g++
FLEX = flex

CXXFLAGS = -Iinclude -Wall -g

##############################
# Directories
##############################

SRC_DIR = src
GEN_DIR = generated
OBJ_DIR = obj
BIN_DIR = bin

##############################
# Files
##############################

# flex 输入和输出（C 版本输出 .c 文件）
LEX_SRC = $(SRC_DIR)/flex_input.l
LEX_OUT = $(GEN_DIR)/lexer.c

# 所有 src/*.c 与 src/*.cpp（用户自己写的源代码）
SRC_C   = $(wildcard $(SRC_DIR)/*.c)
SRC_CPP = $(wildcard $(SRC_DIR)/*.cpp)

# 编译列表（加入生成的 lexer.c）
ALL_C   = $(SRC_C) $(LEX_OUT)
ALL_CPP = $(SRC_CPP)

# => 编译成 .o
OBJ_C   = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(ALL_C)))
OBJ_CPP = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(ALL_CPP)))

OBJS = $(OBJ_C) $(OBJ_CPP)

TARGET = $(BIN_DIR)/compiler

##############################
# Build rules
##############################

all: $(TARGET)

# flex → C 源码
$(LEX_OUT): $(LEX_SRC)
	@mkdir -p $(GEN_DIR)
	$(FLEX) -o $@ $<

# 编译 *.c（生成 *.o）
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译 *.cpp（生成 *.o）
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译 flex 生成的 lexer.c
$(OBJ_DIR)/%.o: $(GEN_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

##############################
# Linking
##############################

$(TARGET): $(LEX_OUT) $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $(OBJS)

##############################
# Commands
##############################

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(GEN_DIR)/*
	rm -rf $(BIN_DIR)/*

.PHONY: all clean run
