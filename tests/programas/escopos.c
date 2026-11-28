// escopos aninhados: bloco, for e switch abrem escopo; o de dentro sombreia o de fora
int a = 1;
{ int a = 2; a = a + 10; int b = 5; }   // não altera o a externo
a = a + 1;                              // 2
for (int i = 0; i < 3; i = i + 1) { a = a + i; }   // 5
int i = 100;                            // não conflita com o i do for
switch (1) { case 1: a = a + 1; }       // 6
return a + i;                           // 106
