#include <stdlib.h>
#include <stdio.h>
#include<string.h>

typedef struct str{
    char data;
    struct str* next;
} str;

str* init_str(){
    str* st = (str*)malloc(sizeof(str));
    st->next = NULL;
    return st;
}

str* str_new_elem(str* last, char symb){
    str* nw = (str*)malloc(sizeof(str));
    nw->data = symb;
    nw->next = NULL;
    last->next = nw;
    return nw;
}

void str_print(str *st) {
    str* ptr = st;
    printf("New line:");
    if(!ptr) printf("_\n");
    else{
        while (ptr) {
            printf("%c", ptr->data);
            ptr = ptr->next;
        }
        printf("\n");
    }
}

void str_erase(str *st){
    str* ptr = st->next, *ptr_prev;
    while(ptr) {
        ptr_prev = ptr;
        ptr = ptr->next;
        free(ptr_prev);
    }
    st->next = NULL;
}

void new_str(char* st, str* begin){
    short int opened = 0;
    short int wordflag = 0;
    char s;
    str* data = begin;

    while(1){
        if(*st == '('){
            if(wordflag != 0){
                data = str_new_elem(data, ' ');
            }
            data = str_new_elem(data, *st);
            opened = 1;
            wordflag = 0;
            st++;
            continue;
        }
        else if(*st == ')'){
            data = str_new_elem(data, *st);
            opened = 0;
            st++;
            continue;
        }

        if(opened == 0){
            if(*st == '\0'){
                break;
            }
            else{
                st++;
            }
        }
        else{
            if(*st == ' ' || *st == '\t'){
                if(wordflag == 1)
                    wordflag = -1;
                st++;
            }

            else if(*st == '\0'){
                data = str_new_elem(data, ')');
                break;
            }

            else{
                if(wordflag == -1)
                    data = str_new_elem(data, ' ');
                wordflag = 1;
                data = str_new_elem(data, *st);
                st++;
            }
        }
    }
}

char* get_str(int *n){
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
	else res = calloc(1,sizeof(char));
	return res;
}


int main() {
    str* data = init_str();
    int p = 1;
    char* st;
    char* strb;
    int n;
    printf("Enter lines:\n");
    while(p != -1){
        st=get_str(&n);
        strb = st;
        if(n < 0){
            printf("\nEOF\n");
            new_str(strb, data);
            str_print(data->next);
            str_erase(data);
            break;
        }
        new_str(strb, data);
        str_print(data->next);
        str_erase(data);
        free(st);
    }
    free(data);
    free(st);
    return 0;
}
