# Build instructions

The build instructions will vary somewhat based on your host system. Ubuntu is used as an example.

## Installing dependencies
In any case you will need to following dependencies:

* libmhash-dev
* libmcrypt-dev
* libjpeg-dev
  * Can either the independent JPEG version or the libjpeg-turbo version
  * The libjpeg version must be below 9 
* zlib1g-dev

On Ubuntu, all of these packages can be installed using the following command:

```
sudo apt install libmhash-dev libmcrypt-dev libjpeg-dev zlib1g-dev
```

## build tools
You will also need the "autotools" suite of build tools. Depending on your distribution, these might be bundled in a single package (such as `build-essential` on Ubuntu).  
If there is not a bundle available, you should install the following tools:  

* make
* g++
* autoconf
* git


On Ubuntu, all of these packages can be installed using the following command:

```
sudo apt install git autoconf build-essential
```

## Building Stegseek

First, clone this repo by calling:
```
git clone https://github.com/RickdeJager/stegseek.git
```

Next, enter the following commands to build and install Stegseek:

```bash
cd stegseek
autoreconf -i
./configure
make
sudo make install
```
