#pragma once

#include <map>
#include <string>

using namespace std;

struct DynamicBool : map<const char*, bool> {
    DynamicBool() { }
    
    bool operator[](const char* key) {
        auto it = find(key);
        if (it == end()) {
            insert({key, false});
            return false;
        }
        return it->second;
    }
    
    // bool operator[](const string &key) { return map<string, bool>::operator[](key); }
};

struct DynamicString : map<const char*, std::string> {
    DynamicString() { }
    
    std::string operator[](const char* key) {
        auto it = find(key);
        if (it == end()) {
            insert({key, ""});
            return "";
        }
        return it->second;
    }
};

static DynamicBool dynamic_bool;
static DynamicString dynamic_string;

// #define dynadd(key, value) dynamic_bool[key] = value;
