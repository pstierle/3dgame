CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude -Iinclude/GLFW -Isrc/header
LDFLAGS = -Llib -lmingw32 -lglfw3 -lopengl32 -lgdi32

SRCS += $(wildcard src/*.cpp) lib/glad/glad.o

OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

TARGET = game.exe

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	for %%I in ($(subst /,\,$(OBJS:.o=.o))) do @if not "%%I"=="lib\glad\glad.o" del %%I
	del /Q $(subst /,\,$(TARGET))