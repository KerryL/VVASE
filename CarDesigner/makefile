# makefile (CarDesigner)
# This is a "child" makefile, which makes one of the static
# libraries (any one) required to build the executable
#
# Include the common definitions
include ../makefile.inc

# Name of the executable to compile and link
TARGET = VVASE

# Directories in which to search for source files
DIRS = \
	src/application \
	src/environment \
	src/gui \
	src/gui/components \
	src/gui/components/edit_panel \
	src/gui/components/edit_panel/gui_car \
	src/gui/components/edit_panel/iteration \
	src/gui/dialogs \
	src/gui/renderer

# Source files
SRC = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))

# Object files
OBJS = $(addprefix $(OBJDIR),$(SRC:.cpp=.o))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@

$(OBJDIR)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	# Nothing required here
