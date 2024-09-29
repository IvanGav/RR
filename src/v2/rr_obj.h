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

    static RRDataType from_str(string str) {
        return RRDataType { {single_type_of(str)} };
    }

    static RRDataType from_token(Token literal) {
        switch (literal.info) {
            case TokenInfo::L_BOOL: return from_str("Bool");
            case TokenInfo::L_FLOAT: return from_str("Float");
            case TokenInfo::L_INT: return from_str("Int");
            case TokenInfo::L_STR: return from_str("Str");
            default: return from_str("Any");
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
        this->type = RRDataType::from_token(t);
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
    void* cpp_fun;
    void* rr_fun;
};