#########
#
# Makefile pour fonctionsTCP
#
#########

# For Solaris
#LD_FLAGS = -lsocket -lnsl

# For Linux
LD_FLAGS =

all: fonctionsTCP

fonctionsTCP: fonctionsTCP.c
	gcc -Wall -c fonctionsTCP.c -o fonctionsTCP.o $(LD_FLAGS)

clean:
	rm *~ ; rm -i \#* ; rm *.o; \
        rm fonctionsTCP
