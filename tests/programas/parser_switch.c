// switch com case de int e de char, fall-through e break
int r = 0;
int op = 2;
switch (op) {
    case 1:
        r = 10;
        break;
    case 2:
        r = r + 20;
    case 3:
        r = r + 3;
        break;
    case 'a':
        r = 99;
}
