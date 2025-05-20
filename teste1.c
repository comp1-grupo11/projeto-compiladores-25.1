#include <stdio.h>

int main()
{
    printf("Hello World!\n");
    int i = 0;
    i += 1;
    i++;
    i--;
    i -= 1;
    i *= 1;
    i /= 1;
    // For loop
    for (int i = 0, j = 1, k = 0; (i < 10 && k <= 0) || j == 1; i = i + 1, j++, k--)
    {
        // Switch statement
        switch (i)
        {
        case 0:
            printf("Zero");
            break;
        case 5:
            printf("Five");
            break;
        default:
            printf("Other");
        }
    }
    return 0;
}