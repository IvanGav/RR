#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "parser.h"

using namespace std;

/*
    Definitions
*/

bool vec_eq(vector<string>& v1, vector<string>& v2);

/*
    Static Data
*/

vector<string> type_names = {"Any", "Int", "Float", "Bool", "Str"};

/*
    Structs
*/

struct EnvVar {
    string name;
    int type_num;
    void* data; //int, float and bool are literals, otherwise pointer to an object; type specified by type_num

    string type_name() { return type_names[type_num]; }
};

struct EnvFun {
    string name;
    vector<vector<string>> params;
    vector<void*> cpp_fun; //if nullptr, use rr_fun instead
    vector<TokenNode*> rr_fun; //if nullptr, use cpp_fun instead
};

// Environment takes ownership of: RR function pointers, EnvVar data pointers
struct Environment {
    vector<EnvVar> vars;
    vector<EnvFun> funs;

    static Environment new_empty() {
        return Environment { vector<EnvVar>(0) };
    }

    void add_var(EnvVar var) {
        vars.push_back(var);
    }

    // leave cpp_fun or rr_fun nullptr
    void add_fun(string name, vector<string> params, void* cpp_fun, TokenNode* rr_fun) {
        //check every existing function for the same name as a new function
        for(int i = 0; i < funs.size(); i++) {
            if(funs[i].name == name) {
                //check every existing function with that name for the same parameters as a new function
                for(int j = 0; j < funs[i].params.size(); j++) {
                    if(vec_eq(funs[i].params[j], params)) {
                        //delete the old rr_fun
                        delete_tree(funs[i].rr_fun[j]);
                        funs[i].params[j] = params;
                        funs[i].cpp_fun[j] = cpp_fun;
                        funs[i].rr_fun[j] = rr_fun;
                        return;
                    }
                }
                funs[i].params.push_back(params);
                funs[i].cpp_fun.push_back(cpp_fun);
                funs[i].rr_fun.push_back(rr_fun);
                return;
            }
        }
        funs.push_back( EnvFun { name, { params }, { cpp_fun }, { rr_fun } } );
    }
};

/*
    Helper Functions
*/

bool vec_eq(vector<string>& v1, vector<string>& v2) {
    if(v1.size() != v2.size()) return false;
    for(int i = 0; i < v1.size(); i++) {
        if(v1[i] != v2[i]) return false;
    }
    return true;
}