/* vim: set tabstop=4, shiftwidth=4 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_SEND_TIMES                           (1000)

int main(int argc, const char *argv[])
{
    char server_ip[16];
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if( fd < 0 ) 
    {
        printf("socket error: %s\n", strerror(errno));
        return 1;
    }

#if 1
    struct sockaddr_in src_addr;
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(12342);
    src_addr.sin_addr.s_addr = INADDR_ANY;
    bind(fd, (struct sockaddr *)&src_addr, sizeof(src_addr));
#endif

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(12340);
    memset(server_ip, 0, sizeof(server_ip));
    strcpy(server_ip, argv[1]);
    sa.sin_addr.s_addr = inet_addr(server_ip);

    char *p = "a";

    ssize_t size = sendto(fd, p, 1, 0, (struct sockaddr *)&sa, sizeof(sa));
    if( size < 0 ) 
    {
        printf("sendto error: %s\n", strerror(errno));
        return 1;
    }

    printf("send ok recv now\n");

    char buff[100];
    struct sockaddr_in ca;
    socklen_t slen = sizeof(ca);
    memset(&ca, 0, sizeof(ca));
    size = recvfrom(fd, buff, sizeof(buff), 0, (struct sockaddr *)&ca, &slen);
    if( size < 0 ) 
    {
        printf("recvfrom error: %s\n", strerror(errno));
        return 1;
    }
    struct sockaddr_in *ps = (struct sockaddr_in *)buff;

    printf("recv another client ip: %s, port: %d\n", inet_ntoa(ps->sin_addr), ntohs(ps->sin_port));

    int i;
    for( i = 0; i < MAX_SEND_TIMES; i++ )
    {
        printf("send %d times\n", i);
        p = "hello world";
        ps->sin_family = AF_INET;
        size = sendto(fd, p, strlen(p), 0, (struct sockaddr *)ps, sizeof(struct sockaddr_in));
        if( size < 0 ) 
        {
            printf("sendto error: %s\n", strerror(errno));
            return 1;
        }

        printf("send %d times ok, recv now\n", i);

        char buf[100] = {0};
        struct sockaddr_in ca1;
        memset(&ca1, 0, sizeof(ca1));
        size = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&ca1, &slen);
        if( size < 0 ) 
        {
            printf("recvfrom error: %s\n", strerror(errno));
            return 1;
        }

        printf("recv %d message: %s\n", i, buf);
        sleep(1);
    }

    return 0;
}
