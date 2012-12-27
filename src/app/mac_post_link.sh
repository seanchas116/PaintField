# arg1: in_pwd arg2: out_pwd

cp -rf $1/Contents $2/PaintField.app/Contents/MacOS
cp -f $1/Info.plist $2/PaintField.app/Contents
