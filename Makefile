CC = gcc
CFLAGS = -W -Wall -pedantic -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wbad-function-cast -Wmissing-prototypes -Wstrict-prototypes -Wmissing-declarations -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wconversion -Wwrite-strings -Wno-conversion -ffloat-store
C11=-std=c11
DEBUG = -g
OPTZ  = -O2
OBJS = *.o
BACK = *~

CPU_IPP = cpu_ipp

RM = /bin/rm

$(CPU_IPP): $(CPU_IPP).c
	$(CC) $(CFLAGS) $(DEBUG) $(C11) -o $(CPU_IPP) $^

clean:
	$(RM) -f $(CPU_IPP) $(BACK) $(OBJS)
