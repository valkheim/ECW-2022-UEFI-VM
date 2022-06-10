qemu-system-x86_64 -cpu qemu64 \
  -drive if=pflash,format=raw,unit=0,file=OVMF_CODE.fd,readonly=on \
  -drive if=pflash,format=raw,unit=1,file=OVMF_VARS.fd \
  -drive format=raw,file=disk.img,if=virtio \
  -net none \
  -nographic \
  -serial mon:stdio \
  -monitor telnet::45454,server,nowait \