# Build instructions
## Installing dependencies
Running the following command will install all required dependencies:

```bash
sudo apt install libmhash-dev libmcrypt-dev libjpeg-dev zlib1g-dev git autoconf build-essential
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

