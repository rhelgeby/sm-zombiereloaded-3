#/bin/sh

ZR_VERSION_FILE="src/zr/hgversion.h.inc"

ZR_PRODUCT_NAME="Zombie:Reloaded"
ZR_COPYRIGHT="Copyright (C) 2009  Greyscale, Richard Helgeby"
ZR_LICENSE="GNU GPL, Version 3"

echo "#define ZR_VER_PRODUCT_NAME     \"$ZR_PRODUCT_NAME\"" > $ZR_VERSION_FILE
echo "#define ZR_VER_COPYRIGHT        \"$ZR_COPYRIGHT\"" >> $ZR_VERSION_FILE
echo "#define ZR_VER_VERSION          VERSION" >> $ZR_VERSION_FILE
echo "#define ZR_VER_BRANCH           \"$(hg id -b)\"" >> $ZR_VERSION_FILE
echo "#define ZR_VER_REVISION         \"$(hg id -n):$(hg id -i)\"" >> $ZR_VERSION_FILE
echo "#define ZR_VER_LICENSE          \"$ZR_LICENSE\"" >> $ZR_VERSION_FILE
echo "#define ZR_VER_DATE             \"$(date)\"" >> $ZR_VERSION_FILE
