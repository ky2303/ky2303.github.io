// example.c

#include <stdio.h>
#include <stdlib.h>

int main() {
    char str[] = "hello world\n";

    int str_length = sizeof (str) / sizeof (char);

    for (int i=0; i < str_length; i++) {
        printf("%c", str[i]);
    }

    return 0;

}