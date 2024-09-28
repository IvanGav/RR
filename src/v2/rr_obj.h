#pragma once

#include <string>
#include <vector>

#include "datatypes.h"

using namespace std;

struct RRDataType {
    int type;

    static RRDataType from_str(string str) {
        return RRDataType { {single_type_of(str)} };
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
    void* data;
};

struct RRFun {
    vector<RRDataType> params;
    void* cpp_fun;
    void* rr_fun;
};
