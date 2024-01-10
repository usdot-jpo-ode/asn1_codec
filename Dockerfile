# === BUILDER IMAGE ===
FROM alpine:3.12 as builder
USER root
WORKDIR /asn1_codec
VOLUME ["/asn1_codec_share"]

# update the package manager
RUN apk update

# add build dependencies
RUN apk add --upgrade --no-cache --virtual .build-deps \
    cmake \
    g++ \
    make \
    bash \
    automake \
    libtool \
    autoconf \
    librdkafka \
    librdkafka-dev

# Install pugixml
ADD ./pugixml /asn1_codec/pugixml
RUN cd /asn1_codec/pugixml && mkdir -p build && cd build && cmake .. && make && make install

# Build and install asn1c submodule
ADD ./asn1c /asn1_codec/asn1c
RUN apt-get -y install automake libtool flex
RUN wget http://launchpadlibrarian.net/140087283/libbison-dev_2.7.1.dfsg-1_amd64.deb
RUN wget http://launchpadlibrarian.net/140087282/bison_2.7.1.dfsg-1_amd64.deb
RUN dpkg -i libbison-dev_2.7.1.dfsg-1_amd64.deb
RUN dpkg -i bison_2.7.1.dfsg-1_amd64.deb
RUN apt-mark hold libbison-dev bison
RUN cd asn1c && test -f configure || autoreconf -iv && ./configure && make && make install

# Generate ASN.1 API.
RUN export LD_LIBRARY_PATH=/usr/local/lib
ADD ./asn1c_combined /asn1_codec/asn1c_combined
RUN cd /asn1_codec/asn1c_combined && bash doIt.sh

# add the source and build files
ADD CMakeLists.txt /asn1_codec
ADD ./config /asn1_codec/config
ADD ./include /asn1_codec/include
ADD ./src /asn1_codec/src
ADD ./kafka-test /asn1_codec/kafka-test
ADD ./unit-test-data /asn1_codec/unit-test-data
ADD ./data /asn1_codec/data
ADD ./run_acm.sh /asn1_codec
ADD ./data /asn1_codec/data

RUN echo "export LD_LIBRARY_PATH=/usr/local/lib" >> ~/.profile
RUN echo "export LD_LIBRARY_PATH=/usr/local/lib" >> ~/.bashrc
RUN echo "export CC=gcc" >> ~/.profile
RUN echo "export CC=gcc" >> ~/.bashrc

# Build acm.
RUN mkdir -p /build && cd /build && cmake /asn1_codec && make

# === RUNTIME IMAGE ===
FROM alpine:3.12
USER root
WORKDIR /asn1_codec
VOLUME ["/asn1_codec_share"]

# add runtime dependencies
RUN apk add --upgrade --no-cache \
    bash \
    librdkafka \
    librdkafka-dev

# copy the built files from the builder
COPY --from=builder /asn1_codec /asn1_codec
COPY --from=builder /build /build

# run ACM
RUN chmod 7777 /asn1_codec/run_acm.sh
CMD ["/asn1_codec/run_acm.sh"]
