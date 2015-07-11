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
	src/gui/components/editPanel \
	src/gui/components/editPanel/guiCar \
	src/gui/components/editPanel/iteration \
	src/gui/dialogs \
	src/gui/renderer

# Source files
SRC = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))

# Object files
OBJS = $(addprefix $(OBJDIR),$(SRC:.cpp=.o))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) $(LDFLAGS) -o $(BINDIR)$@

$(OBJDIR)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	# Nothing required here
