#include <stdio.h>

int main() {

  long int f1, f2, i;

  f1 = 1;
  f2 = 1; //赋初始值

  for (i = 1; i <= 20; i++) { //循环20次，一次两个，结果为前40项

    printf("%12d%12d", f1, f2); //输出 两项

    f1 = f1 + f2;
    f2 = f2 + f1; //计算下面两项

    if (i % 2 == 0) //一行四个 好看用

      printf("\n");
  }

  return 0;
}
