CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Iinclude
TARGET   := ~/Data-Structures-and-Algorithms/project/city-navigation/city_nav
BUILD    := build

SRCS := src/graph.cpp src/pathfinder.cpp src/main.cpp
OBJS := $(patsubst src/%.cpp, $(BUILD)/%.o, $(SRCS))

# ── Default target ─────────────────────────────────────────────────────────────
all: $(TARGET)

# ── Link ───────────────────────────────────────────────────────────────────────
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ -lm
	@echo "Built: $(TARGET)"

# ── Compile each .cpp → build/*.o ──────────────────────────────────────────────
$(BUILD)/%.o: src/%.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# ── Create build dir if missing ────────────────────────────────────────────────
$(BUILD):
	mkdir -p $(BUILD)

# ── Run ────────────────────────────────────────────────────────────────────────
run: all
	./$(TARGET)

# ── Clean ──────────────────────────────────────────────────────────────────────
clean:
	rm -rf $(BUILD) $(TARGET)

# ── Include auto-generated dependency files ────────────────────────────────────
-include $(OBJS:.o=.d)

.PHONY: all run clean
