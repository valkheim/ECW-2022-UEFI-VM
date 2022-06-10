#https://github.com/Pebaz/fasmg-ebc-rs
#https://pypi.org/project/EfiCompressor/#files

all: #comp
	python inject_flag.py
	fasmg-ebc-rs main.asm stage2.efi
	#xxd -i stage2.efi > stage2.in
	#EfiRom -f 0xABCD -i 0x1234 -e stage2.efi
	python compress.py
	xxd -i rom > stage2.in
	cp stage2.in ../stage1/stage2.in

comp:
	gcc lz.c -o lz
	gcc compress.c -o compress
