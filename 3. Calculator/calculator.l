/* calculator.l - Flex specification */
%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "y.tab.h"
void yyerror(char *);
%}

%%
[0-9]+(\.[0-9]+)?    { yylval.dval = atof(yytext); return NUMBER; }
"sqrt"               { return SQRT; }
"cqrt"               { return CQRT; }
[-+*/()]             { return *yytext; }
[ \t]               ; /* skip whitespace */
\n                  { return '\n'; }
.                   { yyerror("Invalid character"); }
%%

int yywrap(void) {
    return 1;
}