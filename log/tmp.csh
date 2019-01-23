#!/bin/csh

gnuplot<<END
set xlabel "test"
set ylabel "score [percentage]"
set title "improvement monitor for bee"
set grid; set yrange [0 to 110]
set style data linespoints
plot "states.bee.plt"
END
