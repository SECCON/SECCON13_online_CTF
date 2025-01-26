FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt -y update --fix-missing
RUN apt -y install python3-pwntools
RUN apt -y install hashcash

RUN mkdir /opt/sc_expwn
ADD https://raw.githubusercontent.com/shift-crops/sc_expwn/master/sc_expwn.py /opt/sc_expwn/

WORKDIR /app
ADD --chmod=644 exploit_qemu /app
ADD --chmod=755 send_exploit.py /app

ENV PYTHONPATH="${PYTHONPATH}:/opt/sc_expwn"
ENV TERM="linux"

CMD ["python3", "send_exploit.py"]
