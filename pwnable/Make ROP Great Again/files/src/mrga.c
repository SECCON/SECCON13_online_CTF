// gcc mrga.c -fno-stack-protector -fno-pic -no-pie -Wl,-z,now -o chall
#include <stdio.h>

void show_prompt(void);

__attribute__((constructor))
static int init(){
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);
	return 0;
}

int main(void){
	char buf[0x10];

	show_prompt();
	gets(buf);

	return 0;
}

void show_prompt(void){
	puts(">");
}
