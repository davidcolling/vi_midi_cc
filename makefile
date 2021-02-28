CC		= "gcc"
LIBS	= "-lncurses"

term:
	eval `tset -s`	#set TERM environment variable; see "initialization" in ncurses(3X)
	tput init	#initialize terminal; see "initialization" in ncurses(3X)

main: term main.c list.c print.c sendmidi.c
	$(CC) $(LIBS) $@.c -o ./builds/$@

run: main
	./builds/main 19

db: main
	$(CC) -g $(LIBS) main.c -o main
	gdb ./builds/main

clean:
	rm -rf ./build/main
