FROM ghcr.io/tcarland/tcanetpp:latest

ENV TCAMAKE_PROJECT=/opt
ENV TCAMAKE_HOME=/opt/tcamake
ENV TCAMAKE_PREFIX=/usr

USER root
WORKDIR /opt

RUN mkdir -p /opt/tcajson
COPY . /opt/tcajson

RUN cd tcajson && make arlib && make install

USER tdh