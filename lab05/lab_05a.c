#include <stdlib.h>
#include <stdio.h>
#include<string.h>

typedef struct Item{
    char* data;
    struct Item *next;
} Item;

typedef struct {
    Item *head;
    Item *tail;
} List;

List* init_list() {
    List* l = (List*)malloc(sizeof(List));
    l->head = NULL;
    l->tail = NULL;
}

void list_print(const List *list) {
    Item *ptr = list->head;
    if(!ptr) printf("\nList is empty\n");
    else{
        printf("\nList:\n");
        while (ptr) {
            printf("%s\n", ptr->data);
            ptr = ptr->next;
        }
    }
}

void list_push_back(List *list, char* data) {
    Item *ptr = (Item *) malloc(sizeof(Item));
    ptr->data = (char*)malloc((1+strlen(data))*sizeof(char));
    memcpy(ptr->data, data, strlen(data));
    ptr->next = NULL;
    if (!list->head) {
        list->head = ptr;
        list->tail = ptr;
    } else {
        list->tail->next = ptr;
        list->tail = ptr;
    }
}

void list_delete(List *list) {
    Item *ptr = list->head, *ptr_prev;
    while (ptr) {
        ptr_prev = ptr;
        ptr = ptr->next;
        free(ptr_prev);
    }
    free(list);
}
//---------------------------------------------------------------------

char* new_str(char* str, char* data, char* buf, char* tek, short int* wordflag, int* datasize, List* l){
     while(1){
        if(*str == ' ' || *str == '\t'){
            if(*wordflag == 1) *wordflag = -1;
            str++;
        }
        else if(*str == ';' || *str == ','){
            *tek = *str;
            str++;
            if(tek == buf + 79){
                    int str_len = *datasize + 79;
                    data = realloc(data, str_len + 1);
                    memcpy(data + *datasize, buf, 79);
                    *datasize = str_len;
                    tek = buf;
            }
            else{
                tek++;
            }
            *wordflag = -1;
            continue;
        }


        else if(*str == '.'){
            *wordflag = 0;
            *tek = *str;
            str++;
            *tek++;
            int chunk_len = tek - buf;
            int str_len = *datasize + chunk_len;
            data = realloc(data, str_len + 1);
            memcpy(data + *datasize, buf, chunk_len);
            *(data + str_len) = '\0';
            *datasize = 0;
            tek = buf;

            list_push_back(l, data);
            free(data);
            data = NULL;
        }

        else if(*str == '\0'){
            if(*wordflag == 1) *wordflag = -1;
            return tek;
        }

        else{
            if(*wordflag == -1){
                *tek = ' ';
                if(tek == buf + 79){
                    int str_len = *datasize + 80;
                    data = realloc(data, str_len + 1);
                    memcpy(data + *datasize, buf, 80);
                    *datasize = str_len;
                    tek = buf;
                }
                else{
                    tek++;
                }
            }
            *wordflag = 1;
            *tek = *str;
            str++;
            if(tek == buf + 79){
                    int str_len = *datasize + 80;
                    data = realloc(data, str_len + 1);
                    memcpy(data + *datasize, buf, 80);
                    *datasize = str_len;
                    tek = buf;
            }
            else{
                tek++;
            }
        }
    }
}

//------------------------------------------------------------------------


char* get_str(int *n, char* data){
	char buf[80];
	char* res = NULL;
	int len = 0;
	do{
		*n = scanf("%80[^\n]", buf);
    if(*n < 0){
      if(!res){
        res = realloc(res, len + 2);
        res[len] = ' ';
        len += 1;
      }
    }
    else if(*n > 0){
			int chunk_len = strlen(buf);
			int str_len = len + chunk_len;
			res = realloc(res,str_len + 1);
			memcpy(res + len, buf, chunk_len);
			len = str_len;
		}
		else{
			scanf("%*c");
		}
	}while(*n > 0);

	if(len > 0) res[len] = '\0';
	else res = (char*)malloc(sizeof(char));
	return res;
}


int main() {
    List* l = init_list();
    int p = 1;
    char* str;
    char* data = NULL;
    char buf[81];
    char* tek = buf;
    short int wordflag = 0;
    int datasize = 0;
    int n;
    printf("Enter lines:\n");
    while(p != -1){
        str=get_str(&n, data);
        char* strb = str;

        if(n < 0){
          tek = new_str(strb, data, buf, tek, &wordflag, &datasize, l);

          if(datasize > 0 || tek - buf > 0){
            int chunk_len = tek - buf;
            int str_len = datasize + chunk_len;
            data = realloc(data, str_len + 1);
            memcpy(data + datasize, buf, chunk_len);
            data[str_len] = '\0';
            list_push_back(l, data);
          }
          printf("\n\nEOF\n");
          break;
        }
        tek = new_str(strb, data, buf, tek, &wordflag, &datasize, l);

        free(str);
    }
    list_print(l);
    list_delete(l);
    free(data);
    free(str);
    return 0;
}
