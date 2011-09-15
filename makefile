# makefile (CarDesigner)
# This is the "parent" makefile from which all of the
# others are called.
#
# Include the common definitions
include makefile.inc

.PHONY: all clean

all:
	$(MKDIR) $(OBJDIR)
	$(MKDIR) $(BINDIR)
	$(MKDIR) $(LIBOUTDIR)
	set -e; for dir in $(PSLIB) ; do $(MAKE) -C $$dir all; done
	$(MAKE) -C CarDesigner all

clean:
	$(RM) -r $(OBJDIR) $(LIBOUTDIR) $(BINDIR)*
	set -e; for dir in $(PSLIB) ; do $(MAKE) -C $$dir clean; done
	$(MAKE) -C CarDesigner clean
