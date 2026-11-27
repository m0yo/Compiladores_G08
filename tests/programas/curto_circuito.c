// && e || com curto-circuito: o lado direito não é avaliado quando não precisa
int a = 0;
int b = 5;
int r = 0;
if (a && (1 / a)) r = 1;
if (b || (1 / a)) r = r + 10;
if (a != 0 && b / a > 1) r = r + 100;
int d = b && 3;
int e = a || 0;
return r + d * 1000 + e;
