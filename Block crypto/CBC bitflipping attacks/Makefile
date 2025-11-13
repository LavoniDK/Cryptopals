CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra
INCLUDES := -I../../Utils/Converter/
LIBS := -lcrypto -lssl
OWN_LIBS := ../../Utils/Converter/ascii.cpp 
SRC := bitflip.cpp
OUT := test

run:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(OUT) $(SRC) $(OWN_LIBS) $(LIBS)
	./$(OUT) $(ARGS);
	rm ./$(OUT)
