FROM ghcr.io/tcarland/tcanetpp:v1.6.0

ENV TCAMAKE_PROJECT=/opt
ENV TCAMAKE_HOME=/opt/tcamake
ENV TCAMAKE_PREFIX=/usr

USER root
WORKDIR /opt

COPY . /opt/tcajson

RUN chown -R tdh /opt/tcajson && \
    cd tcajson && \
    make arlib && \
    make install

USER tdh
