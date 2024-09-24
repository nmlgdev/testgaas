multiAlgo : multialgo.o \
        gcc -O -o multiAlgo multiAlgo.o

multiAlgo.o : multiAlgo.c
        gcc -c multiAlgo.c

clean :
        rm multiAlgo multialgo.o 
