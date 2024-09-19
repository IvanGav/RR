#pragma once

#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct Environment {
    static vector<string> type_names;

    static void init_std() {
        type_names = {"int", "float", "bool", "str"};
    }

    static Environment new_empty() {
        
    }
};