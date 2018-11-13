#!/usr/bin/env bash
make clean
make

FILES="../traces/*"
GSHAREBITS=10
GSHARE_RESULT_DIR='./results/gshare'
STATIC_REUSLT_DIR='./results/static'
mkdir -p $GSHARE_RESULT_DIR
mkdir -p $STATIC_REUSLT_DIR

for f in $FILES
do
  TESTRESULT="$(basename $f).log"
  bunzip2 -kc $f | ./predictor --gshare:$GSHAREBITS > "$GSHARE_RESULT_DIR/$TESTRESULT"
done

for f in $FILES
do
  TESTRESULT="$(basename $f).log"
  bunzip2 -kc $f | ./predictor --static > "$STATIC_REUSLT_DIR/$TESTRESULT"
done

make clean
