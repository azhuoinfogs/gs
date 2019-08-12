#include <stdio.h>
#include <stdlib.h>
#define MAXSIZE 10
typedef int elem_type;
typedef struct {
	int *elem;
	int length;
	int list_size;
} sqlist;

//init
void init_sqlist(sqlist *list)
{
	list->elem = (int *) malloc(MAXSIZE * (sizeof(elem_type)));
	if (!list->elem){
		printf("malloc list elen failed !\n");
		exit(0);
	}
	memset(list->elem, 0, MAXSIZE * (sizeof(elem_type)));
	list->length = 0;
	list->list_size = MAXSIZE;
}
//insert
void insert_list(sqlist *list, int position, int elem)
{
	elem_type *base, *p, *insert;
	if (!list || (position < 0) || (position > list->length)) {
		printf("error !!!\n");
		exit(0);
	}
	if (list->length >= list->list_size) {
		base = (int *) realloc(list->elem, (list->list_size + MAXSIZE) * sizeof(elem_type));
		list->elem = base;
		list->list_size = (list->list_size + MAXSIZE) * sizeof(elem_type);
	}
	p = &(list->elem[position]);
	for (insert = &(list->elem[list->length - 1]); insert >= p; insert--)
		*(insert + 1) = *insert;
	*p = elem;
	list->length++;
}
//del
void del_list(sqlist *list, int position)
{
	elem_type *base, *del, *end;
	if (!list || (position < 0) || (position > list->length)) {
		printf("error !!!\n");
		exit(0);
	}
	del = &(list->elem[position]);
	end = &(list->elem[list->length - 1]);
	for (;del < end;del++)
		*del = *(del + 1);
	list->length--;
}

elem_type *get_list(sqlist *list, int position)
{
	if (!list || (position < 0) || (position > list->length)) {
		printf("error !!!\n");
		exit(0);
	}
	return list->elem[position];
}
//print
void print_list(sqlist *list)
{
	for (int i = 0; i < list->length; i++) {
		printf("%d\n", list->elem[i]);
	}
}

void destroy_list(sqlist *list)
{
	if (!list) {
		printf("list is null !!!\n");
		exit(0);
	}
	if (list->elem) {
		free(list->elem);
		list->elem = NULL;
	}
	list->length  = 0;
	list->list_size  = 0;
}

int main()
{
	sqlist l;
	int i;
	init_sqlist(&l);
	for (i = 0; i < 20; i++) {
		insert_list(&l, i, i);
	}
	print_list(&l);
	printf("---%d\n",get_list(&l, 2));
	del_list(&l, 2);
	print_list(&l);
	destroy_list(&l);
}

