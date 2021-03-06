CC=gcc

HEADERS = picarina.h tones.h

default: picarina.c tones.c $(HEADERS)
	gcc picarina.c tones.c $(HEADERS) -o pic -lasound -lwiringPi

test: picarina.c tones.c $(HEADERS)
	gcc picarina.c tones.c $(HEADERS) -o pic -lasound

clean:
	-rm -f pic logfile.txt

run:
	./pic
