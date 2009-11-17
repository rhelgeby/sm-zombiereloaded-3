#!/bin/sh

RELEASEDIR=release
BUILDDIR=build
ZIPFILE=$(hg id -b)-$(hg id -n).zip

PLUGINFILES="cstrike/*"
DOCS="docs/*"
DOCS_DEST=$RELEASEDIR/zrdocs
PLUGINFILE=zombiereloaded.smx
PLUGINDIR=$RELEASEDIR/addons/sourcemod/plugins
ZRTOOLS_SOURCE=/home/zrdev/archive/zrtools
EXTENSIONDIR=$RELEASEDIR/addons/sourcemod/extensions

# Clean release directory if specified and exit.
if [ "$1" = "clean" ]
then
    rm -rf $RELEASEDIR
    echo "Cleaned release directory."
    exit 0
fi

# Make release directory.
mkdir -p $RELEASEDIR

# Check if the plugin is built.
if [ ! -e $BUILDDIR/$PLUGINFILE ]
then
    echo "Cannot build release package, plugin is not built. Missing file '$BUILDDIR/$PLUGINFILE'."
    exit 1
fi

# Copy files.
echo "Copying plugin files..."
cp -r $PLUGINFILES $RELEASEDIR

echo "Copying documentation..."
mkdir -p $DOCS_DEST
cp -r $DOCS $DOCS_DEST

echo "Copying plugin binaries..."
mkdir -p $PLUGINDIR
cp -r $BUILDDIR/$PLUGINFILE $PLUGINDIR/$PLUGINFILE

echo "Copying extension binaries..."
mkdir -p $EXTENSIONDIR
cp $ZRTOOLS_SOURCE/zrtools.ext.so $EXTENSIONDIR
cp $ZRTOOLS_SOURCE/zrtools.ext.dll $EXTENSIONDIR

# Make release package.
echo "Compressing files..."
cd $RELEASEDIR
zip -r $ZIPFILE *

echo "Release package available at $RELEASEDIR/$ZIPFILE."
