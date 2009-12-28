#!/bin/sh

SYNTAX="Usage: $0 <base rev> <target rev>"

if [ "$1" ]
then
    BASEREV="$1"
    
    if [ "$2" ]
    then
        TARGETREV="$2"
    else
        echo "Missing target revision. $SYNTAX"
        exit 1
    fi
else
    echo "$SYNTAX"
    exit 1
fi

hg diff -r $BASEREV -r $TARGETREV cstrike/cfg/* cstrike/addons/sourcemod/* > docs/changes.diff
