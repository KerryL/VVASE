# makefile (VVASE)
#
# Include the common definitions
include makefile.inc

# Names of the binaries to compile
TARGET = VVASE
TARGET_DEBUG = VVASEd
TARGET_CORE = vvaseCore
TARGET_CORE_DEBUG = vvaseCored

# Directories in which to search for source files
CORE_DIRS = \
	src/core \
	src/core/analyses \
	src/core/car \
	src/core/car/subsystems \
	src/core/optimizations \
	src/core/threads \
	src/core/utilities

GUI_DIRS = \
	src/application \
	src/gui \
	src/gui/components \
	src/gui/components/editPanel \
	src/gui/components/editPanel/guiCar \
	src/gui/components/editPanel/sweep \
	src/gui/dialogs \
	src/gui/renderer \
	src/gui/renderer/3dcar \
	src/gui/renderer/primitives

# Source files
CORE_SRC = $(foreach dir, $(CORE_DIRS), $(wildcard $(dir)/*.cpp))
GUI_SRC = $(foreach dir, $(GUI_DIRS), $(wildcard $(dir)/*.cpp))
VERSION_FILE = src/application/gitHash.cpp

# Object files
TEMP_OBJS_CORE_DEBUG = $(addprefix $(OBJDIR_DEBUG),$(CORE_SRC:.cpp=.o))
TEMP_OBJS_CORE_RELEASE = $(addprefix $(OBJDIR_RELEASE),$(CORE_SRC:.cpp=.o))
TEMP_OBJS_GUI_DEBUG = $(addprefix $(OBJDIR_DEBUG),$(GUI_SRC:.cpp=.o))
TEMP_OBJS_GUI_RELEASE = $(addprefix $(OBJDIR_RELEASE),$(GUI_SRC:.cpp=.o))
VERSION_FILE_OBJ_DEBUG = $(OBJDIR_DEBUG)$(VERSION_FILE:.cpp=.o)
VERSION_FILE_OBJ_RELEASE = $(OBJDIR_RELEASE)$(VERSION_FILE:.cpp=.o)
OBJS_CORE_DEBUG = $(filter-out $(VERSION_FILE_OBJ_DEBUG),$(TEMP_OBJS_CORE_DEBUG))
OBJS_GUI_DEBUG = $(filter-out $(VERSION_FILE_OBJ_DEBUG),$(TEMP_OBJS_GUI_DEBUG))
OBJS_CORE_RELEASE = $(filter-out $(VERSION_FILE_OBJ_RELEASE),$(TEMP_OBJS_CORE_RELEASE))
OBJS_GUI_RELEASE = $(filter-out $(VERSION_FILE_OBJ_RELEASE),$(TEMP_OBJS_GUI_RELEASE))
ALL_OBJS_CORE_DEBUG = $(OBJS_CORE_DEBUG) $(VERSION_FILE_OBJ_DEBUG)
ALL_OBJS_CORE_DEBUG = $(OBJS_CORE_DEBUG) $(VERSION_FILE_OBJ_DEBUG)
ALL_OBJS_GUI_RELEASE = $(OBJS_GUI_RELEASE) $(VERSION_FILE_OBJ_RELEASE)
ALL_OBJS_GUI_RELEASE = $(OBJS_GUI_RELEASE) $(VERSION_FILE_OBJ_RELEASE)

.PHONY: all debug clean version core core_debug

all: $(TARGET)
debug: $(TARGET_DEBUG)
core: $(TARGET_CORE)
core_debug: $(TARGET_CORE_DEBUG)

$(TARGET): $(OBJS_GUI_RELEASE) version_release core
	$(MKDIR) $(BINDIR)
	$(CC) $(ALL_OBJS_RELEASE) $(LDFLAGS_RELEASE) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@

$(TARGET_DEBUG): $(OBJS_DEBUG) version_debug core_debug
	$(MKDIR) $(BINDIR)
	$(CC) $(ALL_OBJS_DEBUG) $(LDFLAGS_DEBUG) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@

$(TARGET_CORE): $(OBJS_CORE_RELEASE) version_release
	$(MKDIR) $(LIBDIR)
	$(CC) $(ALL_OBJS_RELEASE) $(LDFLAGS_RELEASE) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@
	# TODO:  Need to modify to build library

$(TARGET_CORE_DEBUG): $(OBJS_CORE_DEBUG) version_debug
	$(MKDIR) $(LIBDIR)
	$(CC) $(ALL_OBJS_DEBUG) $(LDFLAGS_DEBUG) -L$(LIBOUTDIR) $(addprefix -l,$(PSLIB)) -o $(BINDIR)$@
	# TODO:  Need to modify to build library

$(OBJDIR_RELEASE)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS_RELEASE) -c $< -o $@

$(OBJDIR_DEBUG)%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@

version_debug:
	./getGitHash.sh
	$(MKDIR) $(dir $(VERSION_FILE_OBJ_DEBUG))
	$(CC) $(CFLAGS_DEBUG) -c $(VERSION_FILE) -o $(VERSION_FILE_OBJ_DEBUG)

version_release:
	./getGitHash.sh
	$(MKDIR) $(dir $(VERSION_FILE_OBJ_RELEASE))
	$(CC) $(CFLAGS_RELEASE) -c $(VERSION_FILE) -o $(VERSION_FILE_OBJ_RELEASE)

clean:
	$(RM) -r $(OBJDIR)
	$(RM) $(BINDIR)$(TARGET)
	$(RM) $(BINDIR)$(TARGET_DEBUG)
	$(RM) -r $(LIBOUTDIR)
	$(RM) $(VERSION_FILE)
