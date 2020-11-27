FROM ubuntu:16.04

WORKDIR /usr/src/app

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y make gcc valgrind
