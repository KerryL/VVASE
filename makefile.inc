# makefile.inc (VVASE)
# This file contains all of the environment definitions
# common to each of the makefiles within the project.
#

# Include directories that are not already on the path
# DO NOT include the -I prefix to these paths - it will
# be added automatically
INCDIRS_TEMP = \
	$(CURDIR)/include

INCDIRS = $(addprefix -I,$(INCDIRS_TEMP))

# For 3rd party files - will be included as system files so warnings
# are ignored.  These macros must be defined in your bashrc file.
LIB_INCDIRS_TEMP = \
	$(EIGEN)

LIB_INCDIRS = $(addprefix -isystem,$(LIB_INCDIRS_TEMP))

# Library directories that are not already on the path
# DO NOT include the -L prefix to these paths - it will
# be added automatically
LIBDIRS_TEMP = \


LIBDIRS = $(addprefix -L,$(LIBDIRS_TEMP))

# Libraries to link against
# DO NOT include the -l prefix to these libraries - it
# will be added automatically
LIBS_TEMP = \


LIBS = $(addprefix -l,$(LIBS_TEMP))

# Static libraries to be built before the executable
# MUST be listed in order of dependence (i.e. first
# library must not be needed by other libraries and
# it must need information contained in the following
# libraries).
PSLIB = \
	vvaseCore

# Compiler to use
CC = g++

# Archiver to use
AR = ar rcs
RANLIB = ranlib

# Compiler flags
CFLAGS = -Wall -Wextra $(LIB_INCDIRS) $(INCDIRS) -DwxUSE_GUI=1 -Wno-unused-local-typedefs
CFLAGS_RELEASE = $(CFLAGS) -O2 `pkg-config --cflags lp2d`
CFLAGS_DEBUG = $(CFLAGS) -g `pkg-config --cflags lp2d_d`

# Linker flags
LDFLAGS = $(LIBDIRS) $(LIBS)
LDFLAGS_RELEASE = $(LDFLAGS) `pkg-config --libs lp2d`
LDFLAGS_DEBUG = $(LDFLAGS) `pkg-config --libs lp2d_d`

# Object file output directory
OBJDIR = $(CURDIR)/.obj/
OBJDIR_DEBUG = $(OBJDIR)debug/
OBJDIR_RELEASE = $(OBJDIR)release/

# Binary file output directory
BINDIR = $(CURDIR)/bin/

# Library output directory
LIBOUTDIR = $(CURDIR)/.lib/

# Method for creating directories
MKDIR = mkdir -p

# Method for removing files
RM = rm -f
