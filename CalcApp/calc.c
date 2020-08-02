#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

enum state {
    INITIAL, NONZEROINTEGER, DECIMAL, OP, FINAL
};

char showing[50] = "0";
enum state state = INITIAL;
int showingLen = 0;
double operand1 = 0.0;
double operand2 = 0.0;
char op = 0;

char *getDisplayText() {
    return showing;
}

int charIsDigit(char c) {
    return c >= '0' && c <= '9';
}

int charIsOperation(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

void putCharAt(char c, int index) {
    showing[index] = c;
    showing[index + 1] = 0;
}

void inputChar(char c) {
    if (c == 'C') {
        strcpy(showing, "0");
        showingLen = 0;
        state = INITIAL;
        operand1 = 0.0;
        operand2 = 0.0;
        op = 0;
    } else if (charIsOperation(c)) {
        operand1 = strtod(showing, NULL);
        op = c;
        state = OP;
    } else {
        while (c != 0) {
            switch (state) {
                case INITIAL:
                    if (c != '0') {
                        if (charIsDigit(c)) {
                            state = NONZEROINTEGER;
                            putCharAt(c, showingLen++);
                        } else if (c == '.') {
                            state = DECIMAL;
                            putCharAt(c, 1);
                            showingLen += 2;
                        }
                    }
                    c = 0;
                    break;
                case NONZEROINTEGER:
                    if (charIsDigit(c)) {
                        putCharAt(c, showingLen++);
                        c = 0;
                    } else if (c == '.') {
                        state = DECIMAL;
                        putCharAt(c, showingLen++);
                        c = 0;
                    } else if (c == '=') {
                        operand2 = strtod(showing, NULL);
                        state = FINAL;
                    } else {
                        c = 0;
                    }
                    break;
                case DECIMAL:
                    if (charIsDigit(c)) {
                        putCharAt(c, showingLen++);
                        c = 0;
                    } else if (c == '=') {
                        operand2 = strtod(showing, NULL);
                        state = FINAL;
                    } else {
                        c = 0;
                    }
                    break;
                case OP:
                    if (charIsDigit(c)) {
                        state = INITIAL;
                        strcpy(showing, "0");
                        showingLen = 0;
                    } else {
                        c = 0;
                    }
                    break;
                case FINAL:
                    if (c == '=') {
                        if (op) {
                            switch (op) {
                                case '+':
                                    operand1 += operand2;
                                    break;
                                case '-':
                                    operand1 -= operand2;
                                    break;
                                case '*':
                                    operand1 *= operand2;
                                    break;
                                case '/':
                                    operand1 /= operand2;
                                    break;
                                case '^':
                                    operand1 = pow(operand1, operand2);
                                    break;
                            }
                            state = FINAL;
                            sprintf(showing, "%0.9g", operand1);
                            operand1 = strtod(showing, NULL);
                            showing[30] = 0;
                        }
                    }
                    c = 0;
            }
        }
    }
}

int testesmain() {
    showing[0] = 48;
    showing[1] = 0;
    char c;
    do {
        c = getchar();
        inputChar(c);
        printf("showing: %s\nlen: %i\nop1: %f\nop2: %f\nstate: %i\n", showing, showingLen, operand1, operand2, state);
    } while (c != 's');
}
