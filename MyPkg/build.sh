function log() {
  printf "[+] $1\n"
}

# make -C stage2
# cd edk2
# . ./edksetup.sh
# build -n $(nproc)

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")

NAME="My"
PKG="$NAME""Pkg"

log "Creating the image disk"
rm -rf "Out"
mkdir "Out"
IMG="./Out/disk.img"
dd if=/dev/zero of="$IMG" bs=512 count=93750

log "Create GUID partition table and unformatted EFI partition"
printf "o\ny\nn\n\n\n\nef00\nw\ny\n" | gdisk "$IMG"

log "Expose the EFI partition as a linux loopback device"
LO="$(sudo losetup -f)"
sudo losetup --offset 1048576 --sizelimit 46934528 "$LO" "$IMG"

log "Format the partition for FAT32"
sudo mkdosfs -F 32 "$LO"

log "Mount and copy the UEFI application"
#EFIAPP="./$PKG/Build/DEBUG_GCC5/X64/$PKG/Ecw/OUTPUT/Bootloader.efi"
EFIAPP="$BASEDIR/Build/RELEASE_GCC5/X64/$PKG/$NAME/OUTPUT/Bootloader.efi"
MNT=$(mktemp -d --suffix ".uefi")
sudo mkdir -p "$MNT"
sudo mount "$LO" "$MNT"
sudo cp "$EFIAPP" "$MNT"
#sudo cp "$BASEDIR/Build/DEBUG_GCC5/X64/HelloWorld.efi" "$MNT"
sudo mkdir -p "$MNT/efi/boot"
sudo cp "$EFIAPP" "$MNT/efi/boot/bootx64.efi"
#sudo cp "$BASEDIR/stage2/stage2.efi" "$MNT"
#sudo cp "$BASEDIR/stage2/stage2.efi" "$MNT/efi/boot/bootx64.efi"

log "Unmounting the EFI partition, freeing the lo device"
sudo umount "$MNT"
sudo rm -rf "$MNT"
sudo losetup -d "$LO"

log "Starting qemu (CTRL+A X to exit)"
OVMF_CODE="/usr/share/OVMF/x64/OVMF_CODE.fd"
OVMF_CODE="/usr/share/OVMF/OVMF_CODE.fd"
OVMF_VARS="/usr/share/OVMF/x64/OVMF_VARS.fd"
OVMF_VARS="/usr/share/OVMF/OVMF_VARS.fd"
OVMF_VARS="./OVMF_VARS.fd"

log "Starting qemu"

sudo qemu-system-x86_64 -cpu qemu64 \
  -drive if=pflash,format=raw,unit=0,file="$OVMF_CODE",readonly=on \
  -drive if=pflash,format=raw,unit=1,file="$OVMF_VARS" \
  -drive format=raw,file="$IMG",if=virtio \
  -net none \
  -nographic \
  -serial mon:stdio \
  -monitor telnet::45454,server,nowait \

