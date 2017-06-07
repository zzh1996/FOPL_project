#!/bin/bash

cd tests;
for f in *.m;
do
    cp "$f" program.txt;
    cp "$(basename "$f" .m).in" input.txt;
    echo "$f";
    ../main;
    if ! diff "$(basename "$f" .m).out" output.txt; then
        cat "$(basename "$f" .m).out" output.txt;
    fi
done
rm program.txt;
rm input.txt;
rm output.txt;
