# Create 'crack.gif'
#
# -h 18 --> 18 rows
# -w 105 --> 105 char line width
#
docker run --rm -v $PWD:/data asciinema/asciicast2gif -h 18 -w 105 -t solarized-dark crack.cast crack.gif

# Create 'seed.gif'
#
# -h 24 --> 24 rows
# -w 105 --> 105 char line width
# -s 20 --> 20x speedup. Should make this a 10 second gif
#
# [!] Before compiling this cast to a gif, you should go through and delete ~90% of the 
#     progress updates. Otherwise this tool will create a huge gif. When speed up by 20x
#     you won't notice any difference.
#
#     you might have to go through the cast and do some cleanup though.
#
docker run --rm -v $PWD:/data asciinema/asciicast2gif -h 24 -w 105 -s 20 -t solarized-dark seed.cast seed.gif
