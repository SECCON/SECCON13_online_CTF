FROM debian:bookworm AS run

RUN apt-get update && apt-get install -y libglib2.0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY ./qemu-riscv64 /usr/local/bin/qemu-riscv64
COPY ./qrackv ./qrackv

CMD ["qemu-riscv64", "./qrackv"]
