#!/bin/bash

echo "Starting parsing of .col file..."
./hamilton "$1" > out.cnf
echo "Parsing finished."

echo "Starting minisat..."
./minisat out.cnf out.sat -no-luby -rinc=1.5 -phase-saving=0 -rnd-freq=0.02 > /dev/null
EXITCODE=$?
echo "Solving finished."

echo


if [[ $EXITCODE -eq 10 ]]
then
	echo "SATISFIABLE"
	echo "Cycle:"
	./parsesatouttocycle out.sat
else if [[ $EXITCODE -eq 20 ]]; then
	echo "UNSATISFIABLE"
fi
fi

echo
echo "Bye."
exit $EXITCODE