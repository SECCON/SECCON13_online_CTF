FROM debian:bookworm AS build

WORKDIR /build

RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    python3 \
    python3-venv \
    ninja-build
    #&& rm -rf /var/lib/apt/lists/*

COPY qrackv.patch .

RUN wget https://download.qemu.org/qemu-9.1.1.tar.xz \
    && tar xf qemu-9.1.1.tar.xz

RUN apt-get install -y meson pkg-config libglib2.0-dev

RUN cd qemu-9.1.1 \
    && mkdir build \
    && patch -p1 < ../qrackv.patch \
    && cd build \
    && ../configure --target-list=riscv64-linux-user --enable-strip --enable-trace-backends=nop \
    && make -j$(nproc) \
    && make install

FROM debian:bookworm AS run

RUN apt-get update && apt-get install -y \
    libglib2.0 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=build /usr/local/bin/qemu-riscv64 /usr/local/bin/qemu-riscv64
COPY ./qrackv ./qrackv

CMD ["qemu-riscv64", "./qrackv"]
