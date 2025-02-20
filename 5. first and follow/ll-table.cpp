#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>

using namespace std;

// Structure to represent a production rule
struct Production {
    char lhs;
    string rhs;
};

class CFGParser {
private:
    vector<Production> productions;
    set<char> nonTerminals;
    set<char> terminals;
    char startSymbol;
    map<char, set<char>> firstSets;
    map<char, set<char>> followSets;
    map<pair<char, char>, string> parsingTable;
    
    // Helper function to check if a character is non-terminal
    bool isNonTerminal(char symbol) {
        return nonTerminals.find(symbol) != nonTerminals.end();
    }
    
    // Helper function to check if a string contains epsilon (represented as '#')
    bool containsEpsilon(const set<char>& s) {
        return s.find('#') != s.end();
    }
    
    // Compute FIRST set for a given symbol or string
    set<char> computeFirst(const string& s) {
        set<char> result;
        
        // If s is empty, return epsilon
        if (s.empty()) {
            result.insert('#');
            return result;
        }
        
        char firstChar = s[0];
        
        // If first character is a terminal
        if (!isNonTerminal(firstChar)) {
            result.insert(firstChar);
            return result;
        }
        
        // If first character is a non-terminal
        for (char c : firstSets[firstChar]) {
            if (c != '#') {
                result.insert(c);
            } else if (s.length() > 1) {
                set<char> firstOfRest = computeFirst(s.substr(1));
                result.insert(firstOfRest.begin(), firstOfRest.end());
            } else {
                result.insert('#');
            }
        }
        
        return result;
    }
    
public:
    // Constructor to initialize with grammar productions
    CFGParser(const vector<Production>& prods, char start) {
        productions = prods;
        startSymbol = start;
        
        // Identify non-terminals and terminals
        for (const auto& prod : productions) {
            nonTerminals.insert(prod.lhs);
        }
        
        for (const auto& prod : productions) {
            for (char c : prod.rhs) {
                if (!isNonTerminal(c) && c != '#') {
                    terminals.insert(c);
                }
            }
        }
        
        // Add $ as a terminal (end marker)
        terminals.insert('$');
    }
    
    // Compute FIRST sets for all non-terminals
    void computeFirstSets() {
        // Initialize FIRST sets for all non-terminals
        for (char nt : nonTerminals) {
            firstSets[nt] = set<char>();
        }
        
        bool changed = true;
        while (changed) {
            changed = false;
            
            for (const auto& prod : productions) {
                char lhs = prod.lhs;
                string rhs = prod.rhs;
                
                // If RHS is epsilon, add epsilon to FIRST(LHS)
                if (rhs == "#") {
                    if (firstSets[lhs].insert('#').second) {
                        changed = true;
                    }
                    continue;
                }
                
                // Find FIRST of the RHS
                set<char> firstOfRHS;
                bool allDeriveEpsilon = true;
                
                for (size_t i = 0; i < rhs.length(); i++) {
                    char symbol = rhs[i];
                    
                    // If symbol is a terminal
                    if (!isNonTerminal(symbol)) {
                        firstOfRHS.insert(symbol);
                        allDeriveEpsilon = false;
                        break;
                    }
                    
                    // If symbol is a non-terminal
                    for (char c : firstSets[symbol]) {
                        if (c != '#') {
                            firstOfRHS.insert(c);
                        }
                    }
                    
                    // If this non-terminal doesn't derive epsilon, we're done
                    if (!containsEpsilon(firstSets[symbol])) {
                        allDeriveEpsilon = false;
                        break;
                    }
                    
                    // If this is the last symbol and it can derive epsilon
                    if (i == rhs.length() - 1 && containsEpsilon(firstSets[symbol])) {
                        firstOfRHS.insert('#');
                    }
                }
                
                // If all symbols in RHS can derive epsilon, add epsilon to FIRST(LHS)
                if (allDeriveEpsilon) {
                    firstOfRHS.insert('#');
                }
                
                // Add computed FIRST to the FIRST set of LHS
                size_t oldSize = firstSets[lhs].size();
                firstSets[lhs].insert(firstOfRHS.begin(), firstOfRHS.end());
                if (firstSets[lhs].size() > oldSize) {
                    changed = true;
                }
            }
        }
    }
    
    // Compute FOLLOW sets for all non-terminals
    void computeFollowSets() {
        // Initialize FOLLOW sets for all non-terminals
        for (char nt : nonTerminals) {
            followSets[nt] = set<char>();
        }
        
        // Add $ to FOLLOW of start symbol
        followSets[startSymbol].insert('$');
        
        bool changed = true;
        while (changed) {
            changed = false;
            
            for (const auto& prod : productions) {
                char lhs = prod.lhs;
                string rhs = prod.rhs;
                
                for (size_t i = 0; i < rhs.length(); i++) {
                    char currSymbol = rhs[i];
                    
                    // Skip terminals
                    if (!isNonTerminal(currSymbol)) continue;
                    
                    // Case 1: A -> αBβ
                    if (i < rhs.length() - 1) {
                        string beta = rhs.substr(i + 1);
                        set<char> firstOfBeta = computeFirst(beta);
                        
                        // Add FIRST(β) - {#} to FOLLOW(B)
                        for (char c : firstOfBeta) {
                            if (c != '#') {
                                if (followSets[currSymbol].insert(c).second) {
                                    changed = true;
                                }
                            }
                        }
                        
                        // If β can derive ε, add FOLLOW(A) to FOLLOW(B)
                        if (containsEpsilon(firstOfBeta)) {
                            size_t oldSize = followSets[currSymbol].size();
                            followSets[currSymbol].insert(followSets[lhs].begin(), followSets[lhs].end());
                            if (followSets[currSymbol].size() > oldSize) {
                                changed = true;
                            }
                        }
                    }
                    // Case 2: A -> αB
                    else {
                        // Add FOLLOW(A) to FOLLOW(B)
                        size_t oldSize = followSets[currSymbol].size();
                        followSets[currSymbol].insert(followSets[lhs].begin(), followSets[lhs].end());
                        if (followSets[currSymbol].size() > oldSize) {
                            changed = true;
                        }
                    }
                }
            }
        }
    }
    
    // Compute the predictive parsing table
    void constructParsingTable() {
        for (const auto& prod : productions) {
            char lhs = prod.lhs;
            string rhs = prod.rhs;
            
            // If RHS is epsilon
            if (rhs == "#") {
                for (char follow : followSets[lhs]) {
                    pair<char, char> entry = make_pair(lhs, follow);
                    if (parsingTable.find(entry) != parsingTable.end()) {
                        cout << "Warning: Grammar is not LL(1). Conflict for [" << lhs << "," << follow << "]" << endl;
                    }
                    parsingTable[entry] = "#";
                }
            } else {
                set<char> firstOfRhs = computeFirst(rhs);
                
                for (char first : firstOfRhs) {
                    if (first != '#') {
                        pair<char, char> entry = make_pair(lhs, first);
                        if (parsingTable.find(entry) != parsingTable.end()) {
                            cout << "Warning: Grammar is not LL(1). Conflict for [" << lhs << "," << first << "]" << endl;
                        }
                        parsingTable[entry] = rhs;
                    } else {
                        // If epsilon is in FIRST(α), for each terminal b in FOLLOW(A)
                        for (char follow : followSets[lhs]) {
                            pair<char, char> entry = make_pair(lhs, follow);
                            if (parsingTable.find(entry) != parsingTable.end()) {
                                cout << "Warning: Grammar is not LL(1). Conflict for [" << lhs << "," << follow << "]" << endl;
                            }
                            parsingTable[entry] = rhs;
                        }
                    }
                }
            }
        }
    }
    
    // Display the FIRST sets
    void displayFirstSets() {
        cout << "FIRST Sets:" << endl;
        for (char nt : nonTerminals) {
            cout << "FIRST(" << nt << ") = { ";
            bool first = true;
            for (char symbol : firstSets[nt]) {
                if (!first) cout << ", ";
                cout << symbol;
                first = false;
            }
            cout << " }" << endl;
        }
        cout << endl;
    }
    
    // Display the FOLLOW sets
    void displayFollowSets() {
        cout << "FOLLOW Sets:" << endl;
        for (char nt : nonTerminals) {
            cout << "FOLLOW(" << nt << ") = { ";
            bool first = true;
            for (char symbol : followSets[nt]) {
                if (!first) cout << ", ";
                cout << symbol;
                first = false;
            }
            cout << " }" << endl;
        }
        cout << endl;
    }
    
    // Display the parsing table
    void displayParsingTable() {
        cout << "Predictive Parsing Table:" << endl;
        
        // Print header
        cout << "\t";
        for (char t : terminals) {
            cout << t << "\t";
        }
        cout << endl;
        
        // Print rows
        for (char nt : nonTerminals) {
            cout << nt << "\t";
            for (char t : terminals) {
                pair<char, char> entry = make_pair(nt, t);
                if (parsingTable.find(entry) != parsingTable.end()) {
                    cout << nt << " -> " << parsingTable[entry] << "\t";
                } else {
                    cout << "\t";
                }
            }
            cout << endl;
        }
    }
    
    // Execute the complete parsing analysis
    void analyze() {
        computeFirstSets();
        computeFollowSets();
        constructParsingTable();
        
        displayFirstSets();
        displayFollowSets();
        displayParsingTable();
    }
};

int main() {
    vector<Production> grammar;
    char startSymbol;
    
    cout << "Enter the number of productions: ";
    int n;
    cin >> n;
    
    cout << "Enter productions in the format 'A->α' (use '#' for epsilon):" << endl;
    
    for (int i = 0; i < n; i++) {
        string prodStr;
        cin >> prodStr;
        
        size_t arrowPos = prodStr.find("->");
        if (arrowPos != string::npos) {
            char lhs = prodStr[0];
            string rhs = prodStr.substr(arrowPos + 2);
            grammar.push_back({lhs, rhs});
        } else {
            cout << "Invalid production format. Please try again." << endl;
            i--;
        }
    }
    
    cout << "Enter the start symbol: ";
    cin >> startSymbol;
    
    if (grammar.empty()) {
        cout << "No valid productions entered. Using example grammar instead." << endl;
        // Example grammar (correct E-prime notation):
        // E -> TX
        // X -> +TX | #
        // T -> FY
        // Y -> *FY | #
        // F -> (E) | i
        grammar = {
            {'E', "TX"},
            {'X', "+TX"},
            {'X', "#"},
            {'T', "FY"},
            {'Y', "*FY"},
            {'Y', "#"},
            {'F', "(E)"},
            {'F', "i"}  // 'i' represents 'id'
        };
        startSymbol = 'E';
    }
    
    CFGParser parser(grammar, startSymbol);
    parser.analyze();
    
    return 0;
}
