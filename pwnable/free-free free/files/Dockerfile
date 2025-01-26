FROM ubuntu:24.04@sha256:3f85b7caad41a95462cf5b787d8a04604c8262cdcdf9a472b8c52ef83375fe15 AS base

ENV DEBIAN_FRONTEND=noninteractive

WORKDIR /app
ADD --chmod=755 chall run

ARG FLAG
RUN echo "${FLAG}" > flag.txt
RUN chmod 644 flag.txt
RUN mv flag.txt flag-$(md5sum flag.txt | awk '{print $1}').txt

FROM pwn.red/jail
COPY --from=base / /srv
