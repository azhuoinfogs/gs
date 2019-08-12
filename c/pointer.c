#include <stdio.h>

int main()
{
	int a=511, *b=&a;
	int *d, c;
	printf("Hello world= %d\n", *b);
	scanf("%d\n", &c);
	*d = c;
    return 0;
}

