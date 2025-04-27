all: hex_editor
hex_editor: hex_editor.o
	gcc -m32 -g -Wall -o hex_editor hex_editor.o 
hex_editor.o: hex_editor.c
	gcc -g -Wall -m32 -c hex_editor.c -o hex_editor.o
.PHONY: clean
clean:
	rm -f *.o task1