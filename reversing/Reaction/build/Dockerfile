FROM ubuntu:24.04 AS base
WORKDIR /app
ADD --chmod=555 chemic run
ADD --chmod=444 flag.txt flag.txt

FROM pwn.red/jail
COPY --from=base / /srv
ENV JAIL_TIME=180 JAIL_CPU=100 JAIL_MEM=10M
