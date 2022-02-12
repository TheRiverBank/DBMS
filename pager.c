#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pager.h"

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
    /* Reads blk_num from disk and creates a page with the block data */
    int fd = open_file(filename);
    page_t page = malloc(sizeof(page_t));

    page->data = malloc(BLOCK_SIZE);
    page->page_nr = blk_num;
    page->current_pos = HEADER_SIZE;

    if (fd == -1) {
        printf("read_page() error. Faile to get file.");
    }

    if (lseek(fd, BLOCK_SIZE * page->page_nr, SEEK_SET) < 0) {
        printf("read_page() error. Page does not exist.\n");
    }

    if (read(fd, (int *)page->data, BLOCK_SIZE) < 0) {
        printf("read_page() error. Failed to read page.\n");
    }   

    // Read in last byte written to the page from header
    lseek(fd, PG_LAST_WRITTEN_BYTE + (page->page_nr * BLOCK_SIZE), SEEK_SET);
    read(fd, (int *)&page->last_used_byte, INT_SIZE);

    return page;
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

     // Write header stuff to disk
    lseek(fd, PG_LAST_WRITTEN_BYTE + (page->page_nr * BLOCK_SIZE), SEEK_SET);
    write(fd, (char *)&page->last_used_byte, INT_SIZE);
}

int page_set_current_pos(int pos, page_t page) {
    /* Sets the current position in the page */
    if (pos < HEADER_SIZE) {
        printf("invalid pos placement, pos placed at %d\n", pos);
        return -1;
    }
    page->current_pos = pos;
    return 1;
}

int page_current_pos(page_t page) {
    return page->current_pos;
}

int page_last_written_byte(page_t page) {
    return page->last_used_byte;
}

int page_put_int(int val, page_t page) {
    if (page->current_pos + sizeof(int) < BLOCK_SIZE) {
        memcpy(page->data + page->current_pos, (char *)&val, sizeof(int));
    }
    else {
        printf("Page %d is full\n", page->page_nr);
        return 0;
    }
    
    page->last_used_byte = page->current_pos;
    page->current_pos += INT_SIZE;
    
    return 1;
}   

int page_get_int(page_t page) {
    /* Retrieves an integer at the current position. 
     * The current position is set forward by 4 bytes. 
     */
    int res = (int) *((int *)((page->data) + page->current_pos));
    page->current_pos += INT_SIZE;
    return res;
}

int page_get_int_at(page_t page, int pos) {
      /* Retrieves an integer at the specified position. 
     * The current position is not incremented. 
     */
    if (pos > BLOCK_SIZE) {
        printf("FAILURE, get beyond page size\n");
        return -1;
    }

    int res = (int) *((int *)((page->data) + pos));

    return res;
}

int page_set_pos_beg(page_t page) {
    /* Sets the current position in the page back to 0 */
    page->current_pos = HEADER_SIZE;
}

int get_num_blocks(char *f_name) {
     int n_blocks;
    // Get number of bytes in file
    struct stat buf;
    stat(f_name, &buf);
    off_t size = buf.st_size;
    // Get number of written blocks
    n_blocks = size / BLOCK_SIZE;

    return n_blocks;
}