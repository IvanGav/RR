#include <iostream>
#include <string>
#include "tokenizer.cpp"

using namespace std;

int main() {
    string input;
    // getline(cin, input);
    input = "1. This is a test text.\n2. Delimiting () and {}\n3. Delimiting (this)and{this}";
    tokenizer t = tokenizer::new_empty();
    t.add_str(input);
    string out = t.next();
    do {
        cout << "'" << out << "'" << endl;
        out = t.next();
    } while(out != NO_TOKENS);
}