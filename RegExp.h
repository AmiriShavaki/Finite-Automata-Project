#ifndef REGEXP_H
#define REGEXP_H

#include <map>
#include <string>

const string NOCOEF = "NOCOEF";
const string LANDA = "LANDA";

class RegExp {
public:
    void construct(string q, string letter);
private:
    map < string, string > equation; //q_i -> coefficient of q_i
    const bool isArdenAppliable();
};

const bool RegExp::isArdenAppliable() {

}

void RegExp::construct(string q, string letter) {
    if (letter.empty()) {
        letter = LANDA;
    }
    if (equation.find(q) == equation.end()) {
        equation[q] = letter;
    } else {
        equation[q] += '+'; equation[q] += letter; //union
    }
}

#endif // REGEXP_H
