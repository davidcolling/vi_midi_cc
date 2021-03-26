CC		= "gcc"
LIBS	= "-lncurses" "-lm" "-pthread"

term:
	eval `tset -s`	#set TERM environment variable; see "initialization" in ncurses(3X)
	tput init	#initialize terminal; see "initialization" in ncurses(3X)

main: term main.c list.c print.c sendmidi.c
	$(CC) $(LIBS) $@.c -o ./builds/$@

db: main
	$(CC) -g $(LIBS) main.c -o ./builds/main
	gdb ./builds/main

clean:
	rm -rf ./build/main
