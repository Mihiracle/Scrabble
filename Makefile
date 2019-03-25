BINDIR := bin

CXX := g++
CXXFLAGS := -g -Wall --std=c++11

VALGRIND := valgrind --tool=memcheck --leak-check=yes

GTEST_CXXFLAGS := -I /usr/include/gtest/ 
GTEST_LDFLAGS := -l gtest -l gtest_main -pthread

all: scrabble

# Create binary dir
$(BINDIR)/.dirstamp:
	mkdir -p $(BINDIR)
	touch $(BINDIR)/.dirstamp

# generic rule for .cpp files
$(BINDIR)/%.o : %.cpp $(BINDIR)/.dirstamp
	$(CXX) -c $(CXXFLAGS) $< -o $@

scrabble: $(BINDIR)/Bag.o $(BINDIR)/Board.o $(BINDIR)/ConsolePrinter.o $(BINDIR)/Dictionary.o $(BINDIR)/main.o $(BINDIR)/Move.o $(BINDIR)/Player.o $(BINDIR)/Trie.o $(BINDIR)/ScoreAI.o $(BINDIR)/LengthAI.o
	$(CXX) $(CXXFLAGS) $^ -o $@

# clean
clean:
	rm -rf $(BINDIR) 

