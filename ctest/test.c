#include <stdio.h>
#include <string.h>

int main(void)
{
    char str[] = "ABC";
    char str1[] = "ABC";
    char str2[] = "ABD";
    char str3[] = "B";
    char str4[] = "AAA";

    printf("strcmp(%s, %s) = %d\n", str, str1, strcmp(str, str1));
    printf("strcmp(%s, %s) = %d\n", str, str2, strcmp(str, str2));
    printf("strcmp(%s, %s) = %d\n", str, str3, strcmp(str, str3));
    printf("strcmp(%s, %s) = %d\n", str, str4, strcmp(str, str4));

    return 0;
}
