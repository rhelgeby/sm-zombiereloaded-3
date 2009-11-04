#!/bin/sh

cd src/

echo "$(hg tip)\n"

LINES_MAIN=`wc *.sp -l | cut -d ' ' -f1`
LINES_OTHER=`wc zr/*.* -l | tail -n1 | sed 's/^ *\(.*\) *$/\1/' | cut -d ' ' -f1`
LINES_PLAYERCLASSES=`wc zr/playerclasses/*.* -l | tail -n1 | sed 's/^ *\(.*\) *$/\1/' | cut -d ' ' -f1`
LINES_SOUNDEFFECTS=`wc zr/soundeffects/*.* -l | tail -n1 | sed 's/^ *\(.*\) *$/\1/' | cut -d ' ' -f1`
LINES_VISUALEFFECTS=`wc zr/visualeffects/*.* -l | tail -n1 | sed 's/^ *\(.*\) *$/\1/' | cut -d ' ' -f1`
LINES_VOLFEATURES=`wc zr/volfeatures/*.* -l | tail -n1 | sed 's/^ *\(.*\) *$/\1/' | cut -d ' ' -f1`
LINES_WEAPONS=`wc zr/weapons/*.* -l | tail -n1 | sed 's/^ *\(.*\) *$/\1/' | cut -d ' ' -f1`

LINES_TOTAL="$(($LINES_MAIN + $LINES_OTHER + $LINES_PLAYERCLASSES + $LINES_SOUNDEFFECTS + $LINES_VISUALEFFECTS + $LINES_VOLFEATURES + $LINES_WEAPONS))"

echo "Number of lines:"
echo "$LINES_MAIN\tmain sp"
echo "$LINES_OTHER\tother"
echo "$LINES_PLAYERCLASSES\tplayerclasses"
echo "$LINES_SOUNDEFFECTS\tsoundeffects"
echo "$LINES_VISUALEFFECTS\tvisualeffects"
echo "$LINES_VOLFEATURES\tvolfeatures"
echo "$LINES_WEAPONS\tweapons"

echo "\nTotal:"
echo "$LINES_TOTAL"
