# 编译器
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# 源文件目录
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# 源文件
SRCS = $(SRCDIR)/main.cpp $(SRCDIR)/NFA.cpp $(SRCDIR)/DFA.cpp $(SRCDIR)/GraphStruct.cpp

# 目标文件
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
TARGET = $(BINDIR)/main

# 确保目录存在
$(shell mkdir -p $(OBJDIR) $(BINDIR))

# 默认目标
all: $(TARGET)

# 链接目标
$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $^

# 编译规则
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理命令
clean:
	@rm -rf $(OBJDIR)/*.o $(TARGET)

# 运行命令
run: $(TARGET)
	@./$(TARGET)