#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int exit_code = 0;
    int fd_in, fd_out;
    struct stat stat;
    off64_t len, ret;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd_in = open(argv[1], O_RDONLY);
    if (fd_in == -1)
    {
        fprintf(stderr,"Can't open %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    if (fstat(fd_in, &stat) == -1)
    {
        perror("fstat");
        exit_code = 1;
        goto fd_in;
    }

    len = stat.st_size;

    fd_out = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd_out == -1)
    {
        fprintf(stderr,"Can't open %s\n", argv[2]);
        exit_code = 1;
        goto fd_in;
    }

    if (fchmod(fd_out, stat.st_mode))
    {
        perror("fchmod");
    }

    int bytes = 0;
    do
    {
        ret = copy_file_range(fd_in, NULL, fd_out, NULL, len, 0);
        if (ret == -1)
        {
            perror("copy_file_range");
            exit_code = 1;
            break;
        }
        bytes += ret;
        len -= ret;
    } while (len > 0 && ret > 0);

    printf("PID: %d Bytes copied: %d File name: %s\n", getpid(), bytes, argv[1]);

    if (close(fd_out) == -1)
    {
        perror("close fd_out");
        exit_code = 1;
    }
fd_in:
    if (close(fd_in) == -1)
    {
        perror("close fd_in");
        exit_code = 1;
    }

    return exit_code;

}
