#!/bin/bash
gnuplot -p -e 'set datafile separator ","; plot for [i=1:10] "test.csv" u 0:i w l title "Column ".i'