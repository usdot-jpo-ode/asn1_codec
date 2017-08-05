FROM ubuntu
USER root

WORKDIR /home

VOLUME ["/asn1_codec"]

# Add build tools.
RUN apt-get update && apt-get install -y software-properties-common wget git make gcc-4.9 g++-4.9 gcc-4.9-base && update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.9 100 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.9 100

# Install cmake.
RUN wget https://cmake.org/files/v3.7/cmake-3.7.2.tar.gz && tar -xvf cmake-3.7.2.tar.gz
RUN cd cmake-3.7.2 && ./bootstrap && make && make install && cd /

# Install librdkafka.
RUN git clone https://github.com/edenhill/librdkafka.git && cd librdkafka && ln -s /usr/bin/python3 /usr/bin/python && ./configure && make && make install && cd /asn1_codec

# add the source and build files
ADD ./build_codec.sh /home

# Do the build.
#RUN export LD_LIBRARY_PATH=/usr/local/lib && mkdir asn1_codec_build && cd asn1_codec_build && cmake /asn1_codec && make
#RUN export LD_LIBRARY_PATH=/usr/local/lib && cmake /asn1_codec && make
CMD ["/bin/bash"]
