// if/else encadeados, blocos e lacos aninhados; no fim, o else pendente (dangling else)
int soma = 0;
for (int i = 0; i < 3; i = i + 1) {
    int j = 0;
    while (j < 3) {
        if (i == j)
            soma = soma + 1;
        else if (i < j)
            soma = soma + 2;
        else
            soma = soma + 3;
        j = j + 1;
    }
}
{
    int dentro = soma;
    dentro = dentro + 1;
}

// o else pertence ao if de dentro: pendente continua 0 e depois vira 10 (se ligasse ao if de fora, viraria 2 e depois 12)
int pendente = 0;
if (0) if (1) pendente = 1; else pendente = 2;
pendente = pendente + 10;
