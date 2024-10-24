FROM ghcr.io/tcarland/tcanetpp:v1.5.8

ENV TCAMAKE_PROJECT=/opt
ENV TCAMAKE_HOME=/opt/tcamake
ENV TCAMAKE_PREFIX=/usr

USER root
WORKDIR /opt

RUN mkdir -p /opt/tcajson
COPY . /opt/tcajson

RUN chown -R tdh tcajson && \
    cd tcajson && \
    make arlib && \
    make install

USER tdh