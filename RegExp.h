#ifndef REGEXP_H
#define REGEXP_H

#include <string>

const string EMPTY = "@";
const string LAMBDA = "%";

class RegExp {
public:
    inline static string Star(string x);
    inline static string Concat(string x, string y);
    inline static string Union(string x, string y);
    void setVal(string val) { this -> val = val; }
    string getVal() { return val; }
    RegExp() { val = EMPTY; }
private:
    string val;
};

inline string RegExp::Union(string x, string y) {
    if (x == EMPTY) {
        return y;
    }
    if (y == EMPTY) {
        return x;
    }
    if (x == y) {
        return x;
    }
    string ans = "("; ans += x; ans += '+'; ans += y; ans += ')'; return ans;
    //x += '+'; x += y; return x; old buggy method
}

inline string RegExp::Concat(string x, string y) {
    if (x == EMPTY || y == EMPTY) {
        return EMPTY;
    }
    if (x == LAMBDA) {
        return y;
    }
    if (y == LAMBDA) {
        return x;
    }
    x += y; return x;
}

inline string RegExp::Star(string x) {
    if (x == EMPTY || x == LAMBDA) {
        return x;
    }
    string ans = "("; ans += x; ans += ")*"; return ans;
}

#endif // REGEXP_H
