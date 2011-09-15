# makefile (CarDesigner)
# This is a "child" makefile, which makes one of the static
# libraries (any one) required to build the executable
#
# Include the common definitions
include ../makefile.inc

# Name of the executable to compile and link
TARGET = vMath

# Directories in which to search for source files
DIRS = \
	src

# Source files
SRC = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))

# Object files
OBJS = $(addprefix $(OBJDIR),$(SRC:.cpp=.o))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(LIBOUTDIR)lib$@.a $(OBJS)
	$(RANLIB) $(LIBOUTDIR)lib$@.a

$(OBJDIR)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	# Nothing required here
