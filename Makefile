CC = g++
CFLAGS = -I/usr/local/include -I/usr/local/Cellar -L/usr/local/lib -lSDL2 -lSDL2_image

all:
	$(CC) $(CFLAGS) $(file).cpp -o $(file).out

clean:
	rm -rf *.out