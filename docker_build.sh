#!/bin/bash

# https://hub.docker.com/r/espressif/idf
TAG="release-v4.4"

TARGET=esp32s2

CMD="idf.py set-target $TARGET && idf.py build"

docker run --rm -v $PWD:/project -w /project espressif/idf:$TAG /bin/bash -c "$CMD"
