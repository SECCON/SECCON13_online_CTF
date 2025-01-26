FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt -y update --fix-missing
RUN apt -y install netcat-traditional

WORKDIR /root
ADD --chmod=755 solver.sh .

ENTRYPOINT /root/solver.sh
