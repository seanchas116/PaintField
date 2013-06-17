#!/bin/sh

SRC=$1
DEST=$2/PaintField

if [ -e $DEST ]; then
  rm -r $DEST
fi

mkdir $DEST

cp $SRC/src/paintfield/app/PaintField $DEST
cp -r $SRC/src/paintfield/app/Settings $DEST
cp -r $SRC/src/paintfield/app/Contents $DEST
cp -r $SRC/src/paintfield/app/Translations $DEST
cp $SRC/src/libs/Malachite/src/lib*.so.1 $DEST
cp $SRC/src/libs/minizip/lib*.so.1 $DEST
cp $SRC/src/libs/qtsingleapplication/lib*.so.1 $DEST
cp $SRC/src/paintfield/core/lib*.so.1 $DEST
cp $SRC/src/paintfield/extensions/lib*.so.1 $DEST

