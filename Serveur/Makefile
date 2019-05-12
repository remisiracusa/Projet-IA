#########
#
# Makefile pour serveur Yokai
#
#########

# For Solaris
#LD_FLAGS = -lsocket -lnsl

# For Linux
LD_FLAGS =

all: serveur

serveur: src/serveur.c
	gcc -Wall lib/v3/yokai-fPIC.o src/fonctionsTCP.c src/serveur.c -o serveur   $(LD_FLAGS)


clean:
	rm *~ ; rm -i \#* ; rm *.o; \
        rm serveur
