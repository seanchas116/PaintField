#!/bin/sh

DEST=$2/PaintField

if [ -e $DEST ]; then
  rm -r $DEST
fi

mkdir $DEST

cp $1/src/app/PaintField $DEST
cp $1/src/app/paintfield-launch.sh $DEST
cp -r $1/src/app/Settings $DEST
cp -r $1/src/app/Contents $DEST
cp $1/src/libs/Malachite/src/lib*.so.1 $DEST
cp $1/src/libs/Minizip/lib*.so.1 $DEST
cp $1/src/paintfield-core/lib*.so.1 $DEST
cp $1/src/paintfield-extension/lib*.so.1 $DEST

chmod +x $DEST/paintfield-launch.sh
