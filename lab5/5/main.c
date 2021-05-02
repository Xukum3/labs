#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "graph.h"

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

void get_double(double* x){
    int p;
    do{
        p = scanf("%lf", x);
        if(p <= 0){
            printf("Error. Try again\n");
            scanf("%*[^\n]");
        }
    }while(p <= 0);
    scanf("%*c");
}

int main(){
    Graph G;
    G.size = 0;
    G.vertexes = NULL;

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
            printf("Enter name of file: ");
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

                printf("Do you want to generate graph[y][n]?\n");
                while(1){
                    if_new = get_str(&n);
                    if(strcmp(if_new, "y") == 0){
                        //generate_graph(&G, size);
                        free(if_new);
                        break;
                    }
                    else if(strcmp(if_new, "n") == 0){
                        free(if_new);
                        break;
                    }
                }

                fclose(file);
                break;
            }
            //read_graph(&head, file);
            fclose(file);
            break;
        }
        else if(strcmp(if_new, "n") == 0){
            free(if_new);

            printf("Do you want to generate graph[y][n]?\n");
            while(1){
                if_new = get_str(&n);
                if(strcmp(if_new, "y") == 0){
                    //generate_graph(&G, size);
                    free(if_new);
                    break;
                }
                else if(strcmp(if_new, "n") == 0){
                    free(if_new);
                    break;
                }
            }

            break;
        }
    }

    while(1){
        printf("#------------------");
        printf("\n\t1 Add vertex\n\t2 Add edge\n\t3 Delete vertex\n\t");
        printf("4 Delete edge\n\t5 Print graph\n\t6 BFS\n\t");
        printf("7 Dijkstra\n\t8 \"3 shortest routs\" \n\t9 Count time\n\t");
        printf("10 Visualise (Graphviz)\n\t0 exit (free all, write to file)\n");
        get_num(&ans);
        printf("#------------------\n");
        if(ans == 1){
            int r;
            int x, y;
            get_num(&x);
            get_num(&y);

            r = add_vertex(&G, x, y);
            if(r < 0)
                printf("\nError. This vertex exists\n");
            
            printf("\n\n%d\n", r);
        }
        else if(ans == 2){
            int r;
            int x, y, x2, y2;
            get_num(&x);
            get_num(&y);

            get_num(&x2);
            get_num(&y2);

            r = add_edge(&G, x, y, x2, y2);
            if(r < 0)
                printf("\nError. This edge exists\n");
        }
        else if(ans == 5){
            print_graph(G);
        }/*
        else if(ans == 3){
            int key;
            get_num(&key);
            Node* ptr = find_key(head, key);
            if(ptr != NULL)
                printf("\n%d\n", ptr->key);
            else
                printf("\nno such element\n");
        }
*/      else if(ans == 3){
            int r;
            int x, y;
            get_num(&x);
            get_num(&y);

            delete_vertex(&G, x, y);
        }
        
        else if(ans == 4){
            int x, y, x2, y2;
            get_num(&x);
            get_num(&y);

            get_num(&x2);
            get_num(&y2);

            delete_edge(&G, x, y, x2, y2);
        }

        else if(ans == 6){
            int x, y, x2, y2;
            get_num(&x);
            get_num(&y);

            get_num(&x2);
            get_num(&y2);

            BFS(&G, x, y, x2, y2);
        }
/*
        else if(ans == 5){
            int key;
            get_num(&key);
            delete(&head, key);
        }*/

        else if(ans == 0){
            clean(&G);
            break;
        }
/*
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
        }*/

        else if(ans == 10){
            if(G.size == 0){
                printf("\nGraph is empty\n");
                continue;
            }
            FILE* graph = fopen("graph.dot", "w");
            fputs("graph G{ \n", graph);
            /* fputs("\tnull0[shape=point];\n", graph); */
            show_graph(&G, graph);
            fputs("}", graph);
            fclose(graph);
            system("dot tree.dot | neato -n -Tpng -o tree.png");
        }
    }

    return 0;
}