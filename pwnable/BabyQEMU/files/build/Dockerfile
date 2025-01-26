FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get -y update --fix-missing
RUN apt-get -y upgrade
RUN apt-get -y install libslirp0 libpixman-1-0 libpng16-16t64 libjpeg-turbo8 libsdl2-image-2.0-0 libglib2.0-0t64 libasound2t64 libpulse0 libsndio7.0
RUN apt-get -y install xinetd
RUN groupadd -r pwn && useradd -r -g pwn pwn

ADD --chmod=640 build/pwn.xinetd /etc/xinetd.d/pwn
ADD --chmod=700 build/init.sh /etc/init.sh
RUN chmod 1733 /tmp /var/tmp /dev/shm

WORKDIR /home/pwn
ADD release .
RUN chmod a+x qemu-system-x86_64 run.sh

ARG FLAG
RUN echo "${FLAG}" > flag.txt
RUN chmod 440 flag.txt
RUN mv flag.txt flag-$(md5sum flag.txt | awk '{print $1}').txt

RUN chown -R root:pwn /home/pwn
RUN chmod -R go-w /home/pwn
