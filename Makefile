main: main.c ObjectManager.o ObjectManager.h Object.h
	clang -Wall -g main.c -o main ObjectManager.o
ObjectManager.o: ObjectManager.c ObjectManager.h
	clang -Wall -g -c ObjectManager.c -o ObjectManager.o
clean:
	rm -fr ObjectManager.o main