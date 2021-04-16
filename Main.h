#include <iostream>
#include <string>

using namespace std;

class Main {
public:
    const char menu();
    void showMsg(int cse);
private:
    const int STARLENGTH = 25;
    void showStarBar(const int n);
};

const char Main::menu() {
    cout << endl;
    showStarBar(STARLENGTH);
    cout << "Choose one of the following options:\n";
            cout << "1.isAcceptByNFA\n";
            cout << "2.createEquivalentDFA\n";
            cout << "3.findRegExp\n";
            cout << "4.showSchematicNFA\n";
            cout << "5.isAcceptByDFA\n";
            cout << "6.makeSimpleDFA\n";
            cout << "7.showSchematicDFA\n";
    cout << "or enter Exit to terminate the program.\n";
    showStarBar(STARLENGTH);
    cout << endl;
    string op;
    cin >> op;
    char maxChar = '7';
    while (op.size() != 1 || op[0] < '1' || op[0] > maxChar) {
        if (op == "Exit") {
            return '0';
        }
        cout << "please just enter a positive integer in [1, " << maxChar << "] interval:\n";
        cin >> op;
    }
    return op[0];
}

void Main::showStarBar(const int n) {
    for (int i = 0; i < n; i++) {
        cout << '*';
    } cout << endl;
}
