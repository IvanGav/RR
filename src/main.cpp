#include <iostream>
#include <string>

// #include "tokenizer.h"
// #include "parser.h"
// #include "interpreter.h"
// #include "environment.h"

using namespace std;

// int main() {
//     Tokenizer t = Tokenizer::new_empty();
//     string input;
//     while(getline(cin, input)) {
//         t.add_str(input);
//     }
//     Interpreter i = Interpreter::new_from_parser(Parser::new_from_tokenizer(t));
//     i.run();
// }

#include "v2/parser.h"

int main() {
    cout << "str: " << sizeof(string) << endl;
    cout << "long long: " << sizeof(long long) << endl;
    cout << "double: " << sizeof(double) << endl;
    cout << "vec: " << sizeof(vector<long long>) << endl;
    cout << "vec<str>: " << sizeof(vector<string>) << endl;
    cout << "ptr: " << sizeof(void*) << endl;

    
    
    
    

    cout << "o: " << sizeof(RRObj) << endl;
    cout << "vec: " << sizeof(vector<RRObj>) << endl;
    cout << "set: " << sizeof(unordered_set<RRObj>) << endl;
    cout << "map: " << sizeof(unordered_map<RRObj, RRObj>) << endl;
    cout << "pair: " << sizeof(pair<RRObj, RRObj>) << endl;
}