 .PHONY: clean test
CFLAGS = -O3 -Wall -std=c++11
LDFLAGS+=-lm     # link to math library

all: train test acc

train: train.o
	g++ -o $@ train.o $(CFLAGS)

test: test.o
	g++ -o $@ test.o $(CFLAGS)

acc: acc.o
	g++ -o $@ acc.o $(CFLAGS)

train.o: train.cpp hmm.h train.h
	echo  "*******Caution! I modified the hmm.h. Please use my file.*******"
	g++ -c train.cpp $(CFLAGS)

test.o: test.cpp hmm.h
	g++ -c test.cpp $(CFLAGS)

acc.o: acc.cpp
	g++ -c acc.cpp $(CFLAGS)

clean:
	rm -f *.o *.exe train test acc all
