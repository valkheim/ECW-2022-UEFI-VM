#https://github.com/Pebaz/fasmg-ebc-rs

all:
	python build.py
	fasmg-ebc-rs main.asm stage2.efi
	xxd -i stage2.efi > ../stage1/stage2.in
