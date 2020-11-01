# Stegseek

Stegseek is a lightning fast steghide cracker, build as a fork of the original steghide project. As a result it is way more efficient than other steghide crackers that simply serve as a wrapper for steghide.  
Currently it is roughly 500-800 times faster than stegcracker and can run through the entirety of `rockyou.txt` in under a minute.

Skip ahead to [Performance](#Performance) for some raw numbers.

# Installation
At the moment, Stegseek can only be installed by building from source. Don't worry though, it only requires a few simple steps.  
The install instructions assume you are running an Ubuntu system, but they will likely work for other Debian-like systems as well.

## Installing dependencies
You will need the development versions of following libraries:
* libmhash
* libmcrypt
* libjpeg
* zlib

You will also need to have `git` installed in order to clone this repo.  

All of these dependencies can be installed using the following command:

```bash
sudo apt install libmhash-dev libmcrypt-dev libjpeg-dev zlib1g-dev git
```

## Building Stegseek

First, clone this repo by calling:
```
git clone https://github.com/RickdeJager/stegseek.git
```

Next, enter the following commands to build and install Stegseek:

```bash
autoreconf -i
./configure
make
sudo make install
```

# Using Stegseek

## Cracking
The most important command to know about is `stegseek --crack`, which takes the following parameters:
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

# Performance
This is where Stegseek really shines. As promised, let's start with the `rockyou.txt` under a minute claim.  
All of these numbers are measured on a laptop with an Intel i7-7700HQ CPU @ 2.80GHz and 8 GB of ram.  






