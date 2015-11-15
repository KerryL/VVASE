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
VERSION_FILE = src/application/vvaseConstants.cpp

# Object files
TEMP_OBJS = $(addprefix $(OBJDIR),$(SRC:.cpp=.o))
VERSION_FILE_OBJ = $(OBJDIR)$(VERSION_FILE:.cpp=.o)
OBJS = $(filter-out $(VERSION_FILE_OBJ),$(TEMP_OBJS))
ALL_OBJS = $(OBJS) $(VERSION_FILE_OBJ)

.PHONY: all clean version

all: $(TARGET)

$(TARGET): $(OBJS) version
	$(CC) $(ALL_OBJS) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) $(LDFLAGS) -o $(BINDIR)$@

$(OBJDIR)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

version:
	./../getGitHash.sh ../
	$(MKDIR) $(dir $(VERSION_FILE_OBJ))
	$(CC) $(CFLAGS) -c $(VERSION_FILE) -o $(VERSION_FILE_OBJ)

clean:
	$(RM) $(VERSION_FILE)
