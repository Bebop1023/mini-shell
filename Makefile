CXX      = clang++
CXXFLAGS = -std=c++20 -Wall -Wextra
SRCS     = main.cpp parser.cpp
HDRS     = parser.h

mysh: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -O2 $(SRCS) -o mysh

debug: mysh-debug

mysh-debug: $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -g -fsanitize=address $(SRCS) -o mysh-debug

clean:
	rm -rf mysh mysh-debug fuzz_parser *.dSYM

.PHONY: debug clean fuzz

FUZZCXX ?= /opt/homebrew/opt/llvm/bin/clang++

fuzz_parser: fuzz_parser.cpp parser.cpp $(HDRS)
	$(FUZZCXX) $(CXXFLAGS) -g -fsanitize=fuzzer,address fuzz_parser.cpp parser.cpp -o fuzz_parser

fuzz: fuzz_parser
	mkdir -p fuzz/corpus
	./fuzz_parser fuzz/corpus fuzz/seeds -max_total_time=60

