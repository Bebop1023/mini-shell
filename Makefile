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
	rm -rf mysh mysh-debug *.dSYM

.PHONY: debug clean
