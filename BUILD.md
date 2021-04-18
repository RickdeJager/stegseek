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
The project can be build using `cmake`. You will need the following tools:

* make
* g++
* cmake
* git


On Ubuntu, all of these packages can be installed using the following command:

```
sudo apt install git build-essential cmake
```

## Building Stegseek

First, clone this repo by calling:
```
git clone https://github.com/RickdeJager/stegseek.git
```

Next, enter the following commands to build and install Stegseek:

```bash
cd stegseek
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
sudo make install
```
