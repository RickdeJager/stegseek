# :zap: Stegseek

Stegseek is a lightning fast steghide cracker, built as a fork of the original steghide project. As a result it is _thousands of times_ faster than other crackers and can run through the entirety of **`rockyou.txt` in under 5 seconds.**

Skip ahead to [Performance](#chart_with_upwards_trend-performance) for some raw numbers.

# :wrench: Installation

The following instructions walk you through the installation process. Alternatively, you can run Stegseek in a Docker container. Skip ahead to [Docker](#whale-docker) for instructions.

## Releases

On Ubuntu and Debian-based systems, you can use the provided `.deb` package for installation:

1. Download the latest [Stegseek release](https://github.com/RickdeJager/stegseek/releases)
1. Install the `.deb` file using `sudo apt install ./stegseek_0.3-1.deb`

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
Stegseek version  0.3

=== Stegseek Help ===
To crack a stegofile;
stegseek --crack -sf [stegofile.jpg] -wl [wordlist.txt]

Cracking options:
 -sf, --stegofile        select stego file
 -wl, --wordlist         select the wordlist file
 -xf, --extractfile      select file name for extracted data
 -t, --threads           set the number of threads. Defaults to the number of cores.
 -v, --verbose           display detailed information
 -q, --quiet             skip performance metrics (slightly increases performance)

Use "stegseek --help -v" to include steghides help.
```

## Steghide
Stegseek includes nearly all of steghides functionality, so it can also be used to embed or extract data as normal.

# :whale: Docker
You can also run Stegseek as Docker container:

```
docker run -it -v "$(pwd):/steg" rickdejager/stegseek --crack -sf [stegofile.jpg] -wl [wordlist.txt]
```

This does require that the wordlist and stegofile are located in current working directory, as that folder is mounted to `/steg` inside of the container.

# :chart_with_upwards_trend: Performance
This is where Stegseek really shines. As promised, let's start with the "`rockyou.txt` in just 5 seconds" claim.  
All of these numbers are measured on a laptop with an Intel i7-7700HQ CPU @ 2.80GHz and 8 GB of RAM.  

## RockYou.txt
I picked the last password in `rockyou.txt` without control characters: "␣␣␣␣␣␣␣1" (7 spaces followed by '1').  
This password is on line `14344383` out of `14344391`  

```
time stegseek --crack -wl rockyou.txt -sf 7spaces1.jpg
Stegseek version  0.3
[i] Read the entire wordlist (14344391 words), starting cracker
[ 14299660 / 14344391 ]  (99,69%)                 
[i] --> Found passphrase: "       1"

[i] Original filename: "secret"
[i] Extracting to "7spaces1.jpg.out"

real	0m4,397s
user	0m27,948s
sys	0m0,220s
```

And there it is, over 14 million passwords in less than 5 seconds :heart_eyes:.

## How does this compare to other tools?

To test the performance of of other tools, I created several stego files with different passwords, taken from `rockyou.txt`. I ran each of the tools with their default settings, except Stegbrute where I increased threading for a fair comparison.

| password    | Line        | Stegseek v0.3 | Stegcracker 2.0.9 | Stegbrute v0.1.1 (-t 8) |
|-------------|-------------|---------------|-------------------|-------------------------|
| "cassandra" | 1 000       |          1.0s |              3.1s |                    0.7s |
| "kupal"     | 10 000      |          1.0s |             14.4s |                    7.1s |
| "sagar"     | 100 000     |          1.1s |           2m23.0s |                 1m21.9s |
| "budakid1"  | 1 000 000   |          1.2s | [p]      23m50.0s |                13m45.7s |
| "␣␣␣␣␣␣␣1"  | 14 344 383  |          4.4s | [p]    5h41m52.5s | [p]          3h17m38.0s |
  
[p] = projected time based on previous results.  
  
To compare the speed of each tool, let's look at the last row of the table (otherwise Stegseek finishes before all threads have started).  
At this scale Stegseek is over **4500** times faster than Stegcracker and over **2500** times faster than Stegbrute.


# :notebook: Changelog

## v0.3
2020-11-08  
improvements:  
* Made threading lock-free s.t. multiple threads are used more efficiently
* Added `-xf` argument to `--crack`, used to specify the location of the extracted file
* Made the .deb package compatible with Debian/Kali
* cracks `rockyou.txt` in 4 seconds

## v0.2
2020-11-05  
improvements:  
* Give up on a passphrase once a single bit of the magic fails to decode
* Removed costly BitString / EmbData allocations
* Improved performance of the selector
* cracks `rockyou.txt` in 5 seconds.

## v0.1
2020-11-04  
Initial release, features:  
* Only loads the stego file once, unlike conventional crackers
* Workpool based threading
* Attempts to crack the first 3 bytes first, before attempting full decryption
* Added .deb package and docker container
* cracks `rockyou.txt` in 41 seconds.
