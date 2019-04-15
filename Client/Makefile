#########
#
# Makefile pour client Yokai
#
#########

# For Solaris
#LD_FLAGS = -lsocket -lnsl

# For Linux
LD_FLAGS =

all: client

client: client.c
	gcc -Wall client.c fonctionsTCP/fonctionsTCP.c -o client   $(LD_FLAGS)


clean:
	rm *~ ; rm -i \#* ; rm *.o; \
        rm client
