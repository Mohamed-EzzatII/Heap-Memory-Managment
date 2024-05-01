STATIC:
	gcc hmm.c std_types.h -I./hmm.h
	ar -r hmm.a *.o
	gcc main.c -I./hmm.h hmm.a --static

DYNAMIC:
	gcc -g -fPIC -Wall std_types.h hmm.h hmm.c -shared -o hmm.so
	LD_PRELOAD=$PWD/hmm.so ./a.out
	
