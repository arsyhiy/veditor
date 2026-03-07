all:
	gcc main.c editor.c buffer.c display.c file.c input.c -lncursesw -o veditor

debug:

	gcc -g main.c editor.c buffer.c display.c file.c input.c -lncursesw -o veditor
