COMPILER=gcc
CCFLAGS=-Wall

all:y86emul.c y86dis.c
		$(COMPILER) $(CCFLAGS) -o y86emul y86emul.c
		$(COMPILER) $(CCFLAGS) -o y86dis y86dis.c

y86emul: y86emul.c
		$(COMPILER) $(CCFLAGS) -o y86emul y86emul.c

y86dis: y86dis.c
		$(COMPILER) $(CCFLAGS) -o y86dis y86dis.c
clean:
		rm -f y86dis
		rm -f *.o
		rm -f y86emul
		rm -f *.o