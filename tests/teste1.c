#include <stdio.h>
#include<stdlib.h>
#include    <string.h>

int main()
{
    printf("Hello World!\n");
    int i = 0, a = 0;
    i += 1;
    i++;
    i--;
    i -= 1;
    i *= 1;
    i /= 1;
    a += ++i;
    // For loop
    for (int i = 0, j = 1, k = 0; (i < 10 && k <= 0) || !(j < i); i = i + 1, j++, k--)
    {
        // Switch statement
        switch (i)
        {
        case 0:
            printf("Zero");
            break;
        case 5:
            i += ++j;
            printf("Five");
            break;
        default:
            printf("Other");
        }
    }
    return 0;
}