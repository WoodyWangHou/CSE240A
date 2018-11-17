#!/usr/bin/env bash
# apt install valgrind
make clean
make

FILES="../traces/*"
GSHAREBITS=13
TOURNAMENT_BITS='9:10:10'
GSHARE_RESULT_DIR='./results/gshare'
STATIC_REUSLT_DIR='./results/static'
TOURNAMENT_RESULT_DIR='./results/tournament'
CUSTOM_RESULT_DIR='./results/custom'

mkdir -p $GSHARE_RESULT_DIR
mkdir -p $STATIC_REUSLT_DIR
mkdir -p $TOURNAMENT_RESULT_DIR
mkdir -p $CUSTOM_RESULT_DIR

# run experiments on gshare
#for f in $FILES
#do
  #TESTRESULT="$(basename $f).log"
  #bunzip2 -kc $f | ./predictor --gshare:$GSHAREBITS > "$GSHARE_RESULT_DIR/$TESTRESULT"
#done

# run experiments on static
#for f in $FILES
#do
  #TESTRESULT="$(basename $f).log"
  #bunzip2 -kc $f | ./predictor --static > "$STATIC_REUSLT_DIR/$TESTRESULT"
#done

# run experiments on tournament
for f in $FILES
do
  TESTRESULT="$(basename $f).log"
  bunzip2 -kc $f | ./predictor --tournament:$TOURNAMENT_BITS > "$TOURNAMENT_RESULT_DIR/$TESTRESULT"
done

# run experiments on custom
for f in $FILES
do
  TESTRESULT="$(basename $f).log"
  bunzip2 -kc $f | ./predictor --custom > "$CUSTOM_RESULT_DIR/$TESTRESULT"
done

make clean
