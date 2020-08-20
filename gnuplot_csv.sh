#!/bin/sh

CSV_FILE=$1

gnuplot << EOF
set datafile separator ', '
set autoscale fix
set key autotitle columnhead

# set key autotitle columnhead # use the first line as title
# set ylabel "First Y Units" # label for the Y axis
# set xlabel 'Time' # label for the X axis

plot "$CSV_FILE" using 0:2 with lines, \
              '' using 0:3 with lines, \
              '' using 0:4 with lines, \
              '' using 0:5 with lines, \
              '' using 0:6 with lines, \
              '' using 0:7 with lines, \
              '' using 0:9 with lines, \
              '' using 0:10 with lines

# plot "$CSV_FILE" using 1
pause 30
reread
EOF