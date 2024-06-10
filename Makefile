CC = clang
CFLAGS = -Wall -Wextra -Iinclude -Iinclude/GLFW -Iinclude/glad -Iinclude/KHR -Iinclude/fast-noise 

SRCDIR = src
OBJDIR = obj

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

ifeq ($(OS), Windows_NT)
    CC = gcc
    LDFLAGS = -Llib/win -lglfw3 -lopengl32 -lgdi32 -lwinmm
    TARGET = game.exe
    MKDIR = mkdir
    RMDIR = if exist $(OBJDIR) rmdir /s /q $(OBJDIR)
    RM = del /f /q
    RMOBJ = powershell -Command "if (Test-Path '$(OBJDIR)') { Remove-Item -Path '$(OBJDIR)\\*' -Force }"
else
    CC = clang
    LDFLAGS = -Llib/mac -lglfw3 -framework OpenGL -framework IOKit -framework CoreVideo -framework Cocoa
    TARGET = game
    MKDIR = mkdir -p
    RMDIR = rm -rf $(OBJDIR)
    RM = rm -f
    RMOBJ = rm -f $(OBJDIR)/*.o
endif

.PHONY: all clean

all: $(OBJDIR) $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	$(MKDIR) $(OBJDIR)

clean:
	-$(RM) $(TARGET)
	-$(RMOBJ)
	-$(RMDIR)
