%{
#include <stdio.h>
int line_number = 1;
int token_count = 0;
void print_token(const char* type, const char* lexeme) {
    token_count++;
    printf("%-4d %-7d %-15s %s\n", token_count, line_number, type, lexeme);
}
%}

DIGIT    [0-9]
ID       [a-zA-Z_][a-zA-Z0-9_]*
NUMBER   {DIGIT}+(\.{DIGIT}+)?
STRING   \"[^"\n]*\"
COMMENT  \/\/.*\n

%%

[ \t]         ; /* Skip whitespace */
\n            { line_number++; }

"if"|"else"|"while"|"for"|"return"|"int"|"float"|"void"|"main" { 
    print_token("keyword", yytext); 
}

{ID}          { print_token("identifier", yytext); }
{NUMBER}      { print_token("constant", yytext); }
{STRING}      { print_token("string", yytext); }
{COMMENT}     { print_token("comment", yytext); line_number++; }

[+\-*/=<>!]   { print_token("operator", yytext); }
[\(\){}\[\]]  { print_token("bracket", yytext); }
[,;]          { print_token("special symbols", yytext); }
[\"]          { print_token("special symbols", yytext); }

.             { printf("UNKNOWN: %s\n", yytext); }

%%

int yywrap() { return 1; }

int main() {
    printf("TOKEN NO LINE NO TOKEN         LEXEME\n");
    yylex();
    return 0;
}