all: build

build:
	gcc -pthread -o s-talk -g list.c -g getMes.c -g sender.c -g receiver.c -g printMes.c -g sTalk.c -lpthread 

clean:
	rm s-talk