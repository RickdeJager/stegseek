# Create 'crack.gif'
#
# -h 15 --> 15 rows
# -w 105 --> 105 char line width
#
docker run --rm -v $PWD:/data asciinema/asciicast2gif -h 15 -w 105 -t solarized-dark crack.cast crack.gif

# Create 'seed.gif'
#
# -h 17 --> 17 rows
# -w 105 --> 105 char line width
# -s 3 --> 3x speedup. Should make this a ~7 second gif
#
# [!] Before compiling this cast to a gif, you should go through and delete ~90% of the 
#     progress updates. Otherwise this tool will create a huge gif. When speed up by 5x
#     you won't notice any difference.
#
#     you might have to go through the cast and do some cleanup though.
#
docker run --rm -v $PWD:/data asciinema/asciicast2gif -h 17 -w 105 -s 3 -t solarized-dark seed.cast seed.gif
