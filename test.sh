#!/bin/bash

cd tests
for f in *.m;
do
    cp "$f" program.txt
    cp "$(basename "$f" .m).in" input.txt
    ../main > /dev/null 2>&1
    diff "$(basename "$f" .m).out" output.txt
done
rm program.txt
rm input.txt
rm output.txt