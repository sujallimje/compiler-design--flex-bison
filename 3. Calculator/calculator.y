/* calculator.y - Bison specification */
%{
#include <stdio.h>
#include <math.h>
void yyerror(char *);
int yylex(void);
%}

%union {
    double dval;
}

%token <dval> NUMBER
%token SQRT CQRT
%type <dval> expr

%left '+' '-'
%left '*' '/'

%%
program:
    program expr '\n'  { printf("= %g\n> ", $2); }
    | /* empty */     { printf("> "); }
    ;

expr:
    NUMBER             { $$ = $1; }
    | expr '+' expr    { $$ = $1 + $3; }
    | expr '-' expr    { $$ = $1 - $3; }
    | expr '*' expr    { $$ = $1 * $3; }
    | '(' expr ')'     { $$ = $2; }
    | SQRT '(' expr ')'{ $$ = sqrt($3); }
    | CQRT '(' expr ')'{ $$ = cbrt($3); }
    ;
%%

void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n> ", s);
}

int main(void) {
    printf("\n=== Simple Calculator ===\n");
    printf("Supported Operations:\n");
    printf("1. Addition (+)\n");
    printf("2. Subtraction (-)\n");
    printf("3. Multiplication (*)\n");
    printf("4. Square Root - sqrt(x)\n");
    printf("5. Cube Root - cqrt(x)\n");
    printf("6. Parentheses for grouping ( )\n");
    
    printf("\nPress Ctrl+D to exit\n");
    printf("Enter expression:\n> ");
    yyparse();
    return 0;
}