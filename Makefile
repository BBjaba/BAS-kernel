all:
	cd gnu-efi && make
	cd kernel && make kernel && make buildimg && make run
run:
	cd kernel && make run