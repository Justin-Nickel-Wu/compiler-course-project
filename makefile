# 编译器
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# 运行时传参（可通过 `make run ARGS="..."` 或 `make run arg1 arg2` 传入）
ARGS ?=

# 支持 `make run arg1 arg2` 的形式：把额外的命令行目标作为运行参数
# 当用户执行 `make run a b` 时，`ARGS_FROM_GOALS` 将等于 `a b`。
ARGS_FROM_GOALS := $(filter-out run,$(MAKECMDGOALS))

# 仅当存在位置参数时，添加一个空的通配规则来避免 make 将这些词视为要构建的目标。
# 这样做可以防止在 `make run`（无参数）时出现意外行为。
ifneq ($(ARGS_FROM_GOALS),)
%:
	@:
endif

# 源文件目录
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# 源文件（自动包含 `src/` 下所有 .cpp）
SRCS := $(wildcard $(SRCDIR)/*.cpp)

# 目标文件
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
TARGET = $(BINDIR)/main

# 确保目录存在
$(shell mkdir -p $(OBJDIR) $(BINDIR))

# 默认目标
all: $(TARGET)

.PHONY: all clean run

# 链接目标
$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $^

# 编译规则
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理命令
clean:
	@rm -rf $(OBJDIR)/*.o $(TARGET)

# 运行命令（支持传参）
run: $(TARGET)
	@./$(TARGET) $(if $(ARGS_FROM_GOALS),$(ARGS_FROM_GOALS),$(ARGS))