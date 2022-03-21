CC = gcc

all : mkdirs.exe qecho.exe

mkdirs.exe : mkdirs.c
	$(CC) mkdirs.c -o mkdirs.exe

qecho.exe : qecho.c
	$(CC) qecho.c  -o qecho.exe

clean :
	if exist mkdirs.exe del mkdirs.exe
	if exist qecho.exe  del qecho.exe
