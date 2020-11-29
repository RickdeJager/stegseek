# Create 'crack.gif'
#
# -h 18 --> 18 rows
# -w 105 --> 105 char line width
#
docker run --rm -v $PWD:/data asciinema/asciicast2gif -h 18 -w 105 -t solarized-dark crack.cast crack.gif
