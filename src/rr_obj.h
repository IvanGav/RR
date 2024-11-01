#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <cstring>

#include "datatypes.h"
#include "tokenizer.h"
#include "rr_error.h"

using namespace std;

struct RRFun;
struct RRObj;

//takes ownership of the data whenever `owner = true`
struct RRObj {
    RRDataType type;
    union {
        long long data_int;
        double data_float;
        bool data_bool;
        string* data_str;
        RRFun* data_fn;
        vector<RRObj>* data_list;
        unordered_set<RRObj>* data_set;
        unordered_map<RRObj, RRObj>* data_map;
        pair<RRObj, RRObj>* data_pair;
    };
    bool owner;

    RRObj() {
        type = RRDataType();
        data_int = 0;
        owner = true;
    }
    //deep clone constructor
    RRObj(const RRObj& from) {
        type = from.type;
        if(type == RRDataType("Str")) {
            data_str = new string(*from.data_str);
        } else if(type == RRDataType("List")) {
            data_list = new vector<RRObj>(*from.data_list);
        } else {
            memcpy(this, &from, sizeof(RRObj));
        }
        owner = true;
    }
    RRObj(RRDataType t) {
        type = t;
        data_int = 0;
        owner = true;
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
        owner = true;
    }
    RRObj(vector<RRObj>* list) {
        type = RRDataType("List");
        data_list = list;
        owner = true;
    }
    RRObj(string& literal_str) {
        type = RRDataType("Str");
        data_str = new string(literal_str);
        owner = true;
    }
    RRObj(RRFun* rr_fn) {
        type = RRDataType("Fn");
        data_fn = rr_fn;
        owner = true; //can't take ownership of functions, so doesn't matter
    }

    ~RRObj() {
        if(owner) {
            if(type == RRDataType("Str")) {
                delete data_str;
            } else if(type == RRDataType("List")) {
                delete data_list;
            }
        }
    }

    // RRObj& operator=(const RRObj& other) {
    //     // Check for self-assignment
    //     if (this == &other) {
    //         return *this;
    //     }
    //     memcpy(this, &other, sizeof(RRObj));
    //     this->owner = false;
    //     return *this;
    // }

    //make a reference to `this` - reference will not release the resources
    RRObj ref() {
        RRObj reference = *this;
        reference.owner = false;
        return reference;
    }
    //move ownership of data from `this` to returned obj; make `this` a reference instead
    RRObj move() {
        if(!owner) rr_runtime_error("Trying to move a reference to an object");
        RRObj new_owner = *this;
        this->owner = false;
        new_owner.owner = true; //guaranteed
        return new_owner;
    }
    //move if owned; take reference when not owned
    RRObj transfer() {
        RRObj new_owner = *this;
        new_owner.owner = this->owner;
        owner = false;
        return new_owner;
    }
    //if owner, do nothing; if not owner, deep clone in place
    void to_owned() {
        if(!owner) {
            //not owned, do deep clone
            if(type == RRDataType("Str")) {
                data_str = new string(*this->data_str);
            } else if(type == RRDataType("List")) {
                data_list = new vector<RRObj>(*this->data_list);
            }
            owner = true;
        }
    }
    
    friend std::ostream& operator<<(std::ostream& os, const RRObj& obj) {
        switch(obj.type.type) {
            case 0: return os << "Bool: " << obj.data_bool;
            case 1: return os << "Int: " << obj.data_int;
            case 2: return os << "Float: " << obj.data_float;
            case 3: return os << "Str: " << *(obj.data_str);
            case 4: return os << "Pair: " << "(" << (obj.data_pair->first) << "," << (obj.data_pair->second) << ")";
            case 5: {
                if(obj.data_set->size() == 0) return os << "Set: {}";
                os << "Set: {";
                auto i = obj.data_set->begin();
                while(true) {
                    os << *i;
                    i++;
                    if(i == obj.data_set->end()) break;
                    os << ",";
                }
                return os << "}";
            };
            case 6: {
                if(obj.data_list->size() == 0) return os << "Vec: []";
                os << "Vec: [";
                auto i = obj.data_list->begin();
                while(true) {
                    os << *i;
                    i++;
                    if(i == obj.data_list->end()) break;
                    os << ",";
                }
                return os << "]";
            };
            case 8: {
                if(obj.data_list->size() == 0) return os << "List: []";
                os << "List: [";
                auto i = obj.data_list->begin();
                while(true) {
                    os << *i;
                    i++;
                    if(i == obj.data_list->end()) break;
                    os << ",";
                }
                return os << "]";
            };
            case 10: {
                return os << "None";
            };
            default: return os << "Unhandled type";
        }
    }
};

struct RRFun {
    vector<RRDataType> params;
    RRDataType return_type;
    RRObj (*cpp_fun)(vector<RRObj>&, Env&);
    void* rr_fun;

    RRFun() {}
    RRFun(vector<RRDataType> params, RRDataType return_type, RRObj (*cpp_fun)(vector<RRObj>&, Env&)) {
        this->params = params;
        this->return_type = return_type;
        this->cpp_fun = cpp_fun;
    }
};