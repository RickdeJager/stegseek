from ubuntu:20.04 as build

COPY . /stegseek
WORKDIR /stegseek

RUN apt-get update && \
	apt-get install -y libmhash-dev libmcrypt-dev libjpeg8-dev zlib1g-dev git autoconf build-essential
RUN autoreconf -i && ./configure && cd src && make


from ubuntu:20.04

RUN apt-get update && \
	apt-get install -y libmhash2 libmcrypt4 libjpeg8 zlib1g

COPY --from=build /stegseek/src/stegseek /usr/bin/

WORKDIR /steg


ENTRYPOINT ["stegseek"]
CMD ["--help"]

