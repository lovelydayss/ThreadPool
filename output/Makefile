VPATH=src:../:../src:../src/Utils/ThreadPool:../test

objects = tutorial.o UThreadPool.o

main: $(objects)
	g++ $(objects) -o main.o

tutorial.o : ThreadPool.h MyFunction.h
UThreadPool.o : UThreadPool.h

.PHONY : clean
clean :
	rm -rf *.o