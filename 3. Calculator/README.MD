bison -dy calculator.y

flex calculator.l

gcc -o calculator lex.yy.c y.tab.c -lm

./calculator
