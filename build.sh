#!/bin/sh

RELEASEDIR=release
BUILDDIR=build
VERSION="zombiereloaded-3.0-dev"
REVISION=$(hg id -n)
ZIPFILE=$VERSION-r$REVISION.zip

PLUGINFILES="cstrike/*"
DOCS="docs/*"
DOCS_DEST=$RELEASEDIR/zrdocs
PLUGINFILE=zombiereloaded.smx
PLUGINDIR=$RELEASEDIR/addons/sourcemod/plugins
ZRTOOLS_SOURCE=/home/zrdev/archive/zrtools
EXTENSIONDIR=$RELEASEDIR/addons/sourcemod/extensions

MAKEPATCH=false


# Clean release directory.
rm -rf $RELEASEDIR
echo "Cleaned release directory."


# Exit if cleaning only.
if [ "$1" = "clean" ]
then
    exit 0
fi


# Check if patch mode is enabled.
if [ "$1" = "patch" ]
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
        
        ZIPFILE=$VERSION-patch-r$PATCHREV-r$REVISION.zip
    else
        echo "Missing base revision number. Usage: build.sh patch <base rev>"
        exit 1
    fi
fi


# Make release directory.
mkdir -p $RELEASEDIR


# Clean and compile plugin.
make clean
make


# Check if the plugin is built.
if [ ! -e $BUILDDIR/$PLUGINFILE ]
then
    echo "Cannot build release package, plugin build failed. Missing file '$BUILDDIR/$PLUGINFILE'."
    exit 1
fi


# Copy files.
echo "Copying documentation..."
mkdir -p $DOCS_DEST
cp -r $DOCS $DOCS_DEST

echo "Copying plugin binary..."
mkdir -p $PLUGINDIR
cp -r $BUILDDIR/$PLUGINFILE $PLUGINDIR/$PLUGINFILE

if [ $MAKEPATCH = "false" ]
then
    # Copy all files.
    echo "Copying plugin files..."
    cp -r $PLUGINFILES $RELEASEDIR
    
    echo "Copying extension binaries..."
    mkdir -p $EXTENSIONDIR
    cp $ZRTOOLS_SOURCE/zrtools.ext.so $EXTENSIONDIR
    cp $ZRTOOLS_SOURCE/zrtools.ext.dll $EXTENSIONDIR
else
    # Copy only changed files.
    CHANGEDFILES=$(hg status --rev $PATCHREV | grep "cstrike/" | cut -d ' ' -f2 | cut -d '/' -f2-)
    
    echo "Copying plugin files..."
    cd cstrike
    cp --parents $CHANGEDFILES "../$RELEASEDIR"
    cd ..
fi


# Make release package.
echo "Compressing files..."
cd $RELEASEDIR
zip -r $ZIPFILE *

echo "Release package available at $RELEASEDIR/$ZIPFILE."
