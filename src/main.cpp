#include <iostream>
#include <string>

#include "tokenizer.h"
#include "parser.h"
#include "environment.h"

using namespace std;

bool DEBUG_MAIN = false;

int main(int argc, char** argv) {
    if(argc > 1) DEBUG_MAIN = true;
    init_datatypes();

    string source;
    string line;
    while(getline(cin, line)) {
        source += line;
        source += "\n";
    }
    if(DEBUG_MAIN) cout << "--start source code:\n" << source << "\n--end source code." << endl;

    vector<Token> ts = Tokenizer::from_source(source).tokenize();
    
    if(DEBUG_MAIN) {
        cout << "--start listing tokens:\n" << endl;
        for(int i = 0; i < ts.size(); i++) {
            cout << "token: " << ts[i].type << ", " << ts[i].info << ": '" << ts[i].t << "'" << endl;
        }
        cout << "\n--end listing tokens." << endl;
    }

    Env env = Env();
    Env::init_with_default(env);
    ASTNode* compiled = Parser::from_tokens(ts).parse(env);

    if(DEBUG_MAIN) {
        cout << "--start print AST:\n" << endl;
        cout << *compiled << endl;
        cout << "\n--end print AST." << endl;
    }

    if(DEBUG_MAIN) cout << "--start eval:\n" << endl;
    RRObj return_val = compiled->eval(env);
    cout << return_val << endl;
    if(DEBUG_MAIN) cout << "\n--end eval." << endl;

    return 0;
}