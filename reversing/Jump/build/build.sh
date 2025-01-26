#!/bin/zsh -e
pretzel_logic -mrop-obfuscate --target=aarch64-linux-gnu -fno-ident jump.c -o jump
docker image build -t binutils . --platform=linux/aarch64
docker run -it --rm -v .:/ctf --platform=linux/aarch64 binutils strip /ctf/jump
cp jump ../files/
