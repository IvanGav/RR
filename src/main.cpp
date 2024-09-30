#include <iostream>
#include <string>

#include "tokenizer.h"
#include "parser.h"
#include "environment.h"

using namespace std;

int main() {
    init_op_order();
    init_datatypes();

    string source;
    string line;
    while(getline(cin, line)) {
        source += line;
        source += "\n";
    }
    // cout << "--start source code:\n" << source << "\n--end source code." << endl;

    vector<Token> ts = Tokenizer::from_source(source).tokenize();

    // cout << "--start listing tokens:\n" << endl;
    // for(int i = 0; i < ts.size(); i++) {
    //     cout << "token: " << ts[i].type << ", " << ts[i].info << ": '" << ts[i].t << "'" << endl;
    // }
    // cout << "\n--end listing tokens." << endl;

    ASTNode* compiled = Parser::from_tokens(ts).parse();
    Env env = Env();

    // cout << "--start print AST:\n" << endl;
    // cout << *compiled << endl;
    // cout << "\n--end print AST." << endl;

    // cout << "--start eval:\n" << endl;
    RRObj return_val = compiled->eval(env);
    cout << return_val << endl;
    // cout << "\n--end eval." << endl;

    return 0;
}