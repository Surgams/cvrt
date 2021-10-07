/*

   This is free and unencumbered software released into the public domain.

   Anyone is free to copy, modify, publish, use, compile, sell, or
   distribute this software, either in source code form or as a compiled
   binary, for any purpose, commercial or non-commercial, and by any
   means.

   In jurisdictions that recognize copyright laws, the author or authors
   of this software dedicate any and all copyright interest in the
   software to the public domain. We make this dedication for the benefit
   of the public at large and to the detriment of our heirs and
   successors. We intend this dedication to be an overt act of
   relinquishment in perpetuity of all present and future rights to this
   software under copyright law.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.

   For more information, please refer to <http://unlicense.org/>
*/


#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

int create_dir (char *path) {
    struct stat st = {0};

    if (stat(path, &st) == - 1) {
        return mkdir(path, 0700);
    }
    return 0;
}


void copy_file (char *srcfile, char *destfile) {
    int sfd, dfd;
    char *src, *dest;
    size_t filesize;

    sfd = open(srcfile, O_RDONLY);
    
    if (sfd == -1) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    filesize = lseek(sfd, 0, SEEK_END);

    src = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, sfd, 0);

    if (src == MAP_FAILED) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    dfd = open(destfile, O_RDWR | O_CREAT, 0700);

    if (dfd == -1) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    if (ftruncate(dfd, filesize) != 0) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    dest = mmap(NULL, filesize, PROT_READ | PROT_WRITE, MAP_SHARED, dfd, 0);

    if (dest == MAP_FAILED) {
        fprintf(stderr, "Error number %d: %s\n", errno, strerror(errno));
        return;
    }

    memcpy(dest, src, filesize);
    
    /* unmap */
    munmap(src, filesize);
    munmap(dest, filesize);

    /* close handlers */
    close(sfd);
    close(dfd);

}
