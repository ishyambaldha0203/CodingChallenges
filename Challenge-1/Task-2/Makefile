# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++14

# Source file
SRC = TestLRUCache.cpp
#SRC = test.cpp

# Executable name
EXEC = TestLRUCache

all: $(EXEC)

$(EXEC): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRC) -lpthread -g

clean:
	rm -f $(EXEC)
