// operador %: mesma precedência de * e /, resultado truncado como em C
int a = 17 % 5;
int b = -7 % 3;
int c = 2 + 7 % 4 * 2;
char ch = 'A';
int d = ch % 10;
int pares = 0;
for (int i = 0; i < 10; i = i + 1) {
    if (i % 2 == 0) pares = pares + i;
}
return a * 1000 + (b + 1) * 100 + c * 10 + pares - d;
