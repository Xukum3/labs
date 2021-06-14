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

void get_float(float* x){
    int p;
    do{
        p = scanf("%f", x);
        if(p <= 0){
            printf("Error. Try again\n");
            scanf("%*[^\n]");
        }
    }while(p <= 0);
    scanf("%*c");
}

//--------------------------------------------------
int main(){
    Node* head = NULL;
    int n;
    char* name;
    char* if_new;
    int cont = 0;
    int ans;
    FILE* file = NULL;

    printf("Do you want to use file[y][n]?\n");
    while(1){
        if_new = get_str(&n);
        if(strcmp(if_new, "y") == 0){
            printf("Enter name of tree: ");
            name = get_str(&n);  
            file = fopen(name, "rb");
            free(name);
            free(if_new);

            if(!file){
		        printf("\nNo such file\n");
		        break;
	        }
	        fseek(file, 0, SEEK_END);
	        if(!ftell(file)){    
		        printf("File is empty\n");
                fclose(file);
                break;
            }
            read_tree(&head, file);
            fclose(file);
            break;
        }
        else if(strcmp(if_new, "n") == 0){
            free(if_new);
            break;
        }
    }

    while(1){
        printf("#------------------");
        printf("\n\t1 input\n\t2 output\n\t3 find using key\n\t");
        printf("4 fing maxdiff\n\t5 delete\n\t6 print tree\n\t");
        printf("7 write tree to file\n\t8 graphviz\n\t-5 count time\n\t0 exit\n");
        get_num(&ans);
        printf("#------------------\n");
        if(ans == 1){
            int r;
            Info info;
            int key;
            printf("\ninfo:\n");
            get_num(&(info.d1));
            get_num(&(info.d2));
            info.s1 = get_str(&r);
            printf("\nkey:\n");
            get_num(&key);
            insert(key, info, &head);
            free(info.s1);
        }
        else if(ans == 2){
            output(head);
        }
        else if(ans == 3){
            int key;
            get_num(&key);
            Node* ptr = find_key(head, key);
            if(ptr != NULL)
                printf("\n%d\n", ptr->key);
            else
                printf("\nno such element\n");
        }

        else if(ans == 4){
            int key;
            get_num(&key);
            find_diff(head, key);
        }

        else if(ans == 5){
            int key;
            get_num(&key);
            delete(&head, key);
        }

        else if(ans == 100){
            int key;
            get_num(&key);
            printf("%d", f_min(head, key));
        }

        else if(ans == 0){
            if(head != NULL)
                delete_all(head);
            break;
        }

        else if(ans == 6){
            if(head == NULL)
                printf("\ntree is empty\n");
            else
                print_tree(head, 0);
        }

        else if(ans == 7){
            if(head == NULL){
                printf("\ntree is empty\n");
                continue;
            }

            int nn;
            char* file_name;
            printf("Enter name of file: ");
            file_name = get_str(&nn);  
            file = fopen(file_name, "w+b");
            free(file_name);
            write_tree(head, file);
            fclose(file);
        }

        else if(ans == -5){
            if(head == NULL){
                printf("\ntree is empty\n");
                continue;
            }
            float time = count_time(head);
            printf("avg time %lf\n", time);
        }

        else if(ans == 8){
            if(head == NULL){
                printf("\ntree is empty\n");
                continue;
            }
            
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
        }
    }
    return 0;
}   