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
    Node* Elist = calloc(sizeof(Node), 1);
    Node* head = Elist;
    Buf bufer;
    bufer.rsize = 0;
    bufer.rtime = 0;
    for(int i = 0; i < buf_size; i++){
        bufer.elements[i].alive = 0;
        bufer.elements[i].node = NULL;
    }

    int n;
    char* name;
    char* if_new;
    int cont = 0;
    int ans;
    int rub;
    FILE* file = NULL;
    while(1){
        printf("#------------------");
        printf("\n\t1 input\n\t2 output\n\t3 find using key\n\t");
        printf("4 fing maxdiff\n\t5 delete\n\t6 print tree\n\t");
        printf("7 output in range\n\t-5 count time\n\t0 exit\n");
        printf("\t8 visualise\n");
        get_num(&ans);
        printf("#------------------\n");

        if(ans == 1){
            int r;
            Info info;
            char* key;
            printf("\ninfo:\n");
            get_num(&(info.d1));
            get_num(&(info.d2));
            info.s1 = get_str(&r);
            printf("\nkey:\n");
            key = get_str(&rub);  
            Node* nw = insert(key, info, &head, Elist);

            bufer.rtime += 1;
            int pos = find_buf(&bufer, key); 
            if(pos >= 0){
                bufer.elements[pos].birth_time = bufer.rtime;
            }
            else{
                insert_buf(&bufer, nw);
            }

            free(info.s1);
            free(key);
        }
        else if(ans == 2){
            if(head == Elist)
                printf("\ntree is empty\n");
            else
                output(head, Elist);
        }
        else if(ans == 3){
            char* key;
            key = get_str(&rub);

            int pos = find_buf(&bufer, key); 
            if(pos >= 0){
                printf("\nElem exist in buf\n");
                bufer.rtime += 1;
                bufer.elements[pos].birth_time = bufer.rtime;
                print_node(bufer.elements[pos].node);
            }

            else{
                Node* ptr = find_key(head, key, Elist);
                if(ptr != Elist){
                    bufer.rtime += 1;
                    insert_buf(&bufer, ptr);

                    print_node(ptr);
                }
                else
                    printf("\nno such element\n");
            }
            free(key);
        }

        else if(ans == 4){
            char* key;
            key = get_str(&rub);
            find_diff(head, key, Elist);
            free(key);
        }

        else if(ans == 5){
            char* key;
            key = get_str(&rub);
            if(head == Elist){
                printf("\ntree is empty\n");
                continue;
            }
            delete_buf(&bufer, key);
            delete(&head, key, Elist, &bufer);
            free(key);
        }

        else if(ans == 0){
            if(head != Elist)
                delete_all(head, Elist);
            break;
        }

        else if(ans == 6){
            if(head == Elist)
                printf("\ntree is empty\n");
            else
                print_tree(head, 0, Elist);
        }

        else if(ans == -5){
            count_time(&head, Elist, &bufer);
        }

        else if(ans == 7){
            if(head == Elist){
                printf("\ntree is empty\n");
                continue;
            }

            char* key1;
            char* key2;
            printf("\nEnter range:\n");
            key1 = get_str(&rub);
            key2 = get_str(&rub);
            output_in_range(head, key1, key2, Elist);
            free(key1);
            free(key2);
        }

        else if(ans == 8){
            if(head == Elist){
                printf("\ntree is empty\n");
                continue;
            }
            FILE* graph = fopen("tree.dot", "w");
            fputs("digraph G{ \n", graph);
            fputs("\tgraph [ordering=\"out\"];\n", graph);
            int null_numb = 0;
            /* fputs("\tnull0[shape=point];\n", graph); */
            show_tree(head, graph, &null_numb, Elist);
            fputs("}", graph);
            fclose(graph);
            system("dot tree.dot | neato -n -Tpng -o tree.png");
        }
    }
    free(Elist);
    return 0;
}   