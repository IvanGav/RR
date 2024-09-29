#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "datatypes.h"
#include "tokenizer.h"

using namespace std;

struct RRDataType {
    int type;

    RRDataType() {
        type = -1;
    }
    RRDataType(string str) {
        this->type = single_type_of(str);
    }
    RRDataType(Token literal) {
        switch (literal.info) {
            case TokenInfo::L_BOOL: type = single_type_of("Bool"); break;
            case TokenInfo::L_FLOAT: type = single_type_of("Float"); break;
            case TokenInfo::L_INT: type = single_type_of("Int"); break;
            case TokenInfo::L_STR: type = single_type_of("Str"); break;
            default: type = single_type_of("Any"); break;
        }
    }

    bool operator==(RRDataType const& rhs) {
        return this->type == rhs.type;
    }
    //return true if this object is *equivalent* to rhs
    //rhs may include datatype `Any`
    bool equivalent_to(RRDataType const& rhs) {
        return (*this == rhs) || rhs.type == DATATYPE_ANY;
    }
};

//takes ownership of the data
struct RRObj {
    RRDataType type;
    union {
        long long data_int;
        double data_float;
        bool data_bool;
        string* data_str;
        vector<RRObj>* data_vec;
        unordered_set<RRObj>* data_set;
        unordered_map<RRObj, RRObj>* data_map;
        pair<RRObj, RRObj>* data_pair;
    };

    RRObj(RRDataType t) {
        type = t;
        data_int = 0;
    }

    RRObj(Token t) {
        this->type = RRDataType(t);
        switch(t.info) {
            case TokenInfo::L_BOOL: this->data_bool = (t.t == "true" ? 1 : 0); break;
            case TokenInfo::L_STR: this->data_str = (new string(t.t)); break;
            case TokenInfo::L_INT: this->data_int = (stol(t.t)); break;
            case TokenInfo::L_FLOAT: this->data_float = (stod(t.t)); break;
            default: break;
        }
    }
};

struct RRFun {
    vector<RRDataType> params;
    RRObj (*cpp_fun)(vector<RRObj>);
    void* rr_fun;

    RRFun(vector<RRDataType> params, RRObj (*cpp_fun)(vector<RRObj>)) {
        this->params = params;
        this->cpp_fun = cpp_fun;
    }
};