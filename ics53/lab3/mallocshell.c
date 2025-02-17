/* $begin shellmain */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csapp.h"
#include "mm.h"

#define MAXARGS   128

typedef enum { ALLOC, FREE, BLOCKLIST, HWRITE, HREAD, BESTFIT, FIRSTFIT, NA } options_t;
typedef struct memcmd_s {
	char *raw_command;
	char **args;
	int argc;
	options_t command;
} memcmd_t;

typedef struct heapblock_s heapblock_t;
struct heapblock_s {
	int id, size, allocated;
	char *bp;
	heapblock_t *next;
};

/* function prototypes */
memcmd_t init_command();
void eval(char *cmdline, int *id, heapblock_t *list);
int parseline(char *buf, char **argv, memcmd_t*);
void operate(memcmd_t metadata, int *id, heapblock_t *data);
void allocate(int size, int *id, heapblock_t *last); /* Allocates ${size} blocks */
int freeblock(int id, heapblock_t *list); /* Frees the memory at id ${id} */
void blocklist(); /* Will print all blocks. Uses HDRP(), FTRP(), NEXTBLK() */
int writeheap(int id, char to_write, int n, heapblock_t *data); /* Will write ${data} ${n} times at mem pos ${id} */
void printlist(int id, int n, heapblock_t *list); /* Will print data at mem pos ${id} ${n} times */
int switchfit(int type); /* Will switch memory location algorithm. 0 is firstfit, 1 is bestfit */ 
static void *best_fit(size_t asize); /* Implementation of the best fit memory management algorithm */

memcmd_t init_command() {
	memcmd_t to_return;
	to_return.raw_command = NULL;
	to_return.args = NULL;
	to_return.argc = 0;
	to_return.command = NA;
	return to_return;
}

int main() {
    char cmdline[MAXLINE]; /* Command line */
    mem_init(); /* initialize our memory lib */
    int id = 1;
    heapblock_t *list = malloc(sizeof(heapblock_t));
    list->id = list->size = list->allocated = 0;
    list->bp = NULL;
    list->next = NULL;
    while (1) {
		/* Read */
		printf("> ");
		Fgets(cmdline, MAXLINE, stdin);
		if (feof(stdin))
			exit(0);

		/* Evaluate */
		eval(cmdline, &id, list);
    } 
}
/* $end shellmain */

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline, int *id, heapblock_t *list) {
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    
    memcmd_t cmdobj = init_command();
    cmdobj.raw_command = cmdline;
    cmdobj.args = argv;

    strcpy(buf, cmdline);
    if (!parseline(buf, argv, &cmdobj)) { return; } // Don't do anything.
    operate(cmdobj, id, list);

    return;
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv, memcmd_t *rpath) {
	char *delim;         /* Points to first space delimiter */
	int argc;            /* Number of args */
	int valid = 1; 		 /* Return quit */

	buf[strlen(buf) - 1] = ' ';  /* Replace trailing '\n' with space */
	while (*buf && (*buf == ' ')) /* Ignore leading spaces */
		buf++;

	/* Build the argv list */
	argc = 0;
	while ((delim = strchr(buf, ' '))) {
		/* Replace the current index with the null terminator */
		/* so strcmp stops at it. */
		*delim = '\0';

		if (!strcmp(buf, "allocate")) { rpath->command = ALLOC; }
		else if (!strcmp(buf, "free")) { rpath->command = FREE; }
		else if (!strcmp(buf, "blocklist")) { rpath->command = BLOCKLIST; }
		else if (!strcmp(buf, "writeheap")) { rpath->command = HWRITE; }
		else if (!strcmp(buf, "printheap")) { rpath->command = HREAD; }
		else if (!strcmp(buf, "bestfit")) { rpath->command = BESTFIT; }
		else if (!strcmp(buf, "firstfit")) { rpath->command = FIRSTFIT; }
		else if (!strcmp(buf, "quit")) { printf("Quiting\n"); exit(0); }
		else {
			if (rpath->command != NA) { /* Argument */
				rpath->args[rpath->argc] = malloc(sizeof(char));
				strcpy(rpath->args[rpath->argc++], buf);
			}
			else /* enter some error state. */
				valid = 0;
		}

		buf = delim + 1; /* progression step, next character. */
		while (*buf && (*buf == ' ')) /* Ignore spaces */
			buf++;
	}
	rpath->args[rpath->argc] = NULL;
	return valid;
}

void operate(memcmd_t metadata, int *id, heapblock_t *data) {
	heapblock_t *last = data;
	switch(metadata.command) {
		case ALLOC:			
			while(last->next != NULL) last = last->next;
			if (metadata.argc != 1) printf("Bad arguments. Allocate needs 1 argument.\n");
			else allocate(atoi(metadata.args[0]), id, last);
			break;
		case FREE:
			if (metadata.argc != 1) printf("Bad arguments. Free needs 1 argument.\n");
			else freeblock(atoi(metadata.args[0]), data);
			break;
		case BLOCKLIST:
			if (metadata.argc != 0) printf("Bad arguments. Blocklist needs 0 arguments.\n");
			else blocklist(data);
			break;
		case HWRITE:
			if (metadata.argc != 3) printf("Bad arguments. Write block needs 3 arguments.\n");
			else writeheap(atoi(metadata.args[0]), *metadata.args[1], atoi(metadata.args[2]), data);
			break;
		case HREAD:
			if (metadata.argc != 2) printf("Bad arguments. Printheap requires 2 arguments.\n");
			else printlist(atoi(metadata.args[0]), atoi(metadata.args[1]), data);
			break;
		case BESTFIT:
			if (metadata.argc != 0) printf("Bad arguments. Best fit requries 0 arguments\n");
			else switchfit(1);
			break;
		case FIRSTFIT:
			if (metadata.argc != 0) printf("Bad arguments. First fit requries 0 arguments\n");
			else switchfit(2);
			break;
	}
}

void allocate(int size, int *id, heapblock_t *last) {
	heapblock_t *hb = malloc(sizeof(heapblock_t));
	hb->size = size;
	hb->allocated = 0;
	hb->id = (*id)++;
	hb->bp = (char*)mm_malloc(size);
	hb->next = NULL;
	last->next = hb;
	printf("%d\n", hb->id);
}

int freeblock(int id, heapblock_t *data) {
	/* find the block we want and free using mm_free */ 
	/* and delete the linked list entry. */
	if (data->next == NULL) {
		printf("No blocks have been allocated\n");
		return 0;
	}
	heapblock_t *first = data, *to_delete = data->next;
	while (to_delete != NULL) {
		if (to_delete->id == id) {
			mm_free(to_delete->bp);
			first->next = to_delete->next;
			free (to_delete);
			return 1;
		}
		first = first->next, to_delete = to_delete->next;
	}
}

void blocklist() {
	mm_blocklist();
}

int writeheap(int id, char to_write, int size, heapblock_t *data) {
	heapblock_t *finder = data->next;
	size_t chunksize = sizeof(to_write);
	int writes = 0, offset = 0;
	
	/* find our block and write to it */
	while (finder != NULL && finder->id != id) { finder = finder->next;}
	if (finder == NULL) return 0;
	for (; writes < size && offset < finder->size; writes++) {
		*(finder->bp + offset) = to_write;
		offset += chunksize;
	}
	finder->allocated = 1;
	return 1;
}

void printlist(int id, int n, heapblock_t *list) {
	/* find our block */	
	int i = 0;
	heapblock_t *finder = list->next;
	while (finder != NULL && finder->id != id) { finder = finder->next; }
	for (; i < n; i++) {
		printf("%c", *(finder->bp + (sizeof(char) * i)));
	}
	printf("\n");
}

/* implemented function in mm.c
easier to implement there because
of access to mm.c's macros" */

int switchfit(int type){
	mm_set_fit_mode(type);
	return 0;
}
