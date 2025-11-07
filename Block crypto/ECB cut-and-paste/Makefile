CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra
INCLUDES := -I../../Utils/Converter/ -I../../Utils/ByteVector/
LIBS := -lssl -lcrypto
OWN_LIBS := ../../Utils/Converter/ascii.cpp ../../Utils/ByteVector/linalg.cpp
SRC := ecb_cut.cpp
OUT := test

# Usage:
#   make run ARGS="foo bar baz"
#   make run ARGS="AAAAAAAAAAAAAAAA"

run:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUT) $(SRC) $(OWN_LIBS) $(LIBS)
	./$(OUT) "$(ARGS)"
	rm ./$(OUT)
