#include "Main.h"
#include "NFA.h"
#include "DFA.h"

int main(int argc, char* args[]) {
    Main prg;
    DFA dfa;
    NFA nfa;
    nfa.getInput();
    while (true) {
        char op = prg.menu();
            switch (op) {

                case '0': { //Exit
                    return 0;
                }
                break;

                case '1': { //isAcceptByNFA
                    cout << (nfa.isAcceptByNFA() ? "Given string is accepted by the NFA" : "Given string is rejected by the NFA");
                }
                break;

                case '2': { //createEquivalentDFA
                    nfa.createEquivalentDFA();
                }
                break;

                case '3': { //findRegExp
                    cout << "Generated Regular expression: (State Removal method)\n" << nfa.findRegExp() << endl;
                }
                break;

                case '4': { //showSchematicNFA
                    nfa.showSchematicNFA();
                }
                break;

                case '5': { //isAcceptByDFA
                    cout << (dfa.isAcceptByDFA(nfa) ? "Given string is accepted by the DFA" : "Given string is rejected by the DFA");
                }
                break;

                case '6': { //makeSimpleDFA
                    dfa.makeSimpleDFA(nfa);
                }
                break;

                case '7': { //showSchematicDFA
                    dfa.showSchematicDFA(nfa);
                }
                break;

        }
    }
}
