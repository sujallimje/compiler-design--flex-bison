%{
#include <stdio.h>
#include <stdlib.h>

int open_count = 0;  // Counter for the opening parentheses
%}

%%

"("      { open_count++; }   // Increment counter for '('
")"      { open_count--;     // Decrement counter for ')'
           if (open_count < 0) {
               printf("Parenthesis mismatch: More closing parentheses than opening ones.\n");
               exit(1);  // Exit immediately on mismatched parentheses
           }
        }

. ;         // Ignore all other characters

%%

int main(int argc, char *argv[])  /*** File name can be given in-line ***/
{
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r"); /*** Open the file ***/
    if (!yyin) {
        perror("Error opening file");
        return 1;
    }

    yylex();

    // After the lexer finishes, check if the parentheses were matched
    if (open_count == 0) {
        printf("All parentheses are matched.\n");
    } else {
        printf("Parenthesis mismatch: Unmatched opening parentheses.\n");
    }

    fclose(yyin);  // Don't forget to close the file
    return 0;
}

int yywrap(void)
{
    return 1;
}
