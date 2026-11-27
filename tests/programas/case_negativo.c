// teste: valida o suporte a rótulos com sinal no switch (case negativo) e operador unário mais (+5)
int x = +5;
int r = 0;

switch (x) {
case -5:
r = 1;
break;
case 5:
r = 2;
break;
default:
r = 0;
}

return r;