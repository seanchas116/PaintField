#!/bin/sh

SRC=$1
DEST=$2/PaintField

if [ -e $DEST ]; then
  rm -r $DEST
fi

mkdir $DEST

cp $SRC/src/paintfield/app/PaintField $DEST
cp $SRC/src/paintfield/app/paintfield-launch.sh $DEST
cp -r $SRC/src/paintfield/app/Settings $DEST
cp -r $SRC/src/paintfield/app/Contents $DEST
cp -r $SRC/src/paintfield/app/Extensions $DEST
cp $SRC/src/libs/Malachite/src/lib*.so.1 $DEST
cp $SRC/src/libs/Minizip/lib*.so.1 $DEST
cp $SRC/src/libs/QtSingleApplication/lib*.so.1 $DEST
cp $SRC/src/paintfield/core/lib*.so.1 $DEST

chmod +x $DEST/paintfield-launch.sh

