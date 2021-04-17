#ifndef NFA_H
#define NFA_H

#include <vector>
#include <map>
#include <utility>
#include <queue>
#include <algorithm>
#include "RegExp.h"
#include "Graphics.h"
#include <cstdio>

class NFA {
public:
    void getInput();
    const bool isAcceptByNFA();
    void createEquivalentDFA();
    void createEquivalentDFA2(vector < string >& states2, vector < string >& sigma2, vector < string >& finalStates2,
                              map < string, vector < pair < string, string > > >& trans2);
    const string findRegExp();
    void showSchematicNFA();
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
    map < string, vector < pair < string, string > > > trans;
    void showStarBar(const int n);
    const bool checkSetFormat(const string s, bool finalStateCheck = false);
    const bool checkState(const string s);
    const bool checkTransFormat(const string s);
    const bool checkAlpha(const string s);
    const bool unorderedCheckStates(const string s, vector < string > DFAStates);
    void storeStates(const string s);
    void storeSigma(const string s);
    void storeFinalStates(const string s);
    void storeTrans(const string s);
    const bool dfs(string s, string start);
    void dfs2(string s, string letter, vector < string >& res, bool flg = false);
    string addTrans(string s, string letter);
    const bool isFinalState(const string state);
    const bool isThereFinalState(const string s);
    void blitStates();
    void blitTrans(Graphics& window);
    const int getTransInd(string s);
    void drawTrans(int st, int ed, int lvl, Graphics& window, string s);
    const int getStateInd(string s);
};

const int NFA::getStateInd(string s) {
    for (int i = 0; i < states.size(); i++) {
        if (states[i] == s) {
            return i;
        }
    }
    cout << "This is a bug in getStateInd functionnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn\n";
}

const string NFA::findRegExp() {
    const string EMPTY = "@";
    const string LAMBDA = "%";
    const string NEWFINALSTATE = "newFinalState";
    vector < vector < RegExp > > tbl; tbl.resize(states.size() + 1); // tbl[i][j]: RegExp for path from i to j
    for (int i = 0; i < states.size() + 1; i++) {
        tbl[i].resize(states.size() + 1);
    }
    for (int i = 0; i < states.size() + 1; i++) {
        tbl[i][i].setVal(LAMBDA); //Every single state has a lambda transition to itself
    }
    map < string, vector < pair < string, string > > > newTrans = trans;
    states.push_back(NEWFINALSTATE);
    int n = states.size();
    for (int i = 0; i < finalStates.size(); i++) { //Add new finals ignore old finals
        string cur = finalStates[i];
        if (newTrans.find(cur) == newTrans.end()) {
            vector < pair < string, string > > existing;
            newTrans[cur] = existing;
        }
        newTrans[cur].push_back(make_pair(NEWFINALSTATE, "")); //LAMBDA transition to new added final state
    }
    map < string, vector < pair < string, string > > >::iterator it;
    for (it = newTrans.begin(); it != newTrans.end(); it++) { //Remove multi edges
        string src = it -> first;
        for (int i = 0; i < it -> second.size(); i++) {
            string dest = it -> second[i].first;
            string letter = it -> second[i].second;
            if (letter.empty()) {
                letter = LAMBDA;
            }
            tbl[getStateInd(src)][getStateInd(dest)].setVal(RegExp::Union(tbl[getStateInd(src)][getStateInd(dest)].getVal(), letter));
            //cout << "Remove multi edges " << src << " " << dest << " " << tbl[getStateInd(src)][getStateInd(dest)].getVal() << endl;
        }
    }
    for (int k = 1; k < n - 1; k++) { //Init state and final state remain, we remove other states
        for (int i = 0; i < n; i++) {
            if (i == k || (i < k && i != 0)) {
                continue;
            }
            //Below expressions are equivalent to tbl[][] += tbl[][].(tbl[][])*.tbl[][]
            tbl[i][i].setVal(RegExp::Union(tbl[i][i].getVal(), RegExp::Concat(tbl[i][k].getVal(), RegExp::Concat(RegExp::Star(tbl[k][k].getVal()), tbl[k][i].getVal()))));
        }
        for (int i = 0; i < n; i++) {
            if (i == k || (i < k && i != 0)) {
                continue;
            }
            for (int j = i + 1; j < n; j++) {
                if (j == k || (j < k && j != 0)) {
                    continue;
                }
                //Both of below expressions are equivalent to tbl[][] += tbl[][].(tbl[][])*.tbl[][]
                tbl[i][j].setVal(RegExp::Union(tbl[i][j].getVal(), RegExp::Concat(tbl[i][k].getVal(), RegExp::Concat(RegExp::Star(tbl[k][k].getVal()), tbl[k][j].getVal()))));
                tbl[j][i].setVal(RegExp::Union(tbl[j][i].getVal(), RegExp::Concat(tbl[j][k].getVal(), RegExp::Concat(RegExp::Star(tbl[k][k].getVal()), tbl[k][i].getVal()))));
                cout << "State to remove: " << states[k] << endl;
                cout << states[i] << " to " << states[i] << " " << tbl[i][i].getVal() << endl;
                cout << states[j] << " to " << states[j] << " " << tbl[j][j].getVal() << endl;
                cout << states[i] << " to " << states[j] << " " << tbl[i][j].getVal() << endl;
                cout << states[j] << " to " << states[i] << " " << tbl[j][i].getVal() << endl << endl;
            }
        }
    }
    states.erase(states.begin() + n - 1); //It was new final state so we don't want that anymore
    //Below expression is equivalent to (tbl[0][0])*.tbl[0][n-1].(tbl[n-1][0].(tbl[0][0])*.tbl[0][n-1] + tbl[n-1][n-1])*
    /*cout << "tbl[0][0]:" << tbl[0][0].getVal() << endl;
    cout << "tbl[0][n - 1]:" << tbl[0][n - 1].getVal() << endl;
    cout << "Debug: " << RegExp::Concat(RegExp::Star(tbl[0][0].getVal()), tbl[0][n - 1].getVal()) << endl;
    cout << "Debug: " << RegExp::Concat(tbl[n - 1][0].getVal(), RegExp::Concat(RegExp::Star(tbl[0][0].getVal()), tbl[0][n - 1].getVal())) << endl;
    cout << "Debug: " << tbl[n - 1][n - 1].getVal() << endl;*/
    return RegExp::Concat(RegExp::Concat(RegExp::Star(tbl[0][0].getVal()), tbl[0][n - 1].getVal()), RegExp::Star(RegExp::Union(RegExp::Concat(tbl[n - 1][0].getVal(), RegExp::Concat(RegExp::Star(tbl[0][0].getVal()), tbl[0][n - 1].getVal())), tbl[n - 1][n - 1].getVal())));
}

void NFA::drawTrans(int st, int ed, int lvl, Graphics& window, string s) {
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

const int NFA::getTransInd(string s) {
    for (int i = 0; i < states.size(); i++) {
        if (states[i] == s) {
            return i;
        }
    }
    cout << "It is a Bugggggggggggggggggggggggggggggggg\n";
}

void NFA::blitTrans(Graphics& window) {
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

void NFA::blitStates() {
    for (int x = HORIZONTALLINEWIDTH, i = 0; i < states.size(); x += (SCREENWIDTH - HORIZONTALLINEWIDTH) / states.size(),
     i++) {
        if (isFinalState(states[i])) {
            window.blit(finalImg, x, STATEY, STATEWIDTH, STATEHEIGHT);
        } else {
            window.blit(nonFinal, x, STATEY, STATEWIDTH, STATEHEIGHT);
        }
        window.blitText(states[i], x + STATEWIDTH / 3, STATEY + STATEHEIGHT / 3, 100, 100);
    }
}

void NFA::showSchematicNFA() {
    window = Graphics(SCREENWIDTH, SCREENHEIGHT, "Schematic NFA");
    nonFinal = window.loadImage("nonFinal.png");
    finalImg = window.loadImage("final.png");
    horizontalArrow = window.loadImage("horizontalArrow.png");
    window.blitText("Press space key to close the window", 400, 675, 100, 100);
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

void NFA::createEquivalentDFA2(vector < string >& states2, vector < string >& sigma2, vector < string >& finalStates2,
                          map < string, vector < pair < string, string > > >& trans2) {
    vector < string > DFAstates;
    vector < string > DFAsigma = sigma;
    vector < string > DFAFinalStates;
    map < string, vector < pair < string, string > > > DFATrans;

    //Add initial state
    queue < string > q;
    string initialState = "{"; initialState += startNode; initialState += "}";
    q.push(initialState);
    DFAstates.push_back(initialState);

    //Construct the other states
    while (!q.empty()) {
        string top = q.front(); q.pop();
        for (int i = 0; i < DFAsigma.size(); i++) {
            string newlyConstructedState = addTrans(top, DFAsigma[i]);
            if (!unorderedCheckStates(newlyConstructedState, DFAstates)) {
                DFAstates.push_back(newlyConstructedState);
                q.push(newlyConstructedState);
            }
            DFATrans[top].push_back(make_pair(newlyConstructedState, DFAsigma[i]));
        }
    }

    //Specify final states
    for (int i = 0; i < DFAstates.size(); i++) {
        if (isThereFinalState(DFAstates[i])) {
            DFAFinalStates.push_back(DFAstates[i]);
        }
    }

    //Assign to DFA object refferences
    states2 = DFAstates;
    sigma2 = DFAsigma;
    finalStates2 = DFAFinalStates;
    trans2 = DFATrans;
}

const bool NFA::unorderedCheckStates(const string s, vector < string > DFAStates) { //true -> found, false -> not found
    map < string, bool > visited1;
    map < string, bool > visited2;
    for (int i = 0; i < states.size(); i++) {
        visited1[states[i]] = false;
    }
    string state;
    for (int i = 1; i < s.size(); i++) {
        if (s[i] != ',' && s[i] != '}') {
            state += s[i];
        } else {
            visited1[state] = true;
            state = "";
        }
    }
    bool found = false;
    for (int j = 0; j < DFAStates.size(); j++) {
        //Initialize to false
        for (int i = 0; i < states.size(); i++) {
            visited2[states[i]] = false;
        }
        //Visit
        for (int i = 1; i < DFAStates[j].size(); i++) {
            if (DFAStates[j][i] != ',' && DFAStates[j][i] != '}') {
                state += DFAStates[j][i];
            } else {
                visited2[state] = true;
                state = "";
            }
        }
        //Check
        bool isEqual = true;
        for (int i = 0; i < states.size(); i++) {
            if (visited2[states[i]] != visited1[states[i]]) {
                isEqual = false;
            }
        }
        found |= isEqual;
    }
    return found;
}

const bool NFA::isThereFinalState(const string s) {
    string state;
    for (int i = 1; i < s.size(); i++) {
        if (s[i] != ',' && s[i] != '}') {
            state += s[i];
        } else {
            if (isFinalState(state)) {
                return true;
            }
            state = "";
        }
    }
    return false;
}

void NFA::dfs2(string s, string letter, vector < string >& res, bool flg) {
    for (int i = 0; i < trans[s].size(); i++) {
        if (trans[s][i].second == letter && !flg) {
            if (find(res.begin(), res.end(), trans[s][i].first) == res.end()) {
                res.push_back(trans[s][i].first);
                dfs2(trans[s][i].first, letter, res, true); //Letter doesn't matter here
            }
        } else if (!trans[s][i].second.size()) { //Landa transition
            if (find(res.begin(), res.end(), trans[s][i].first) == res.end()) {
                if (flg) {
                    res.push_back(trans[s][i].first);
                    dfs2(trans[s][i].first, letter, res, true); //Letter doesn't matter here
                } else {
                    dfs2(trans[s][i].first, letter, res, false); //Letter doesn't matter here
                }
            }
        }
    }
}

string NFA::addTrans(string s, string letter) {
    string res = "{";

    vector < string > tmp; //Store states here so we can work easily with them
    string state = "";
    for (int i = 1; i < s.size(); i++) {
        if (s[i] != ',' && s[i] != '}') {
            state += s[i];
        } else {
            tmp.push_back(state);
            state = "";
        }
    }
    vector < string > vectorRes;
    for (int i = 0; i < tmp.size(); i++) {
        vector < string > tmpVector; dfs2(tmp[i], letter, tmpVector);
        for (int j = 0; j < tmpVector.size(); j++) {
            if (find(vectorRes.begin(), vectorRes.end(), tmpVector[j]) == vectorRes.end()) {
                vectorRes.push_back(tmpVector[j]);
            }
        }
    }
    for (int i = 0; i < vectorRes.size(); i++) {
        res += vectorRes[i];
        if (i != vectorRes.size() - 1) {
            res += ',';
        }
    }

    res += "}";
    return res;
}

void NFA::createEquivalentDFA() {
    vector < string > DFAstates;
    vector < string > DFAsigma = sigma;
    vector < string > DFAFinalStates;
    map < string, vector < pair < string, string > > > DFATrans;

    //Add initial state
    queue < string > q;
    string initialState = "{"; initialState += startNode; initialState += "}";
    q.push(initialState);
    DFAstates.push_back(initialState);

    //Construct the other states
    while (!q.empty()) {
        string top = q.front(); q.pop();
        for (int i = 0; i < DFAsigma.size(); i++) {
            string newlyConstructedState = addTrans(top, DFAsigma[i]);
            if (!unorderedCheckStates(newlyConstructedState, DFAstates)) {
                DFAstates.push_back(newlyConstructedState);
                q.push(newlyConstructedState);
            }
            DFATrans[top].push_back(make_pair(newlyConstructedState, DFAsigma[i]));
        }
    }

    //Specify final states
    for (int i = 0; i < DFAstates.size(); i++) {
        if (isThereFinalState(DFAstates[i])) {
            DFAFinalStates.push_back(DFAstates[i]);
        }
    }

    //Print output
    cout << "Created DFA properties:\n\nList of states:\n";
    for (int i = 0; i < DFAstates.size(); i++) {
        cout << DFAstates[i] << endl;
    }
    cout << "List of alphabet: ";
    for (int i = 0; i < DFAsigma.size(); i++) {
        cout << DFAsigma[i] << (i != DFAsigma.size() - 1 ? ',' : '\n');
    }
    cout << "List of final states:\n";
    for (int i = 0; i < DFAFinalStates.size(); i++) {
        cout << DFAFinalStates[i] << endl;
    }
    cout << "List of transitions:\n";
    for (map < string, vector < pair < string, string > > >::iterator it = DFATrans.begin(); it != DFATrans.end(); it++) {
        for (int i = 0; i < it -> second.size(); i++){
            cout << it -> first << " -> " << it -> second[i].first << " ,   letter: " << it -> second[i].second << endl;
        }
    } cout << endl;
}

const bool NFA::isFinalState(const string state) {
    for (int i = 0; i < finalStates.size(); i++) {
        if (finalStates[i] == state) {
            return true;
        }
    }
    return false;
}

const bool NFA::dfs(string s, string start) {
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

const bool NFA::isAcceptByNFA() {
    cout << endl;
    showStarBar(STARLENGTH);
    cout << "Enter a string to check whether it is accepted or not\n";
    showStarBar(STARLENGTH);
    cout << endl;
    string s;
    cin.ignore(); getline(cin, s);
    return dfs(s, startNode);
}

void NFA::storeTrans(const string s) {
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

void NFA::storeSigma(const string s) {
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

void NFA::storeStates(const string s) {
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

void NFA::storeFinalStates(const string s) {
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

const bool NFA::checkAlpha(const string s) {
    if (!s.size()) { //Landa transition
        return true;
    }
    for (int i = 0; i < sigma.size(); i++) {
        if (s == sigma[i]) {
            return true;
        }
    }
    cout << "There is not any letter named " << s << endl;
    return false;
}

const bool NFA::checkState(const string s) {
    for (int i = 0; i < states.size(); i++) {
        if (states[i] == s) {
            return true;
        }
    }
    cout << "There is not any state named " << s << endl;
    return false;
}

const bool NFA::checkTransFormat(const string s) {
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

const bool NFA::checkSetFormat(const string s, bool finalStateCheck) {
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

void NFA::getInput() {

    //Get states
    cout << endl;
    showStarBar(STARLENGTH);
    cout << "Enter set of states\n";
    cout << "Sample: {q0, q1, q2, q3, q4}\n";
    showStarBar(STARLENGTH);
    cout << endl;
    string statesRaw;
    getline(cin, statesRaw);
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
    cout << endl;
    showStarBar(STARLENGTH);
    cout << "Enter number of transitions\n";
    showStarBar(STARLENGTH);
    cout << endl;
    int n;
    cin >> n; cin.ignore();
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
        cout << "Sample:\nq0, q1, a\nq0, q1,\n";
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

void NFA::showStarBar(const int n) {
    for (int i = 0; i < n; i++) {
        cout << '*';
    } cout << endl;
}

#endif // NFA_H
