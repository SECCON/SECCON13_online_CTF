#!/bin/sh

cd $(dirname $0)
exec timeout -sKILL 180 ./qemu-system-x86_64 \
	-L ./roms \
	-m 64M \
	-kernel bzImage \
	-append "console=ttyS0 oops=panic panic=1 loglevel=3 pti=on kaslr" \
	-cpu kvm64,smap,smep \
	-initrd rootfs.cpio.gz \
	-device baby \
	-monitor /dev/null \
	-nographic \
	-no-reboot \
	-net nic,model=virtio \
	-net user
