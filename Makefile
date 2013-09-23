
HEADERS = $(shell find ./include/ -name "*.hpp")

bin/main: main.cpp $(HEADERS)
	clang++ -std=c++11 -I./include -lboost_system-mt -o ./bin/main main.cpp

clean:
	rm main

.PHONY: clean
