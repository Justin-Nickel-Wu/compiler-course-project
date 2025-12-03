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

# flex 输入和输出
LEX_SRC = $(SRC_DIR)/flex_input.l
LEX_OUT = $(GEN_DIR)/lexer.cpp

# 所有 src/*.c 与 src/*.cpp
SRC_C   = $(wildcard $(SRC_DIR)/*.c)
SRC_CPP = $(wildcard $(SRC_DIR)/*.cpp)

# flex 生成的 C++ 文件加入编译列表
ALL_CPP = $(SRC_CPP) $(LEX_OUT)

# 生成 obj/*.o
OBJ_CPP = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(ALL_CPP)))
OBJ_C   = $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRC_C)))

OBJS = $(OBJ_CPP) $(OBJ_C)

TARGET = $(BIN_DIR)/compiler

##############################
# Build rules
##############################

all: $(TARGET)

# flex → C++ 源码
$(LEX_OUT): $(LEX_SRC)
	@mkdir -p $(GEN_DIR)
	$(FLEX) --c++ -o $@ $<

# 编译普通 C 文件（用 g++）
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译普通 C++ 文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 编译 flex 生成的 lexer.cpp
$(OBJ_DIR)/%.o: $(GEN_DIR)/%.cpp
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
