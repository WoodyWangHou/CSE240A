#!/usr/bin/env bash
make clean

FILENAME='hou_wang_a53241783.zip'
rm $FILENAME
zip -X $FILENAME *.c *.h Makefile
