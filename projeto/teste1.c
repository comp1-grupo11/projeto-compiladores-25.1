#include <stdio.h>

int main()
{
    printf("Hello World!\n");
    // For loop
    for (int i = 0; i < 10; i = i + 1)
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