#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512

struct page {
    char *data;
    int page_nr;
    int current_pos;
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

int close_file(int fd) {
    close(fd);
}

page_t get_page(const char *filename, int blk_num) {
    page_t p = malloc(sizeof(page_t));
    int fd = open_file(filename);
    p->data = malloc(BLOCK_SIZE);
    memset(p->data, 0, BLOCK_SIZE);
    p->page_nr = blk_num;
    p->current_pos = BLOCK_SIZE * blk_num;
    
    return p;
}

int read_page(const char *filename, page_t page) {
    int fd = open_file(filename);

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
    /* Writes page to disk */
    int fd = open_file(filename);

    if (fd == -1) {
        printf("write_page() error. Failed to get file.");
    }

    if (lseek(fd, BLOCK_SIZE * page->page_nr, SEEK_SET) < 0) {
        printf("write_page() error. Page pos does not exist.\n");
    }

    if (write(fd, page->data, BLOCK_SIZE) == -1) {
        printf("write_page() error. Failed to write data to page.\n");
    }
}

int page_set_current_pos(const char *filename, int pos, page_t page) {
    int offset = (page->page_nr * BLOCK_SIZE) + pos;
    if ((page->page_nr * BLOCK_SIZE) + pos < (page->page_nr * BLOCK_SIZE) + BLOCK_SIZE) {
        int fd = open_file(filename);
        lseek(fd, offset, SEEK_SET);
        page->current_pos = offset;
    }
    else {
        printf("Attempt to set position out of page bounds.\n");
    }
}

int page_put_int(const char *filename, int val, page_t page) {
    int fd = open_file(filename);
    if (page->current_pos < (page->page_nr * BLOCK_SIZE + BLOCK_SIZE) - sizeof(int)) {
        memcpy(page->data + page->current_pos, (char *)&val, sizeof(int));
    }
    else {
        printf("Page %d is full\n", page->page_nr);
        return 0;
    }

    return 1;
}   

int page_get_int(const char *filename, page_t page) {
    int fd, buf, offset;
    fd = open_file(filename);
    int res = (int) *((int *)((page->data) + page->current_pos));
    
    return res;
}