#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

#include "tokenizer2.h"
#include "parser2.h"

using namespace std;

/*
    Definitions
*/

unordered_map<string, int> type_names;

/*
    Functions
*/

//should be called at the beginning to initialize before creating any environment
void env_global_init() {

}

/*
    Structs
*/

struct EnvVar {
    
};

struct Env {
    unordered_map<string, EnvVar> vars;
};

/*
let's say you have a program
it has: 
- main execution line
- functions
- function calls
- loops
- ifs
- code blocks
- some data that's remaining from different/previous code executions

so

data from function calls *must not* be saved and transfered into main line
and functions shouldn't be able to use the main environment

so,
- every function has its own environment (unless i want to be cringe)
- environment can be saved between runs, if that's what i need

- every scope should have access to global environment
- no local variables should escape its scope (except for the return value)

what i need now is:
- how to store variables
- how to store rvalues
- what parameters do functions take and how do they take them
- how do rvalues get passed into a function
- how to store type info
- how to store function pointers
- what happens when a variable gets reassigned

how are lists stored?
- must be of the same type
- vectors
- but how to indicate the type

how to indicate type of a collection?
- possibly many template parameters (int; string, int)
*/