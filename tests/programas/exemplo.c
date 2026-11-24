// soma de 0 a 4 e fatorial de 5
int soma = 0;
int i;
float media;
char letra = 'A';

for (i = 0; i < 5; i = i + 1) {
    soma = soma + i;
}

media = soma / 5.0;

int fat = 1;
int n = 5;
while (n > 1) {
    fat = fat * n;
    n = n - 1;
}

if (fat == 120 && !(soma != 10))
    letra = 'B';
else
    letra = 'C';

if (soma > 100) if (soma > 0) soma = -1; else soma = -2;

(soma + 1) * 2;
