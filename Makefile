a.out: src/main.cpp src/tokenizer.h src/parser.h src/environment.h src/cpp_fun_impl.h src/datatypes.h src/rr_obj.h src/rr_error.h
	g++ src/main.cpp -g

clear:
	rm a.out