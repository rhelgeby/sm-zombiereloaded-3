# Script made by [SG-10]Cpt.Moore
# Note that this script will convert line endings in source files to LF.

SOURCEDIR=src
SMINCLUDES=env/include
BUILDDIR=build
SPCOMP_LINUX=env/linux/bin/spcomp-1.7.0
SPCOMP_DARWIN=env/darwin/bin/spcomp-1.7.0
DOS2UNIX_LINUX=dos2unix -p
DOS2UNIX_DARWIN=env/darwin/bin/dos2unix -p
VERSIONDUMP=./updateversion.sh

OS = $(shell uname -s)
ifeq "$(OS)" "Darwin"
	SPCOMP = $(SPCOMP_DARWIN)
	DOS2UNIX = $(DOS2UNIX_DARWIN)
else
	SPCOMP = $(SPCOMP_LINUX)
	DOS2UNIX = $(DOS2UNIX_LINUX)
endif

vpath %.sp $(SOURCEDIR)
vpath %.inc $(SOURCEDIR)/zr
vpath %.smx $(BUILDDIR)

SOURCEFILES=$(SOURCEDIR)/*.sp
OBJECTS=$(patsubst %.sp, %.smx, $(notdir $(wildcard $(SOURCEFILES))))

all: prepare $(OBJECTS)

prepare: prepare_newlines prepare_builddir

prepare_newlines:
	@echo "Removing windows newlines"
	@find $(SOURCEDIR)  -name \*.inc -exec $(DOS2UNIX) '{}' \;
	@find $(SOURCEDIR)  -name \*.sp  -exec $(DOS2UNIX) '{}' \;
	@find $(SMINCLUDES) -name \*.inc -exec $(DOS2UNIX) '{}' \;

prepare_builddir:
	@echo "Creating build directory"
	@mkdir -p $(BUILDDIR)

%.smx: %.sp
	$(VERSIONDUMP)
	$(SPCOMP) -i$(SOURCEDIR) -i$(SMINCLUDES) -o$(BUILDDIR)/$@ $<

clean:
	@echo "Removing build directory"
	@rm -fr $(BUILDDIR)
