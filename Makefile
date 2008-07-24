CFLAGS=-Wall -ansi -O3 -m32

all: dlx pentos bedlam_cube

dlx: main.o dlx.o
	$(CC) $(CFLAGS) -o dlx main.o dlx.o

pentos: pentos.o dlx.o
	$(CC) $(CFLAGS) -o pentos pentos.o dlx.o

bedlam_cube: bedlam_cube.o dlx.o
	$(CC) $(CFLAGS) -o bedlam_cube bedlam_cube.o dlx.o

clean:
	-rm *.o
	-rm dlx
	-rm pentos
	-rm bedlam_cube
