#
# Makefile pour generer le serveur 
#

OBJS = serveur.o http.o capteur.o
LIBS = -lpthread
LOCAL_LIBS  = -L ../Communication  -L ../Threads
LOCAL_LIBS += -lcom -lthrd
CFLAGS     += -I ../Communication  -I ../Threads

#
# Cible generale
#

all: serveur

#
# La cible de nettoyage
#

clean: 
	rm -f core *.o serveur

#
# Les cibles pour l'executable
#

serveur: $(OBJS)
	$(CC) $(CFLAGS) -o serveur $(OBJS) $(LOCAL_LIBS) $(LIBS)

http.o : http.c serveur.h
capteur.o : capteur.c serveur.h
serveur.o: serveur.c serveur.h 
