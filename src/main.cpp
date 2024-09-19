#include <iostream>
#include <string>
#include "tokenizer.h"
#include "parser.h"
#include "interpreter.h"
#include "environment.h"

using namespace std;

int main() {
    Tokenizer t = Tokenizer::new_empty();
    string input;
    while(getline(cin, input)) {
        t.add_str(input);
    }
    Interpreter i = Interpreter::new_from_parser(Parser::new_from_tokenizer(t));
    i.run();
}