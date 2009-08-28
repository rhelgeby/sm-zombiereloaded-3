#!/bin/sh

RELEASEDIR=release
BUILDDIR=build
ZIPFILE=$(hg id -b)-$(hg id -n).zip

PLUGINFILES=cstrike
DOCS=docs
PLUGINFILE=zombiereloaded.smx
PLUGINDIR=cstrike/addons/sourcemod/plugins

# Clean release directory if specified and exit.
if [ "$1" = "clean" ]
then
    rm -r $RELEASEDIR
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
echo "Copying files..."
cp -r "$PLUGINFILES" "$RELEASEDIR"
cp -r "$DOCS" "$RELEASEDIR"

mkdir -p $RELEASEDIR/$PLUGINDIR
cp -r $BUILDDIR/$PLUGINFILE $RELEASEDIR/$PLUGINDIR/$PLUGINFILE
cp -r license.txt $RELEASEDIR/$DOCS/license.txt

# Make release package.
echo "Compressing files..."
cd $RELEASEDIR
zip -r $ZIPFILE *

echo "Release package available at $RELEASEDIR/$ZIPFILE."
