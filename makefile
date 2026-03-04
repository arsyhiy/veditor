all:
	gcc main.c -lncursesw -o veditor

debug:
	gcc -g main.c -lncursesw -o veditor
