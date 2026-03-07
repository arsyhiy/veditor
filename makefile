all:
	gcc main.c editor.c buffer.c -lncursesw -o veditor

debug:
	gcc -g main.c editor.c buffer.c -lncursesw -o veditor
