a.out: src/main.cpp src/tokenizer.h src/parser.h src/interpreter.h src/environment.h src/cpp_fun_impl.h
	g++ src/main.cpp

clear:
	rm a.out