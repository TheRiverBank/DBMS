#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BLOCK_SIZE 512

struct page {
    char *data;
    int page_nr;
};
typedef struct page *page_t;

int open_file(const char *filename) {
    /* Opens file and retuns file handle.
     * If the file is not found, it is created.
     */
    int fd = open(filename, O_RDWR, 0);
    if (fd == -1) {
        fd = creat(filename, 0600);
        close(fd);
        open(filename, O_RDWR, 0);
    }

    return fd;
}   

page_t get_page(const char *filename, int blk_num) {
    page_t p = malloc(sizeof(page_t));
    int fd = open(filename, O_RDWR);
    p->data = NULL;
    p->page_nr = blk_num;

    return p;
}


int read_page(const char *filename, page_t page) {
    int fd = open(filename, O_RDWR);

    if (fd == -1) {
        printf("read_page() error. Faile to get file.");
    }

    if (lseek(fd, BLOCK_SIZE * page->page_nr, SEEK_SET) < 0) {
        printf("read_page() error. Page does not exist.\n");
    }

    if (read(fd, page->data, BLOCK_SIZE) < 0) {
        printf("read_page() error. Failed to read page.\n");
    }   
}

int write_page(const char *filename, page_t page) {
    int fd = open(filename, O_RDWR);

    if (fd == -1) {
        printf("write_page() error. Failed to get file.");
    }

    if (lseek(fd, BLOCK_SIZE * page->page_nr, SEEK_SET) < 0) {
        printf("write_page() error. Page does not exist.\n");
    }

    if (write(fd, page->data, BLOCK_SIZE == -1) == -1) {
        printf("write_page() error. Failed to write data to page.");
    }
}