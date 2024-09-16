a.out: src/main.cpp src/tokenizer.cpp
	g++ src/main.cpp

clear:
	rm a.out