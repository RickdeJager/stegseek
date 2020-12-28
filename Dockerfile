from ubuntu:20.04 as build

COPY . /stegseek
WORKDIR /stegseek

RUN	apt-get update && \
	DEBIAN_FRONTEND=noninteractive TZ=UTC \
	apt-get install -y libmhash-dev libmcrypt-dev libjpeg8-dev zlib1g-dev git build-essential cmake

RUN	mkdir -p build && \
	rm -rf build/* && \
	cd build && cmake .. && make

from ubuntu:20.04

RUN	apt-get update && \
	apt-get install -y libmhash2 libmcrypt4 libjpeg8 zlib1g && \
	rm -rf /var/lib/apt/lists/*

COPY --from=build /stegseek/build/src/stegseek /usr/bin/

WORKDIR /steg


ENTRYPOINT ["stegseek"]
CMD ["--help"]

