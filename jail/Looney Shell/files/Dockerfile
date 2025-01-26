FROM ubuntu:24.04@sha256:3f85b7caad41a95462cf5b787d8a04604c8262cdcdf9a472b8c52ef83375fe15 AS base

ENV DEBIAN_FRONTEND=noninteractive

RUN apt -y update --fix-missing
RUN apt -y install libc6-i386=2.39-0ubuntu8.3

WORKDIR /app
RUN mkdir bin work
ADD --chmod=755 bin/hello bin/hello
ADD --chmod=511 bin/flag flag
ADD --chmod=755 build/run.sh run
ADD --chmod=644 build/bashrc work/.bashrc

FROM pwn.red/jail
COPY --from=base / /srv
