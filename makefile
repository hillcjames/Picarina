CC=gcc

HEADERS = picarina.h tones.h

default: picarina.c tones.c $(HEADERS)
	gcc picarina.c tones.c $(HEADERS) -o pic -lasound -lwiringPi


clean:
	-rm -f pic logfile.txt

run:
	./pic
