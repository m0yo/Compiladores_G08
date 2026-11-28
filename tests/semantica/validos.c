// usos validos: nada de erro nem aviso
int soma = 0;
for (int i = 0; i < 6; i = i + 1) {
    if (i == 2) continue;
    switch (i) {
        case 4:
            break;
    }
    soma = soma + i;
}
while (soma > 100) break;
