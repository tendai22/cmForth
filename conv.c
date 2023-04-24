#include <stdio.h>

int main(int ac, char **av)
{
    int c;
    int count = 0;
    while ((c = getchar()) != EOF) {
        if (c != '\n') {
            putchar(c);
            continue;
        }
        if (count++ & 1) {
            putchar (c);
            continue;
        }
        // ignore it.
    }
}