#!/bin/bash

rm out.sat &> /dev/null

./minisat <(./hamilton "$1") out.sat -no-luby -rinc=1.5 -phase-saving=0 -rnd-freq=0.02 &> /dev/null

./parsesatouttocycle out.sat

EXITCODE=$?

exit $EXITCODE