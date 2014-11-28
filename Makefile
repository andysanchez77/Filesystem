all:
	# TODO

clean:
	rm disco*.vd

createvd: createvd.c filesystem.h
	gcc -o createvd createvd.c filesystem.h

dumpsec: dumpsec.c vdisk.h vdisk.c
	gcc -o dumpsec dumpsec.c vdisk.h vdisk.c

dumpseclog: dumpseclog.c vdisk.h vdisk.c
	gcc -o dumpseclog dumpseclog.c vdisk.h vdisk.c -std=gnu99

formatvd: formatvd.c vdisk.h vdisk.c
	gcc -o formatvd formatvd.c vdisk.h vdisk.c filesystem.h -std=gnu99

filesystem:
	# TODO