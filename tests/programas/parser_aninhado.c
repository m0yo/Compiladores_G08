// if/else (else pertence ao if mais proximo), blocos e lacos aninhados
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
