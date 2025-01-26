// gcc free3.c -o chall
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>

#define MAX_DEPTH (4)

typedef struct Data {
	struct Data *next;
	uint32_t len;
	uint32_t id;
	char buf[];
} data_t;
data_t head = {}, *tail = &head;

__attribute__((constructor))
static int init(){
	srand(time(NULL));

	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	return 0;
}

static int alloc(uint32_t size);
static int edit(uint32_t id);
static int release(uint32_t id);

static int getnline(char *buf, size_t size);
static int getint(void);

int main(void){
	printf("MENU\n"
			"1. Allocate\n"
			"2. Edit\n"
			"3. Free\n"
			"0. Exit\n");

	for(;;){
		printf("> ");
		switch(getint()){
			case 1:
				{
				printf("size: ");
				uint32_t id = alloc(getint());
				if(id)
					printf("ID:0x%08x allocated\n", id);
				}
				break;
			case 2:
				printf("id: ");
				if(edit(getint()))
					puts("Not found");
				break;
			case 3:
				printf("id: ");
				if(release(getint()))
					puts("Not found");
				break;
			default:
				goto end;
		}
	}

end:
	puts("Bye.");
	return 0;
}

static int alloc(uint32_t size){
	if(size < 0x20 || size > 0x400){
		puts("Invalid size");
		return 0;
	}

	data_t *p = malloc(8+size);
	if(!p){
		puts("Allocation error");
		return 0;
	}

	p->len = size;
	p->id = rand();

	tail->next = p;
	tail = p;

	return p->id;
}

static int edit(uint32_t id){
	data_t *p = head.next;
	for(int i = 0; p && i < MAX_DEPTH; p = p->next, i++)
		if(p->id == id){
			printf("data(%u): ", p->len);
			getnline(p->buf, p->len);
			return 0;
		}

	return -1;
}

static int release(uint32_t id){
	for(data_t *p = &head; p->next; p = p->next)
		if(p->next->id == id){
			// free-free is more secure
			if(tail == p->next)
				tail = p;
			p->next = p->next->next;
			return 0;
		}

	return -1;
}

static int getnline(char *buf, size_t size){
	if(size < 1) return -1;

	int len;
	for(len = 0; len < size-1; len++){
		char c;
		if(read(STDIN_FILENO, &c, 1) < 0 || c == '\n')
			break;
		buf[len] = c;
	}
	buf[len] = '\0';

	return len;
}

static int getint(void){
	char buf[0x10] = {};

	getnline(buf, sizeof(buf));
	return strtol(buf, NULL, 0);
}
