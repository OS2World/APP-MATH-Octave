all: hello.exe opentest.exe memory.exe

clean:
	rm -f *.o *.obj *.exe *.dll

opentest.obj : opentest.c
	gcc -m486 -O2 -Ii:/project/C -c opentest.c -o opentest.obj -Zomf

opentest.exe : opentest.obj
	gcc -Wall -m486 -s -o opentest.exe opentest.obj -Zcrtdll -Zomf

LDFLAGS  = -lstdcpp  -Zcrtdll -Zstack 0x10000
LDFLAGS2 = -lstdcpp2 -Zcrtdll -Zstack 0x10000

hello.obj : hello.cc
	gcc -Wall -m486 -O2 -Ii:/project/C -s -c hello.cc -o hello.obj -Zomf

hello.exe : hello.obj
	gcc -Wall -m486 -s -o hello.exe hello.obj $(LDFLAGS2) -Zomf

#hello.o : hello.cc
#	gcc -Wall -m486 -O2 -s -Ii:/project/C -c hello.cc -o hello.o

#hello : hello.o
#	gcc -Wall -m486 -O2 -o hello hello.o $(LDFLAGS2)

#hello.exe : hello
#	emxbind -bqh32 j:/apps1/c/emx/bin/emxl hello hello.exe
#	emxbind -sq hello.exe

memory.obj : memory.cc
	gcc -Wall -m486 -O2 -s -Ii:/project/C -c memory.cc -o memory.obj -Zomf

memory.exe : memory.obj
	gcc -Wall -m486 -O2 -o memory.exe memory.obj $(LDFLAGS2) -Zomf

#memory.o : memory.cc
#	gcc -Wall -m486 -O2 -s -Ii:/project/C -c memory.cc -o memory.o

#memory : memory.o
#	gcc -Wall -m486 -O2 -o memory memory.o $(LDFLAGS2)

#memory.exe : memory
#	emxbind -bqh32 i:/apps/c/emx/bin/emxl memory memory.exe
#	emxbind -sq memory.exe
