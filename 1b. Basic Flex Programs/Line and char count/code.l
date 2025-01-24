%{
#include <stdio.h>

int line_count = 0;  // To count the number of lines
int char_count = 0;  // To count the number of characters
%}

%%

\n      { line_count++; }  // Increment line count on newline
.       { char_count++; }   // Increment character count for each character

%%

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");  // Open the file for reading
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    yyin = file;  // Assign the input file to yyin
    yylex();      // Start lexical analysis

    // After the lexer finishes, print the counts
    printf("Lines: %d\n", line_count);
    printf("Characters: %d\n", char_count);

    fclose(file);  // Close the file
    return 0;
}

int yywrap(void) {
    return 1;  // Indicates no more input is available
}
