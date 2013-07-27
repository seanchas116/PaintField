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
cp -P $SRC/src/libs/Malachite/src/lib*.so* $DEST
cp -P $SRC/src/libs/minizip/lib*.so* $DEST
cp -P $SRC/src/libs/qtsingleapplication/lib*.so* $DEST
cp -P $SRC/src/paintfield/core/lib*.so* $DEST
cp -P $SRC/src/paintfield/extensions/lib*.so* $DEST

