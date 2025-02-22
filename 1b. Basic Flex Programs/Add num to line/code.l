%{
#include <stdio.h>
int lineno = 1;
%}
%%
.*\n?  { printf("%d: %s", lineno++, yytext); }
%%
int main(int argc, char **argv) {
    if (argc > 1) {
        if (!(yyin = fopen(argv[1], "r"))) {
            perror(argv[1]);
            return 1;
        }
    }
    yylex();
    return 0;
}
int yywrap() {
    return 1;
}