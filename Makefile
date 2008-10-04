
SOURCEDIR=src
BUILDDIR=build
SPCOMP=bin/spcomp

vpath %.sp $(SOURCEDIR)
vpath %.inc $(SOURCEDIR)/include
vpath %.smx $(BUILDDIR)

SOURCEFILES=$(SOURCEDIR)/*.sp
OBJECTS=$(patsubst %.sp, %.smx, $(notdir $(wildcard $(SOURCEFILES))))

all: prepare $(OBJECTS)

prepare: prepare_newlines prepare_builddir

prepare_newlines:
	@echo "Removing windows newlines"
	@find $(SOURCEDIR) -name \*.inc -exec dos2unix -p '{}' \;
	@find $(SOURCEDIR)  -name \*.sp -exec dos2unix -p '{}' \;

prepare_builddir:
	@echo "Creating build directory"
	@mkdir -p $(BUILDDIR)

%.smx: %.sp
	$(SPCOMP) -i$(SOURCEDIR) -i$(SOURCEDIR)/include -o$(BUILDDIR)/$@ $<

clean:
	@echo "Removing build directory"
	@rm -fr $(BUILDDIR)

