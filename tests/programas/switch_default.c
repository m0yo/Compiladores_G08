// default no switch: usado quando nenhum case casa, em qualquer posição,
// com fall-through para os case seguintes
int a = 0;
int b = 0;
int c = 0;
switch (9) { case 1: a = 1; break; default: a = a + 10; case 2: a = a + 100; break; }
switch (1) { case 1: b = 5; break; default: b = 99; }
switch (7) { default: c = 3; }
return a * 1000 + b * 10 + c;
