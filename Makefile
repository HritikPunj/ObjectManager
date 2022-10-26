main: main1.c ObjectManager.o ObjectManager.h Object.h
	clang++ -Wall -g main1.c -o main ObjectManager.o
ObjectManager.o: ObjectManager.c ObjectManager.h
	clang++ -Wall -g -c ObjectManager.c -o ObjectManager.o
clean:
	rm -fr ObjectManager.o main