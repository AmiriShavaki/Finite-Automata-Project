#ifndef DFA_H
#define DFA_H

#include <vector>
#include <map>
#include <utility>
#include "NFA.h"
#include <algorithm>

class DFA {
public:
    void getInput();
    const bool isAcceptByDFA(NFA& nfa);
    void makeSimpleDFA(NFA& nfa);
    void showSchematicDFA(NFA& nfa);
private:
    const int STARLENGTH = 25;
    const int SCREENWIDTH = 1300;
    const int SCREENHEIGHT = 700;
    const int STATEHEIGHT = 65;
    const int STATEWIDTH = 65;
    const int HORIZONTALLINEHEIGHT = 25;
    const int HORIZONTALLINEWIDTH = 50;
    const int VERTICALLINEHEIGHT = HORIZONTALLINEWIDTH;
    const int VERTICALLINEWIDTH = HORIZONTALLINEHEIGHT;
    const int STATEY = SCREENHEIGHT / 2 - STATEHEIGHT / 2;
    SDL_Texture* nonFinal;
    SDL_Texture* finalImg;
    SDL_Texture* horizontalArrow;
    Graphics window;
    string startNode;
    vector < string > states;
    vector < string > sigma;
    vector < string > finalStates;
    map < string, vector < pair < string, string > > > trans; // (from, (to, letter)) In that order!
    void showStarBar(const int n);
    const bool checkSetFormat(const string s, bool finalStateCheck = false);
    const bool checkState(const string s);
    const bool checkTransFormat(const string s);
    const bool checkAlpha(const string s);
    void storeStates(const string s);
    void storeSigma(const string s);
    void storeFinalStates(const string s);
    void storeTrans(const string s);
    const bool dfs(string s, string start);
    const bool isFinalState(const string state);
    vector < string > getTransByOrder(const string dep);
    const int getGroupInd(string state, vector < vector < pair < string, vector < string > > > >& transTbl);
    const bool areEqualStates(vector <string>& outTrans1, vector <string>& outTrans2,
                         vector < vector < pair < string, vector < string > > > >& transTbl);
    const bool sameGroup(vector < vector < pair < string, vector < string > > > >& transTbl, string& s1, string& s2);
    void blitStates();
    void blitTrans(Graphics& window);
    const int getTransInd(string s);
    void drawTrans(int st, int ed, int lvl, Graphics& window, string s);
};

void DFA::showSchematicDFA(NFA& nfa) {
    nfa.createEquivalentDFA2(states, sigma, finalStates, trans);
    window = Graphics(SCREENWIDTH, SCREENHEIGHT, "Schematic DFA");
    nonFinal = window.loadImage("nonFinal.png");
    finalImg = window.loadImage("final.png");
    horizontalArrow = window.loadImage("horizontalArrow.png");
    window.blitText("Press space key or hit the close button to close the window", 400, 675, 100, 100);
    blitStates();
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    blitTrans(window);
    window.blit(horizontalArrow, 0, SCREENHEIGHT / 2 - HORIZONTALLINEHEIGHT / 2, HORIZONTALLINEWIDTH, HORIZONTALLINEHEIGHT);
    window.update();
    while (true) {
        if (window.getEvents()) {
            break;
        }
        window.delay(5);
    }
    SDL_DestroyTexture(nonFinal); nonFinal = NULL;
    SDL_DestroyTexture(horizontalArrow); horizontalArrow = NULL;
    SDL_DestroyTexture(finalImg); finalImg = NULL;
    window.endGame();
}

void DFA::drawTrans(int st, int ed, int lvl, Graphics& window, string s) {
    int LVLCNST = 20;
    int xOut = HORIZONTALLINEWIDTH + st * ((SCREENWIDTH - HORIZONTALLINEWIDTH) / states.size()) + STATEWIDTH / 2;
    int yOut = STATEY;
    int xIn = HORIZONTALLINEWIDTH + ed * ((SCREENWIDTH - HORIZONTALLINEWIDTH) / states.size()) + STATEWIDTH / 2;
    int yIn = STATEY + STATEHEIGHT;
    window.drawLine(xOut, yOut, xOut, yOut - lvl * LVLCNST);
    if (st < ed) { //forward edge
        window.drawLine(xOut, yOut - lvl * LVLCNST, xIn - STATEWIDTH / 2 - lvl * 5, yOut - lvl * LVLCNST);
        window.drawLine(xIn - STATEWIDTH / 2 - lvl * 5, yOut - lvl * LVLCNST, xIn - STATEWIDTH / 2 - lvl * 5,
                        yOut + lvl * LVLCNST + STATEHEIGHT);
        window.drawLine(xIn - STATEWIDTH / 2 - lvl * 5, yOut + lvl * LVLCNST + STATEHEIGHT, xIn,
                        yOut + lvl * LVLCNST + STATEHEIGHT);
        window.blitText(s, xIn - STATEWIDTH / 2 + 2 - lvl * 5, yOut + lvl * LVLCNST + STATEHEIGHT - 20, 200, 200);
        window.drawArrow(xIn, yOut + lvl * LVLCNST + STATEHEIGHT, xIn, yIn);
    } else { //backward edge / self loop
        window.drawLine(xOut, yOut - lvl * LVLCNST, xIn + STATEWIDTH / 2 + lvl * 5, yOut - lvl * LVLCNST);
        window.drawLine(xIn + STATEWIDTH / 2 + lvl * 5, yOut - lvl * LVLCNST, xIn + STATEWIDTH / 2 + lvl * 5,
                        yOut + lvl * LVLCNST + STATEHEIGHT);
        window.drawLine(xIn + STATEWIDTH / 2 + lvl * 5, yOut + lvl * LVLCNST + STATEHEIGHT, xIn,
                        yOut + lvl * LVLCNST + STATEHEIGHT);
        window.blitText(s, xIn + 2, yOut + lvl * LVLCNST + STATEHEIGHT - 20, 200, 200);
        window.drawArrow(xIn, yOut + lvl * LVLCNST + STATEHEIGHT, xIn, yIn);
    }
}

const int DFA::getTransInd(string s) {
    for (int i = 0; i < states.size(); i++) {
        if (states[i] == s) {
            return i;
        }
    }
    cout << "It is a Bugggggggggggggggggggggggggggggggg\n";
}

void DFA::blitTrans(Graphics& window) {
    map < string, vector < pair < string, string > > >::iterator it;
    string adjMat[states.size()][states.size()];
    for (it = trans.begin(); it != trans.end(); it++) {
        for (int i = 0; i < it -> second.size(); i++) {
            string src = it -> first;
            string dest = it -> second[i].first;
            if (adjMat[getTransInd(src)][getTransInd(dest)].empty()) {
                adjMat[getTransInd(src)][getTransInd(dest)] = (it -> second[i].second == "" ? "eps" : it -> second[i].second);
            } else {
                adjMat[getTransInd(src)][getTransInd(dest)] += "," +
                (it -> second[i].second == "" ? "eps" : it -> second[i].second);
            }
        }
    }
    int cnt = 0;
    for (int i = 0; i < states.size(); i++) {
        for (int j = 0; j < states.size(); j++) {
            if (adjMat[i][j].size()) {
                cnt++;
            }
        }
    }
    int lvl = 1;
    for (int i = 0; i < states.size(); i++) {
        for (int j = 0; j < states.size(); j++) {
            if (adjMat[i][j].size()) {
                drawTrans(i, j, lvl++, window, adjMat[i][j]);
            }
        }
    }
}

void DFA::blitStates() {
    for (int x = HORIZONTALLINEWIDTH, i = 0; i < states.size(); x += (SCREENWIDTH - HORIZONTALLINEWIDTH) / states.size(),
     i++) {
        if (isFinalState(states[i])) {
            window.blit(finalImg, x, STATEY, STATEWIDTH, STATEHEIGHT);
        } else {
            window.blit(nonFinal, x, STATEY, STATEWIDTH, STATEHEIGHT);
        }
        string stateName = "q"; stateName += i + '0';
        window.blitText(stateName, x + STATEWIDTH / 3, STATEY + STATEHEIGHT / 3, 100, 100);
    }
}

const bool DFA::sameGroup(vector < vector < pair < string, vector < string > > > >& transTbl, string& s1, string& s2) {
    int g1;
    int g2;
    for (int i = 0; i < transTbl.size(); i++) {
        for (int j = 0; j < transTbl[i].size(); j++) {
            if (transTbl[i][j].first == s1) {
                g1 = i;
            }
            if (transTbl[i][j].first == s2) {
                g2 = i;
            }
        }
    }
    return g1 == g2;
}

const bool DFA::areEqualStates(vector <string>& outTrans1, vector <string>& outTrans2,
                         vector < vector < pair < string, vector < string > > > >& transTbl) {
    if (outTrans1.size() != outTrans2.size()) {
        cout << "BUUUUUUUUUUUUUUUUUUUUUUUUUGGGGGGGGGGGGGGGGGGGGGGGGGGGGG\n";
    }
    for (int i = 0; i < outTrans1.size(); i++) {
        if (!sameGroup(transTbl, outTrans1[i], outTrans2[i])) {
            return false;
        }
    }
    return true;
}

const int DFA::getGroupInd(string state, vector < vector < pair < string, vector < string > > > >& transTbl) {
    for (int res = 0; res < transTbl.size(); res++) {
        for (int i = 0; i < transTbl[res].size(); i++) {
            if (transTbl[res][i].first == state) {
                return res;
            }
        }
    }
    cout << "Errrrrrrrorrrrrrrr check getGroupInd function please!!!!!!!!!!!!!!!!!\n";
}

vector < string > DFA::getTransByOrder(const string dep) {
    vector < string > res;
    for (int i = 0; i < sigma.size(); i++) {
        string letter = sigma[i];
        for (int j = 0; j < trans[dep].size(); j++) {
            if (letter == trans[dep][j].second) {
                res.push_back(trans[dep][j].first);
                //cout << trans[dep][j].first << "  ";
                break;
            }
        }
    }
    //cout << endl;
    return res;
}

void DFA::makeSimpleDFA(NFA& nfa) {
    nfa.createEquivalentDFA2(states, sigma, finalStates, trans);
    vector < vector < pair < string, vector < string > > > > transTbl;
    // vector of vectors     from     to ..  for each letter in sigma

    //Base case: first grouping process based on being or not being a final state
    vector < pair < string, vector < string > > > finals;
    vector < pair < string, vector < string > > > nonFinals;
    //cout << "Testiiinggggg...\nFinals:\n";
    for (int i = 0; i < finalStates.size(); i++) {
        //cout << finalStates[i] << "          ";
        finals.push_back(make_pair(finalStates[i], getTransByOrder(finalStates[i])));
    }
    //cout << "NonFinalssssss:\n";
    for (int i = 0; i < states.size(); i++) {
        if (find(finalStates.begin(), finalStates.end(), states[i]) == finalStates.end()) { //then states[i] is not a final state
            //cout << states[i] << "          ";
            nonFinals.push_back(make_pair(states[i], getTransByOrder(states[i])));
        }
    }
    transTbl.push_back(nonFinals);
    transTbl.push_back(finals);
    //Make a copy of the table to specify when loop should end and also use in current table calculations
    vector < vector < pair < string, vector < string > > > > oldTransTbl;
    do {
        oldTransTbl = transTbl;
        transTbl.clear();
        for (int i = 0; i < oldTransTbl.size(); i++) { //Group i'th
            vector < pair < string, vector < string > > > curGroup = oldTransTbl[i];
            while (!curGroup.empty()) {
                vector < string > firstMemberOutTrans = curGroup[0].second;
                vector < pair < string, vector < string > > > newGroup;
                newGroup.push_back(curGroup[0]);
                curGroup.erase(curGroup.begin());
                vector < int > eraseQ;
                for (int j = 0; j < curGroup.size(); j++) {
                    if (areEqualStates(curGroup[j].second, firstMemberOutTrans, oldTransTbl)) {
                        eraseQ.push_back(j);
                        newGroup.push_back(curGroup[j]);
                    }
                }
                for (int j = 0; j < eraseQ.size(); j++) {
                    curGroup.erase(curGroup.begin() + eraseQ[eraseQ.size() - 1 - j]); //Erase from the end to prevent errors
                }
                transTbl.push_back(newGroup);
            }
        }
    } while(transTbl.size() != oldTransTbl.size());
    cout << "minimal-state DFA properties:\n\nList of states:\n";
    for (int i = 0; i < transTbl.size(); i++) {
        cout << "g" << i + 1 << endl;
    }
    cout << "List of alphabets:\n";
    for (int i = 0; i < sigma.size(); i++) {
        cout << sigma[i] << endl;
    }
    cout << "List of final states:\n";
    for (int i = 0; i < transTbl.size(); i++) {
        if (find(finalStates.begin(), finalStates.end(), transTbl[i][0].first) != finalStates.end()) {
            cout << "g" << i + 1 << endl;
        }
    }
    cout << "List of transitions:\n";
    for (int i = 0; i < transTbl.size(); i++) {
        for (int j = 0; j < sigma.size(); j++) {
            cout << "g" << i + 1 << " -> g" << getGroupInd(transTbl[i][0].second[j], transTbl) + 1 << "       letter: " <<
            sigma[j] << endl;
        }
    }
}

const bool DFA::isAcceptByDFA(NFA& nfa) {
    return nfa.isAcceptByNFA();
    /*cout << endl;
    showStarBar(STARLENGTH);
    cout << "Enter a string to check whether it is accepted or not\n";
    showStarBar(STARLENGTH);
    cout << endl;
    string s;
    cin.ignore(); getline(cin, s);
    return dfs(s, startNode);*/
}

const bool DFA::isFinalState(const string state) {
    for (int i = 0; i < finalStates.size(); i++) {
        if (finalStates[i] == state) {
            return true;
        }
    }
    return false;
}

const bool DFA::dfs(string s, string start) {
    if (!s.size()) {
        return isFinalState(start);
    }
    bool res = false;
    for (int i = 0; i < trans[start].size(); i++) {
        if (!trans[start][i].second.size()) {
            res |= dfs(s, trans[start][i].first);
        } else if (trans[start][i].second[0] == s[0]) {
            string sCopy = s;
            sCopy.erase(sCopy.begin());
            res |= dfs(sCopy, trans[start][i].first);
        }
    }
    return res;
}

void DFA::storeStates(const string s) {
    string state;
   for (int i = 1; i < s.size(); i++) {
        if (s[i] != ' ' && s[i] != ',' && s[i] != '}') {
            state += s[i];
        } else if (s[i] == ' ') {
            continue;
        } else {
            states.push_back(state);
            cout << "state " << state << " stored successfully\n";
            state = "";
        }
    }
}

const bool DFA::checkSetFormat(const string s, bool finalStateCheck) {
    if (s[0] != '{' || s[s.size() - 1] != '}') {
        return false;
    }
    string state;
    for (int i = 1; i < s.size() - 1; i++) {
        if (s[i] != ' ' && s[i] != ',') {
            state += s[i];
        } else if (s[i] == ' ') {
            continue;
        } else {
            if (finalStateCheck && !checkState(state)) {
                return false;
            }
            if (state.size() == 0) {
                return false;
            }
            state = "";
        }
    }
    return true;
}


void DFA::getInput() {

    //Get states
    cout << endl;
    showStarBar(STARLENGTH);
    cout << "Enter set of states\n";
    cout << "Sample: {q0, q1, q2, q3, q4}\n";
    showStarBar(STARLENGTH);
    cout << endl;
    string statesRaw;
    cin.ignore(); getline(cin, statesRaw);
    while (!checkSetFormat(statesRaw)) {
        cout << "please enter in correct format\n";
        getline(cin, statesRaw);
    }
    storeStates(statesRaw);

    startNode = states[0]; //This is just an assumption maybe later I'll change it

    //Get sigma
    cout << endl;
    showStarBar(STARLENGTH);
    cout << "Enter set of alphabet\n";
    cout << "Sample: {a, b}\n";
    showStarBar(STARLENGTH);
    cout << endl;
    string sigmaRaw;
    getline(cin, sigmaRaw);
    while (!checkSetFormat(sigmaRaw)) {
        cout << "please enter in correct format\n";
        getline(cin, sigmaRaw);
    }
    storeSigma(sigmaRaw);

    //Get final states
    cout << endl;
    showStarBar(STARLENGTH);
    cout << "Enter set of final states\n";
    cout << "Sample: {q1, q3}\n";
    showStarBar(STARLENGTH);
    cout << endl;
    string finalStatesRaw;
    getline(cin, finalStatesRaw);
    while (!checkSetFormat(finalStatesRaw, true)) {
        cout << "please enter in correct format\n";
        getline(cin, finalStatesRaw);
    }
    storeFinalStates(finalStatesRaw);

    //Get transitions
    int n = states.size() * sigma.size();
    cout << endl;
    showStarBar(STARLENGTH);
    cout << "You should enter " << n << " transitions totally\n";
    showStarBar(STARLENGTH);
    cout << endl;
    for (int i = 1; i <= n; i++) {
        string nrSuffix = "st";
        if (i == 2) {
            nrSuffix = "nd";
        } else if (i == 3) {
            nrSuffix = "rd";
        } else if (i > 3) {
            nrSuffix = "th";
        }
        cout << endl;
        showStarBar(STARLENGTH);
        cout << "Enter " << i << nrSuffix << " transition rule\n";
        cout << "Sample:\nq0, q1, a\n";
        showStarBar(STARLENGTH);
        cout << endl;
        string transitionRule;
        getline(cin, transitionRule);
        while (!checkTransFormat(transitionRule)) {
            cout << "please enter in correct format\n";
            getline(cin, transitionRule);
        }
        storeTrans(transitionRule);
        cout << i << nrSuffix << " transition stored successfully\n";
    }
}

void DFA::storeTrans(const string s) {
    string start;
    string end;
    string edge;
    int i = 0;
    for (;i < s.size(); i++) {
        if (s[i] != ' ' && s[i] != ',') {
            start += s[i];
        } else if (s[i] == ',') {
            break;
        }
    }
    for (i++ ;i < s.size(); i++) {
        if (s[i] != ' ' && s[i] != ',') {
            end += s[i];
        } else if (s[i] == ',') {
            break;
        }
    }
    for (i++ ;i < s.size(); i++) {
        if (s[i] != ' ' && s[i] != ',') {
            edge += s[i];
        }
    }
    trans[start].push_back(make_pair(end, edge));
}

const bool DFA::checkTransFormat(const string s) {
    string start;
    string end;
    string edge;
    int i = 0;
    int commaCnt = 0;
    for (;i < s.size(); i++) {
        if (s[i] != ' ' && s[i] != ',') {
            start += s[i];
        } else if (s[i] == ',') {
            commaCnt++;
            break;
        }
    }
    for (i++ ;i < s.size(); i++) {
        if (s[i] != ' ' && s[i] != ',') {
            end += s[i];
        } else if (s[i] == ',') {
            commaCnt++;
            break;
        }
    }
    for (i++ ;i < s.size(); i++) {
        if (s[i] != ' ' && s[i] != ',') {
            edge += s[i];
        } else if (s[i] == ',') {
            commaCnt++;
            break;
        }
    }
    return commaCnt == 2 && checkState(start) && checkState(end) && checkAlpha(edge);
}

const bool DFA::checkAlpha(const string s) {
    if (!s.size()) { //Landa transition
        cout << "Don't try to enter a landa transition 'cause we won't store that in DFA ;)\n";
        return false;
    }
    for (int i = 0; i < sigma.size(); i++) {
        if (s == sigma[i]) {
            return true;
        }
    }
    cout << "There is not any letter named " << s << endl;
    return false;
}

void DFA::storeFinalStates(const string s) {
    string state;
   for (int i = 1; i < s.size(); i++) {
        if (s[i] != ' ' && s[i] != ',' && s[i] != '}') {
            state += s[i];
        } else if (s[i] == ' ') {
            continue;
        } else if (state.size()) {
            finalStates.push_back(state);
            cout << "final state " << state << " stored successfully\n";
            state = "";
        }
    }
}

void DFA::storeSigma(const string s) {
    string letter;
   for (int i = 1; i < s.size(); i++) {
        if (s[i] != ' ' && s[i] != ',' && s[i] != '}') {
            letter += s[i];
        } else if (s[i] == ' ') {
            continue;
        } else {
            sigma.push_back(letter);
            cout << "letter " << letter << " stored successfully\n";
            letter = "";
        }
    }
}

const bool DFA::checkState(const string s) {
    for (int i = 0; i < states.size(); i++) {
        if (states[i] == s) {
            return true;
        }
    }
    cout << "There is not any state named " << s << endl;
    return false;
}



void DFA::showStarBar(const int n) {
    for (int i = 0; i < n; i++) {
        cout << '*';
    } cout << endl;
}

#endif // DFA_H
