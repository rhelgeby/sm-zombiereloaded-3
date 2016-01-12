#!/bin/sh

VERSION="zombiereloaded-3.1"
REVISION=$(git rev-parse --short HEAD)

SYNTAX="Usage: $0 [--patch <base rev>]"

# Source paths
BUILDDIR=build
PLUGINFILE=zombiereloaded.smx
PLUGINFILES="cstrike/*"
DOCS="docs/*"
SOURCEDIR="src/*"

# Destination paths
RELEASEDIR=release
SOURCE_DEST=$RELEASEDIR/addons/sourcemod/scripting
DOCS_DEST=$RELEASEDIR/zrdocs
PLUGINDIR=$RELEASEDIR/addons/sourcemod/plugins
EXTENSIONDIR=$RELEASEDIR/addons/sourcemod/extensions
ZIPFILE=$VERSION-$REVISION.zip

MAKEPATCH=false

# Clean build and release directory.
make clean
rm -rf $RELEASEDIR
echo "Cleaned build and release directory."

# Exit if cleaning only.
if [ "$1" = "--clean" ]
then
    exit 0
fi

# Check if patch mode is enabled.
if [ "$1" = "--patch" ]
then
    if [ "$2" ]
    then
        MAKEPATCH=true
        PATCHREV="$2"
        
        if [ "$2" = $REVISION ]
        then
            echo "No changes since base revision."
            exit 1
        fi
        
        ZIPFILE=$VERSION-patch-$PATCHREV-$REVISION.zip
    else
        echo "Missing base revision number. $SYNTAX"
        exit 1
    fi
fi

# Make release directory.
mkdir -p $RELEASEDIR

# Compile plugin.
make

# Check if the plugin is built.
if [ ! -e $BUILDDIR/$PLUGINFILE ]
then
    echo "Cannot build release package, plugin build failed. Missing file $BUILDDIR/$PLUGINFILE."
    exit 1
fi

# Rebuild hgversion.h.inc for unofficial builds.
sh updateversion.sh --unofficial

if [ $MAKEPATCH = "false" ]
then
    # Copy all files.
    echo "Copying plugin files..."
    cp -r $PLUGINFILES $RELEASEDIR
else
    # Make diff files with config changes for each patch.
    sh diffgen.sh
    
    # Copy only changed files.
    CHANGEDFILES=$(hg status --rev $PATCHREV | grep "cstrike/" | cut -d ' ' -f2 | cut -d '/' -f2-)
    
    echo "Copying plugin files..."
    cd cstrike
    cp --parents $CHANGEDFILES "../$RELEASEDIR"
    cd ..
fi

# Copy  files.
echo "Copying plugin binary..."
mkdir -p $PLUGINDIR
cp -r $BUILDDIR/$PLUGINFILE $PLUGINDIR/$PLUGINFILE

echo "Copying plugin source code..."
mkdir -p $SOURCE_DEST
cp -r $SOURCEDIR $SOURCE_DEST

echo "Copying documentation..."
mkdir -p $DOCS_DEST
cp -r $DOCS $DOCS_DEST

# Make release package.
echo "Compressing files..."
cd $RELEASEDIR
zip -r $ZIPFILE *

echo "Release package available at $RELEASEDIR/$ZIPFILE."
