#!/bin/sh

PATCHLIST=patchlist.conf
DEST=docs/changes/

BASEREV="none"

mkdir -p $DEST

for TARGETREV in $(cat $PATCHLIST); do
    if [ $BASEREV = "none" ]
    then
        BASEREV=$TARGETREV
    else
        hg diff -r $BASEREV -r $TARGETREV cstrike/cfg/* cstrike/addons/sourcemod/* > $DEST/r$TARGETREV.diff
        BASEREV=$TARGETREV
    fi
done
