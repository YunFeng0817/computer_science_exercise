//
// Created by 1160300524 on 17-12-30.
//

#include "csapp.h"

#define str_len 14

void usage();

int main(int argc,char **argv)
{
    int fd;
    if(argc!=2)
        usage();
    char *filename=argv[1];
    char *bufp;
    fd=open(filename,O_RDWR,0);
    bufp = mmap(NULL,str_len,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0);
    bufp[0]='J';
    write(fd,bufp,str_len);
    munmap(bufp,str_len);
    return 0;
}

void usage(){
    printf("<elf_name> <filename>\n");
    exit(0);
}