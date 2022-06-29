all: cmd_format.c sha256_utils.c sha256.c timestamp.c couche1.c couche2.c couche3.c couche4.c couche5.c main.c installeur.c
	gcc -w cmd_format.c -o cmd_format
	gcc -w sha256_utils.c sha256.c timestamp.c couche1.c couche2.c couche3.c couche4.c couche5.c main.c installeur.c -o virtual_os.exe