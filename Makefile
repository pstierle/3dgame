CC = clang
CFLAGS = -Wall -Wextra -Iinclude -Iinclude/GLFW -Iinclude/glad -Iinclude/KHR 
LDFLAGS = -Llib -lglfw3 -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa

SRCDIR = src
OBJDIR = obj

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))
TARGET = game

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