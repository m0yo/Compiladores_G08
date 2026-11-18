// break, continue, do-while, switch (com fall-through) e return
long pares = 0;
double x = 1.5;

// continue pula os ímpares; break sai em 8
for (int k = 0; k < 100; k = k + 1) {
    if (k == 8) break;
    if (k - (k / 2) * 2 == 1) continue;
    pares = pares + 1;
}

// do-while executa o corpo pelo menos uma vez
int d = 10;
do {
    d = d + 1;
} while (d < 5);

// switch: case 2 cai no case 3 (sem break) e para no break
int r = 0;
switch (pares - 2) {
    case 1:
        r = 100;
        break;
    case 2:
        r = r + 20;
    case 3:
        r = r + 3;
        break;
    case 'a':
        r = -1;
}

// switch sem case correspondente não executa nada
switch (99) {
    case 1:
        r = 0;
}

// break dentro de switch dentro de laço sai só do switch
int voltas = 0;
while (voltas < 3) {
    switch (voltas) {
        case 1:
            break;
    }
    voltas = voltas + 1;
}

return r + voltas;
r = 999;
