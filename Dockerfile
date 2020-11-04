from alpine as build

COPY . /stegseek
WORKDIR /stegseek

RUN apk add \
	--no-cache \
	--repository http://dl-cdn.alpinelinux.org/alpine/edge/testing \
	libmhash-dev libmcrypt-dev jpeg-dev zlib-dev autoconf automake g++ make
RUN autoreconf -i && ./configure && cd src && make

from alpine

WORKDIR /steg

COPY --from=build /stegseek/src/stegseek /usr/bin/

RUN apk add \
	--no-cache \
	--repository http://dl-cdn.alpinelinux.org/alpine/edge/testing \
	libmhash libmcrypt libjpeg zlib libstdc++

ENTRYPOINT ["stegseek"]
CMD ["--help"]

