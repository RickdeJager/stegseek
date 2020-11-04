# :zap: Stegseek

Stegseek is a lightning fast steghide cracker, build as a fork of the original steghide project. As a result it is _way_ faster than other crackers and can run through the entirety of **`rockyou.txt` in under a minute.**

Skip ahead to [Performance](#chart_with_upwards_trend-performance) for some raw numbers.

# :wrench: Installation

The following instructions walk you through the installation process. Alternatively, you can run Stegseek in a Docker container. Skip ahead to [Docker](#whale-docker) for instructions.

## Releases

On Ubuntu-based systems, you can use the provided `.deb` package for installation:

1. Download the latest [Stegseek release](https://github.com/RickdeJager/stegseek/releases)
1. Install the `.deb` file using `sudo apt install ./stegseek_0.1-1.deb`

## Building from source
On other systems you will have to build Stegseek yourself. See [BUILD.md](BUILD.md) for more information.  

# :arrow_forward: Using Stegseek

## Cracking
The most important command to know about is `stegseek --crack`:
```
stegseek --crack -sf [stegofile.jpg] -wl [wordlist.txt]
```

Use `stegseek --help` to get the full list of available options:
```
Stegseek version  0.1

=== Stegseek Help ===
To crack a stegofile;
stegseek --crack -sf [stegofile.jpg] -wl [wordlist.txt]

Cracking options:
 -sf, --stegofile        select stego file
 -wl, --wordlist         select the wordlist file
 -t, --threads           set the number of threads. Defaults to the number of cores.
 -v, --verbose           display detailed information
 -q, --quiet             skip performance metrics (slightly increases performance)

Use "stegseek --help -v" to include steghides help.
```

## Steghide
Stegseek includes nearly all of steghides functionality, so it can also be used to embed or extract data as normal.

# :whale: Docker
You can also run Stegseek in as Docker container:

```
docker run -it -v "$(pwd):/steg" rickdejager/stegseek --crack -sf [stegofile.jpg] -wl [wordlist.txt]
```

This does require that the wordlist and stegofile are located in current working directory, as that folder is mounted to `/steg` inside of the container.

# :chart_with_upwards_trend: Performance
This is where Stegseek really shines. As promised, let's start with the "`rockyou.txt` under a minute claim".  
All of these numbers are measured on a laptop with an Intel i7-7700HQ CPU @ 2.80GHz and 8 GB of RAM.  

## RockYou.txt
I picked the last password in `rockyou.txt` without control characters: "␣␣␣␣␣␣␣1" (7 spaces followed by '1').  
This password is on line `14344383` out of `14344391`  

```
time stegseek --crack -sf pic.jpg -wl rockyou.txt 
[i] Read the entire wordlist (14344391 words), starting cracker
[ 14344392 / 14344391 ]  (100,00%)                 
[i] --> Found passphrase: "       1"

[i] Original filename: "secret.txt"
[i] Extracting to "pic.jpg.out"

real	0m41,359s
user	5m24,819s
sys	0m0,544s
```

And there it is, over 14 million passwords in 41 seconds :heart_eyes:.

## How does this compare to other tools?

To test the performance of of other tools, I created several stego files with different passwords, taken from `rockyou.txt`. I ran each of the tools with their default settings, except Stegbrute where I increased threading for a fair comparison.

| password    | Line        | Stegseek v0.1 | Stegcracker 2.0.9 | Stegbrute v0.1.1 (-t 8) |
|-------------|-------------|---------------|-------------------|-------------------------|
| "cassandra" | 1 000       |          0.7s |              3.1s |                    0.7s |
| "kupal"     | 10 000      |          0.7s |             14.4s |                    7.1s |
| "sagar"     | 100 000     |          1.0s |           2m23.0s |                 1m21.9s |
| "budakid1"  | 1 000 000   |          3.2s | [p]      23m50.0s |                13m45.7s |
| "␣␣␣␣␣␣␣1"  | 14 344 383  |         41.4s | [p]    5h41m52.5s | [p]          3h17m38.0s |
  
[p] = projected time based on previous results.  
  
To compare the speed of Stegseek, let's look at the speed for the `1 000 000th` entries (below this value, initialization still takes too much time to get a measurement). At this scale `Stegseek` is `400+` times faster than stegcracker and `250+` times faster than Stegbrute.

