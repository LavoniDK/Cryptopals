make:
	g++ -std=c++17 -Wall -Wextra \
	-I../../Utils/Converter/ \
	-I../../Utils/ByteVector/ \
	-o cbc cbc.cpp \
	../../Utils/Converter/base64.cpp \
	../../Utils/Converter/ascii.cpp \
	../../Utils/Converter/hex.cpp \
	../../Utils/ByteVector/linalg.cpp \
	../../Utils/ByteVector/xor.cpp \
	-lcryptopp
	./cbc
	rm ./cbc
