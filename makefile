# makefile (CarDesigner)
# This is the "parent" makefile from which all of the
# others are called.
#
# Include the common definitions
include makefile.inc

.PHONY: all clean

all:
	$(MKDIR) $(TOP_OBJDIR)
	$(MKDIR) $(TOP_BINDIR)
	$(MKDIR) $(TOP_LIBOUTDIR)
	set -e; for dir in $(PSLIB) ; do $(MAKE) -C $$dir all; done
	$(MAKE) -C CarDesigner all

clean:
	$(RM) -r $(TOP_OBJDIR) $(TOP_LIBOUTDIR) $(TOP_BINDIR)VVASE
	set -e; for dir in $(PSLIB) ; do $(MAKE) -C $$dir clean; done
	$(MAKE) -C CarDesigner clean
