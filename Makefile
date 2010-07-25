# Script made by [SG-10]Cpt.Moore

SOURCEDIR=src
SMINCLUDES=env/include
BUILDDIR=build
SPCOMP=env/linux/bin/spcomp-1.3.4
VERSIONDUMP=./updateversion.sh

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
	$(VERSIONDUMP)
	$(SPCOMP) -i$(SOURCEDIR) -i$(SOURCEDIR)/include -i$(SMINCLUDES) -o$(BUILDDIR)/$@ $<

clean:
	@echo "Removing build directory"
	@rm -fr $(BUILDDIR)

