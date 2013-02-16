#!/bin/sh
 
DIR=$(cd $(dirname $0);pwd)

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/$DIR

$DIR/PaintField $@

