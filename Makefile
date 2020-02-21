UNAME := $(shell uname -s)

# For non-optimizing, comment both
C_OPT	= -O3

# Uncomment for fast & dirty
#NDEBUG=-DNDEBUG

# Uncomment for Fix architecture
#ARCH=-march=pentium-m 

# END OF USER-FIX

CC	= gcc $(ARCH) $(NDEBUG)
C++     = g++ $(ARCH) $(NDEBUG) 
CC	= $(C++)

#GDB=-ggdb3

CFLAGS	= -Wall $(GDB) $(C_OPT) $(EFENCE) 
# -fstack-protector is added by default in my
# gcc 4.1 under Ubuntu 7.10, but then it fails in other OS as Rock Cluster

all: prime-impl

prime-impl: prime-impl-test.cc prime-impl.o clauses.o
	$(C++) $(CFLAGS) -o prime-impl prime-impl-test.cc prime-impl.o clauses.o

.SUFFIXES: .c .cc .o
.c.o:
	$(CC) $(CFLAGS) -c $<
.cc.o:
	$(CC) $(CFLAGS) -c $<

clean:
			rm -rf prime-impl *.o *.dSYM

prime-impl-test.o: prime-impl.hpp clauses.hpp
prime-impl.o: prime-impl.hpp clauses.hpp
clauses.o: clauses.hpp
