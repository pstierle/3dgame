CC = clang
CFLAGS = -Wall -Wextra -Iinclude -Iinclude/GLFW -Iinclude/glad -Iinclude/KHR 

SRCDIR = src
OBJDIR = obj

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
    LDFLAGS = -Llib/mac -lglfw3 -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa
	TARGET = game
else ifeq ($(UNAME_S), Windows_NT)
    LDFLAGS = -Llib/win -lglfw3 -lopengl32 -lgdi32 -lwinmm
	TARGET = game.exe
else
    $(error Unsupported operating system)
endif

.PHONY: all clean

all: $(OBJDIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(TARGET) $(OBJDIR)/*.o
	rm -rf $(OBJDIR)