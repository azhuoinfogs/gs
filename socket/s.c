#include <stdio.h>

/*service.c*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#define MYPORT 3490 /*开放的端口号*/
#define BACKLOG 5 /*指定套接字可以接受的最大未接受客户机请求的数目*/
main() {
  int sockfd, new_fd;
  struct sockaddr_in srvaddr;
  struct sockaddr_in cliaddr;
  int sin_size;
  /*创建套接字描述符*/
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket error");
    exit(1);
  }
  bzero(&srvaddr, sizeof(srvaddr));
  /*用自己的ip地址和端口信息填充一个internet套接字地址结构*/
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_port = htons(MYPORT);
  /*函数bind将服务器的地址和套接字帮定在一起*/
  if (bind(sockfd, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr)) ==
      -1) {
    perror("bind error");
    exit(1);
  }
  /*listen函数告诉内核，这个套接字可以接受来自客户机的请求*/
  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen error");
    exit(1);
  }
  /*处理客户机的请求，调用函数accept来获得一个客户机的连接*/
  for (;;) {
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&cliaddr, &sin_size)) ==
        -1) {
      perror("accept error");
      continue;
    }
    printf("server: got connection from %s \n", inet_ntoa(cliaddr.sin_addr));
    /*向客户起写数据*/
    if (write(new_fd, "Hello,Network!\n", 14) == -1)
      perror("write error!");
    close(new_fd);
  }
  close(sockfd);
}
