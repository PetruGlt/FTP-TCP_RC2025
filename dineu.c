#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>

int main(int argc, char* argv []){

    if(argc!=2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return 2;
    }

    int fd=open(argv[1],O_RDONLY);
    if(fd == -1)
    {
        perror("open");
        return 2;
    }

    char buf[1];
    ssize_t nread;
    int is_alphanumeric = 1;

    while((nread=read(fd,buf,sizeof(buf)))>0) {
        if(!isalnum(buf[0])) {
            is_alphanumeric = 0;
            break;
        }
    }


    if(nread == -1) {
        perror("read");
        close(fd);
        return 3;
    }

    printf("%s:%s\n", argv[1] , is_alphanumeric ? "alphanumeric" : "not alphanumeric");

    close(fd);
    return 0;

}ok