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

int tolower(int n){
    if(n >= 65 && n <= 90)
        return n + 32;
    return n;
}
//--------------------------------------------------
int main(){
    Node* Elist = calloc(sizeof(Node), 1);
    Node* head = Elist;
    int n;
    char* name;
    char* if_new;
    int cont = 0;
    int ans;
    int rub;
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

    long int str = 0, str_offset = 0;
    Info info;
    while(1){
        char buf;
        fread(&buf, 1, 1, file);
        if(feof(file))
            break;

        if(buf == '\n'){
            str += 1;
            str_offset = 0;
        }
        else
            ++str_offset;

        buf = tolower(buf);
        if(buf < 97 || buf > 122)
            continue;
        
        info.str = str;
        info.pos = str_offset;
        info.next = NULL;

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

            if(buf == '\n'){
                str += 1;
                str_offset = 0;
                break;
            }
            else
                ++str_offset;

            buf = tolower(buf);
        }
        rsize += 1;
        word = realloc(word, rsize);
        word[rsize - 1] = '\0';
        insert(word, info, &head, Elist);
        free(word);
        if(end == 1){
            break;
        }
    }
    fclose(file);
    print_tree(head, 0, Elist);

    if(head == Elist){
        printf("\nNo words in file\n");
        return 0;
    }
    FILE* graph = fopen("tree.dot", "w");
    fputs("digraph G{ \n", graph);
    fputs("\tgraph [ordering=\"out\"];\n", graph);
    int null_numb = 0;
    //fputs("\tnull0[shape=point];\n", graph);
    show_tree(head, graph, &null_numb, Elist);
    fputs("}", graph);
    fclose(graph);
    system("dot tree.dot | neato -n -Tpng -o tree.png");

    while(1){
        printf("\n\t1 find word\n\t2 to output all\n\t-1 quit\n");
        get_num(&ans);
        if(ans == 1){
            char* key;
            key = get_str(&rub);
            Node* ptr = find_key(head, key, Elist);
            if(ptr != Elist)
                print_node(ptr);
            else
                printf("\nno such element\n");
            free(key);
        }
        else if(ans == 2){
            if(head == Elist)
                printf("\ntree is empty\n");
            else
                output(head, Elist);
        }
        else{
            if(head != Elist)
                delete_all(head, Elist);
            break;
        }
    }
    

    /* while(1){
        printf("#------------------");
        printf("\n\t1 input\n\t2 output\n\t3 find using key\n\t");
        printf("4 fing maxdiff\n\t5 delete\n\t6 print tree\n\t");
        printf("7 write tree to file\n\t8 output in range\n\t-5 count time\n\t0 exit\n");
        printf("\t9 visualise\n");
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
            insert(key, info, &head, Elist);
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
            Node* ptr = find_key(head, key, Elist);
            if(ptr != Elist)
                print_node(ptr);
            else
                printf("\nno such element\n");
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
            delete(&head, key, Elist);
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

        else if(ans == 7){
            if(head == Elist){
                printf("\ntree is empty\n");
                continue;
            }

            int nn;
            char* file_name;
            printf("Enter name of file: ");
            file_name = get_str(&nn);  
            file = fopen(file_name, "w+b");
            free(file_name);
            write_tree(head, file, Elist);
            fclose(file);
        }

        else if(ans == -5){
            if(head == Elist){
                printf("\ntree is empty\n");
                continue;
            }
            float time = count_time(head, Elist);
            printf("avg time %lf\n", time);
        }

        else if(ans == 8){
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


        else if(ans == 9){
            if(head == Elist){
                printf("\ntree is empty\n");
                continue;
            }
            FILE* graph = fopen("tree.dot", "w");
            fputs("digraph G{ \n", graph);
            fputs("\tgraph [ordering=\"out\"];\n", graph);
            int null_numb = 0;
            //fputs("\tnull0[shape=point];\n", graph);
            show_tree(head, graph, &null_numb, Elist);
            fputs("}", graph);
            fclose(graph);
            system("dot tree.dot | neato -n -Tpng -o tree.png");
        }
    } */
    free(Elist);
    return 0;
}   