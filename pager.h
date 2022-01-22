struct page {
    char *data;
    int page_nr;
    int current_pos;
    int last_used_byte;
};
typedef struct page *page_t;



#define BLOCK_SIZE 512
#define INT_SIZE 4
#define HEADER_SIZE 24

#define PG_LAST_WRITTEN_BYTE 1

enum {CLEAN, USED};
enum {FALSE, TRUE};

int open_file(const char *filename);
int close_file(int fd);
page_t get_page(const char *filename, int blk_num);
int write_page(const char *filename, page_t page);


int page_set_current_pos(int pos, page_t page);
int page_put_int(int val, page_t page);
int page_get_int(page_t page);
int page_get_int_at(page_t page, int pos);
int page_set_pos_beg(page_t page);