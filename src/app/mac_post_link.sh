# arg1: in_pwd arg2: out_pwd

rm -rf $2/PaintField.app/Contents/MacOS/Contents
rm -rf $2/PaintField.app/Contents/MacOS/Settings

cp -rf $1/Contents $2/PaintField.app/Contents/MacOS
cp -rf $1/Settings $2/PaintField.app/Contents/MacOS
cp -f $1/Info.plist $2/PaintField.app/Contents
