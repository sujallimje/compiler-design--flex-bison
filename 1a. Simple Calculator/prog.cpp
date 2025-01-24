#include <iostream>
#include <stack>
#include <string>
#include <cmath>
#include <cctype>
#include <sstream>

using namespace std;

bool isOperator(char c) {
    return (c == '+' || c == '-' || c == '*' || c == '/' || c == '^');
}

int getPrecedence(char op) {
    if (op == '^')
        return 3;
    else if (op == '*' || op == '/')
        return 2;
    else if (op == '+' || op == '-')
        return 1;
    return 0;
}

bool isBalanced(string expr) {
    stack<char> s;
    for (char c : expr) {
        if (c == '(') {
            s.push(c);
        } else if (c == ')') {
            if (s.empty()) return false;
            s.pop();
        }
    }
    return s.empty();
}

string infixToPostfix(string infix) {
    stack<char> st;
    string postfix;
    string num;
    
    for (size_t i = 0; i < infix.length(); i++) {
        char c = infix[i];
        
        if (isdigit(c) || c == '.') {
            num += c;
            continue;
        } else if (!num.empty()) {
            postfix += num + " ";
            num.clear();
        }
        
        if (c == ' ') continue;
        
        if (c == '(') {
            st.push(c);
        }
        else if (c == ')') {
            while (!st.empty() && st.top() != '(') {
                postfix += st.top();
                postfix += " ";
                st.pop();
            }
            if (!st.empty()) st.pop();
        }
        else if (isOperator(c)) {
            while (!st.empty() && st.top() != '(' && 
                   ((getPrecedence(st.top()) > getPrecedence(c)) || 
                    (getPrecedence(st.top()) == getPrecedence(c) && (c != '^' || st.top() != '^')))) {
                postfix += st.top();
                postfix += " ";
                st.pop();
            }
            st.push(c);
        }
    }
    
    if (!num.empty()) {
        postfix += num + " ";
    }
    
    while (!st.empty()) {
        if (st.top() != '(') {
            postfix += st.top();
            postfix += " ";
        }
        st.pop();
    }
    
    return postfix;
}

double evaluatePostfix(string postfix) {
    stack<double> st;
    stringstream ss(postfix);
    string token;
    
    while (ss >> token) {
        if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) {
            st.push(stod(token));
        }
        else if (token.length() == 1 && isOperator(token[0])) {
            if (st.size() < 2) {
                throw runtime_error("Invalid expression");
            }
            
            double val2 = st.top(); st.pop();
            double val1 = st.top(); st.pop();
            
            switch (token[0]) {
                case '+': st.push(val1 + val2); break;
                case '-': st.push(val1 - val2); break;
                case '*': st.push(val1 * val2); break;
                case '/': 
                    if (val2 == 0) throw runtime_error("Division by zero");
                    st.push(val1 / val2); 
                    break;
                case '^': 
                    st.push(pow(val1, val2));
                    break;
            }
        }
    }
    
    if (st.size() != 1) {
        throw runtime_error("Invalid expression");
    }
    
    return st.top();
}

int main() {
    string input;
    cout << "Welcome to Calculator Program!" << endl;
    cout << "Supported operators: +, -, *, /, ^, and ()" << endl;
    cout << "Enter 'exit' to quit the program" << endl << endl;
    
    while (true) {
        cout << "Enter an expression: ";
        getline(cin, input);
        
        if (input == "exit" || input == "quit") {
            cout << "Thank you for using the calculator!" << endl;
            break;
        }
        
        if (input.empty()) continue;
        
        try {
            if (!isBalanced(input)) {
                cout << "Error: Unbalanced parentheses" << endl;
                continue;
            }
            
            string postfix = infixToPostfix(input);
            double result = evaluatePostfix(postfix);
            
            cout << "Result: " << result << endl;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
        cout << endl;
    }
    
    return 0;
}
