CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra
INCLUDES := -I../../Utils/Converter/
LIBS := -lcrypto -lssl
OWN_LIBS := ../../Utils/Converter/ascii.cpp ../../Utils/Converter/base64.cpp
SRC := ecb_byte.cpp
OUT := test

# Usage:
#   make run ARGS="foo bar baz"
#   make run ARGS="AAAAAAAAAAAAAAAA"

run:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUT) $(SRC) $(OWN_LIBS) $(LIBS)
	./$(OUT) $(ARGS)
	rm ./$(OUT)
