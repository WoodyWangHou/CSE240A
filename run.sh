#!/usr/bin/env bash
docker run --rm -it -v "$(pwd)"/src:/home/src -v "$(pwd)"/traces:/home/traces prodromou87/ucsd_cse240a
