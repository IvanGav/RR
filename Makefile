a.out: src/main.cpp src/tokenizer.h src/parser.h src/interpreter.h
	g++ src/main.cpp

clear:
	rm a.out