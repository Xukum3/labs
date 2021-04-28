#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"

//--------------------------------------------------
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

void get_num(int* x){
    int p;
    do{
        p = scanf("%d", x);
        if(p <= 0){
            printf("Error. Try again\n");
            scanf("%*[^\n]");
        }
    }while(p <= 0);
    scanf("%*c");
}

/* void get_float(float* x){
    int p;
    do{
        p = scanf("%f", x);
        if(p <= 0){
            printf("Error. Try again\n");
            scanf("%*[^\n]");
        }
    }while(p <= 0);
    scanf("%*c");
} */

int tolower(int n){
    if(n >= 65 && n <= 90)
        return n + 32;
    return n;
}

//--------------------------------------------------
int main(){
    Node* head = NULL;
    int n;
    char* name;
    int cont = 0;
    int ans;
    FILE* file = NULL;

    printf("Enter name of file:\n");
    while(1){
        name = get_str(&n);  
        file = fopen(name, "r");
        free(name);

        if(!file){
		    printf("\nNo such file\n\n");
		    continue;
	    }
        fseek(file, 0, SEEK_END);
	    if(!ftell(file)){    
		    printf("File is empty\n");
            fclose(file);
            return 0;
        }
        break;
    }
    fseek(file, 0, SEEK_SET);


    while(1){
        char buf;
        fread(&buf, 1, 1, file);
        if(feof(file))
            break;

        buf = tolower(buf);
        if(buf <= 97 || buf >= 122)
            continue;
        
        int end = 0;
        int rsize = 0;
        int size = 20;
        char* word = (char*)calloc(1, 20);

        while(buf >= 97 && buf <= 122){
            if(size == rsize){
                word = realloc(word, size + 20);
                size += 20;
            }
            word[rsize] = tolower(buf);
            rsize += 1;

            fread(&buf, 1, 1, file);
            if(feof(file)){
                end = 1;
                break;
            }
            buf = tolower(buf);
        }
        rsize += 1;
        word = realloc(word, rsize);
        word[rsize - 1] = '\0';
        insert(word, &head);
        free(word);
        if(end == 1){
            break;
        }
    }
    fclose(file);
    print_tree(head, 0);
    FILE* graph = fopen("tree.dot", "w");
    fputs("digraph G{ \n", graph);
    fputs("\tgraph [ordering=\"out\"];\n", graph);
    int null_numb = 0;
    /* fputs("\tnull0[shape=point];\n", graph); */
    show_tree(head, graph, &null_numb);
    fputs("}", graph);
    fclose(graph);
    system("dot tree.dot | neato -n -Tpng -o tree.png");
    printf("\n");
    output(head);
    printf("\n");
    delete_all(head);
    return 0;
}   