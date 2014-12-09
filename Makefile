all: createvd dumpseclog formatvd shell

clean:
	rm shell

createvd: createvd.c filesystem.h
	gcc -o createvd createvd.c filesystem.h

dumpsec: dumpsec.c vdisk.h vdisk.c
	gcc -o dumpsec dumpsec.c vdisk.h vdisk.c

dumpseclog: dumpseclog.c vdisk.h vdisk.c
	gcc -o dumpseclog dumpseclog.c vdisk.h vdisk.c filesystem.h filesystem.c -std=gnu99

formatvd: formatvd.c vdisk.h vdisk.c
	gcc -o formatvd formatvd.c vdisk.h vdisk.c filesystem.h -std=gnu99

gui: vdisk.h vdisk.c filesystem.h filesystem.c Archivos.c Archivos.h gui_utils.h gui_utils.c gui.h gui.c
	gcc -o gui vdisk.h vdisk.c filesystem.h filesystem.c Archivos.c Archivos.h gui_utils.h gui_utils.c gui.h gui.c `pkg-config --libs --cflags gtk+-2.0` -std=gnu99 -Wimplicit-function-declaration


shell: 	filesystem.h filesystem.c vdisk.h vdisk.c Archivos.c Archivos.h
	gcc -o shell Shell.c filesystem.h filesystem.c vdisk.h vdisk.c Archivos.c Archivos.h -std=gnu99

tshell: filesystem.h filesystem.c vdisk.h vdisk.c Archivos.c Archivos.h
	gcc -o debugshell Shell.c filesystem.h filesystem.c vdisk.h vdisk.c Archivos.c Archivos.h -std=gnu99 -D ACTIVE_DEBUG

erase:
	rm disco*.vd
disc:
	./createvd 0
	./formatvd 0
