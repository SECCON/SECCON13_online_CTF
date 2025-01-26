FROM ubuntu:24.04@sha256:f470988096c4d77efac9740a1b6700823681af518a17fad30111430b95dfbffa AS base
WORKDIR /app
COPY --chmod=555 chall run
RUN echo "SECCON{The_cat_seemed_surprised_when_you_showed_this_flag.}" > /flag.txt
RUN mv /flag.txt /flag-$(md5sum /flag.txt | awk '{print $1}').txt

FROM pwn.red/jail
COPY --from=base / /srv
ENV JAIL_TIME=120 JAIL_CPU=100 JAIL_MEM=10M
