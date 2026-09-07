#include <stdio.h>

int main(void)
{
    do {
        printf("this can only run once!\n\n");
    } while (1, 1, 0);

    do {
        printf("how can run this forever???\n");
    } while (0, 0, 1);

    return 0;
}