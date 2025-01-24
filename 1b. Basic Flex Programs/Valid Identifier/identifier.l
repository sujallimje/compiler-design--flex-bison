%{
#include <stdio.h>
%}

%%

[a-zA-Z_][a-zA-Z0-9_]*   { printf("'%s' is a valid identifier.\n", yytext); }
[0-9]+[a-zA-Z_][a-zA-Z0-9_]* { printf("'%s' is NOT a valid identifier.\n", yytext); }  // Reject numbers followed by letters
[0-9]+                   { printf("'%s' is NOT a valid identifier.\n", yytext); }  // Reject numbers
.                         { /* Ignore other characters */ }

%%

int main() {
    printf("Enter text (Ctrl+D to end input):\n");
    yylex();  // Perform lexical analysis
    return 0;
}

int yywrap() {
    return 1;
}
