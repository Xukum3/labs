#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tree.h"

//--------------------------------------------------
Node* find_min(Node* node){
    while(node->left != NULL)
        node = node->left;
    return node;
}
Node* find_max(Node* node){
    while(node->right != NULL)
        node = node->right;
    return node;
}

Node* find_next(Node* node){
    if(node->right != NULL)
        return find_min(node->right);

    Node* ptr;
    ptr = node->parent;
    while(ptr != NULL && node == ptr->right){
        node = ptr;
        ptr = ptr->parent;
    }
    return ptr;
}

Node* find_prev(Node* node){
    if(node->left != NULL)
        return find_max(node->left);
    Node* ptr;
    ptr = node->parent;
    while(ptr != NULL && node == ptr->left){
        node = ptr;
        ptr = ptr->parent;
    }
    return ptr;
}

Node* find_key(Node* root, char* key){
    Node* ptr = root;
    while(ptr != NULL){
        int st = strcmp(key, ptr->key);
        if(st == 0)
            break;

        if(st < 0)
            ptr = ptr->left;
        else
            ptr = ptr->right;
    }
    return ptr;
}

//--------------------------------------------------
/*void* add_info(Info info, Node* node){
    List* nw = (List*)malloc(sizeof(List));
    nw->next = node->info;
    nw->info.d1 = info.d1;
    nw->info.d2 = info.d2;
    nw->info.s1 = (char*)malloc(strlen(info.s1) + 1);
    memcpy(nw->info.s1, info.s1, strlen(info.s1) + 1);

    node->info = nw;
}*/

void insert(char* key, Node** root){
    if(*root == NULL){
        *root = (Node*)malloc(sizeof(Node));
        (*root)->info.numb = 1;
        (*root)->next = NULL;
        (*root)->prev = NULL;
        (*root)->parent = NULL;
        (*root)->left = NULL;
        (*root)->right = NULL;

        (*root)->key = (char*)malloc(strlen(key) + 1);
        memcpy((*root)->key, key, strlen(key) + 1);
        return;
    }
    Node* ptr = *root;
    Node* par = NULL;
    int st;

    while(ptr != NULL){
        st = strcmp(key, ptr->key);
        if(st == 0){
            ptr->info.numb += 1;
            return;
        }
        par = ptr;
        if(st < 0){
            ptr = ptr->left;
        }
        else{
            ptr = ptr->right;
        }
    }

    ptr = (Node*)malloc(sizeof(Node));
    ptr->info.numb = 1;
    ptr->key = (char*)malloc(strlen(key) + 1);
    memcpy(ptr->key, key, strlen(key) + 1);
    ptr->parent = par;
    ptr->left = NULL;
    ptr->right = NULL;
    if(st < 0){
        par->left = ptr; 
    }
    else{
        par->right = ptr;
    }
    ptr->next = find_next(ptr);
    ptr->prev = find_prev(ptr);

    if(ptr->next != NULL){
        ptr->next->prev = ptr;
    }
    if(ptr->prev != NULL){
        ptr->prev->next = ptr;
    }
}

//--------------------------------------------------
void delete(Node** root, char* key){
    Node* node = find_key(*root, key);
    if(node == NULL){
        printf("\nno such element\n");
        return;
    }

    free(node->key);
    Node* del;
    Node* par;
    Node* p;
    //1 step
    if(node->left == NULL || node->right == NULL)
        del = node;
    else
        del = node->next;

    if(del->prev != NULL)
        del->prev->next = del->next;
    if(del->next != NULL)
        del->next->prev = del->prev;

    //2 step
    if(del->left != NULL)
        p = del->left;
    else
        p = del->right;
    par = del->parent;

    //3 step
    if(p != NULL)
        p->parent = par;
    
    //4 step
    if(par == NULL)
        *root = p;
    
    //5 step
    else{
        if(par->left == del)
            par->left = p;
        else
            par->right = p;
    }

    //6 step
    if(del != node){
        node->key = (char*)malloc(strlen(del->key) + 1);
        memcpy(node->key, del->key, strlen(del->key) + 1);
        node->info.numb = del->info.numb;
    }
    free(del->key);
    free(del);
}

void delete_all(Node* node){
    if(node->left != NULL)
        delete_all(node->left);
    if(node->right != NULL)
        delete_all(node->right);
    
    free(node->key);
    free(node);
}

//--------------------------------------------------
void print_node(Node* node){
    printf("%s %d\n", node->key, node->info.numb);
}

/* void find_diff(Node* root, int key){
    if(root == NULL){
        printf("\ntree is empty\n");
        return;
    }
    Node* min = find_min(root);
    Node* max = find_max(root);
    int mindiff = key - min->key;
    int maxdiff = max->key - key;
    if(mindiff > maxdiff){
        print_node(min);
    }
    else if(mindiff < maxdiff){
        print_node(max);
    }
    else{
        print_node(min);
        if(min != max)
            print_node(max);
    }
} */

void output(Node* root){
    if(root == NULL){
        printf("\ntree is empty\n");
        return;
    }
    Node* ptr = find_min(root);
    while(ptr != NULL){
        print_node(ptr);
        ptr = ptr->next;
    }
}

void output_2(Node* root){
    if(root == NULL){
        printf("\ntree is empty\n");
        return;
    }
    Node* ptr = find_max(root);
    while(ptr != NULL){
        print_node(ptr);
        ptr = ptr->prev;
    }
}

void print_tree(Node* node, int lvl){
    for(int i = 0; i < lvl; i++)
        printf("|  ");
    if(node == NULL)
        printf("|->NULL\n");
    else{
        printf("|->%s\n", node->key);
        if(node->left != NULL || node->right != NULL){
            print_tree(node->left, lvl + 1);
            print_tree(node->right, lvl + 1);
        }
    }
    for(int i = 0; i <= lvl; i++)
        printf("|  ");
    printf("\n");
}


void show_tree(Node* node, FILE* graph, int* null_numb){
    if(node->left != NULL){
        fprintf(graph, "\t\"%s\"->\"%s\";\n", node->key, node->left->key);
        if(node->right != NULL){
            fprintf(graph, "\t\"%s\"->\"%s\";\n", node->key, node->right->key);
            show_tree(node->left, graph, null_numb);
            show_tree(node->right, graph, null_numb);
        }
        else{
            fprintf(graph, "\tnull%d[shape=point];\n", *null_numb);
            fprintf(graph, "\t\"%s\"->\"null%d\";\n", node->key, (*null_numb)++);
            show_tree(node->left, graph, null_numb);
        }
        
    }
    else{
        if(node->right != NULL){
            if(node->left != NULL)
                fprintf(graph, "\t\"%s\"->\"%s\";\n", node->key, node->left->key);
            else{
                fprintf(graph, "\tnull%d[shape=point];\n", *null_numb);
                fprintf(graph, "\t\"%s\"->\"null%d\";\n", node->key, (*null_numb)++);
            }

            fprintf(graph, "\t\"%s\"->\"%s\";\n", node->key, node->right->key);

            if(node->left != NULL){
                show_tree(node->left, graph, null_numb);
                show_tree(node->right, graph, null_numb);
            }
            else
                show_tree(node->right, graph, null_numb);
        }
    }
}

//--------------------------------------------------
/* oid write_tree(Node* node, FILE* file){
    fwrite(&(node->key), sizeof(int), 1, file);
    List* ptr = node->info;
    while(ptr != NULL){
        Info2 info;
        info.d1 = ptr->info.d1;
        info.d2 = ptr->info.d2;
        info.len_s1 = strlen(ptr->info.s1) + 1;
        if(ptr->next == NULL)
            info.fin = 1;
        else
            info.fin = 0;
        fwrite(&info, sizeof(Info2), 1, file);
        fwrite(ptr->info.s1, 1, info.len_s1, file);
        ptr = ptr->next;
    }
    if(node->left != NULL)
        write_tree(node->left, file);
    
    if(node->right != NULL)
        write_tree(node->right, file);
    
}

void read_tree(Node** root, FILE* file){
    fseek(file, 0, SEEK_END);
    rewind(file);
    while(1){
        int key;
        fread(&key, sizeof(int), 1, file);
        if(feof(file))
            break;
        while(1){ 
            Info info;
            Info2 inf2;
            fread(&inf2, sizeof(Info2), 1, file);
            info.d1 = inf2.d1;
            info.d2 = inf2.d2;
            info.s1 = (char*)malloc(inf2.len_s1);
            fread(info.s1, 1, inf2.len_s1, file);
            insert(key, info, root);
            free(info.s1);
            if(inf2.fin == 1)
               break;
        }
    }
    return;
} */

/* double count_time(Node* root){
    int N = 50000;
    int ans;
    double avg = 0;
    clock_t start, finish;
    while(1){
        printf("\n\t1: find min\n\t2: find max\n\t3: find key\n\t4: find next\
                    \n\t5: find prev\n\t-1 exit\n");
        get_num(&ans);
        printf("#------------------\n");

        if(ans == 1){
            for(int i = 0; i < N; i++){
                start = clock();
                find_min(root);
                finish = clock();
                avg += (double)(finish - start);
            }
            return avg/N;
        }

        if(ans == 2){
            for(int i = 0; i < N; i++){
                start = clock();
                find_max(root);
                finish = clock();
                avg += (double)(finish - start);
            }
            return avg/N;
        }

        if(ans == 3){
            printf("Enter a key:\n");
            int key;
            get_str(&key);
            for(int i = 0; i < N; i++){
                start = clock();
                find_key(root, key);
                finish = clock();
                avg += (double)(finish - start);
            }
            return avg/N;
        }
        
        if(ans == 4){
            printf("Enter a key:\n");
            int key;
            get_num(&key);
            Node* node = find_key(root, key);
            if(node == NULL){
                printf("\nNo such element\n");
                continue;
            }
            for(int i = 0; i < N; i++){
                start = clock();
                find_next(node);
                finish = clock();
                avg += (double)(finish - start);
            }
            return avg/N;
        }

        if(ans == 5){
            printf("Enter a key:\n");
            int key;
            get_num(&key);
            Node* node = find_key(root, key);
            if(node == NULL){
                printf("\nNo such element\n");
                continue;
            }
            for(int i = 0; i < N; i++){
                Node* rub;
                start = clock();
                rub = find_prev(node);
                finish = clock();
                avg += (double)(finish - start);
            }
            return avg/N;
        }

        else if(ans == -1){
            return 0;
            break;
        }
    }
} */


/*void push_back(Node* node, int level, queue* q){
    elem* nw = (elem*)malloc(sizeof(elem));
    nw->cur = node;
    nw->next = NULL;
    nw->level = level;

    if(q->size == 0){
        q->front = nw;
    }
    else{
        q->back->next = nw;
    }
    q->back = nw;
}

if(root == NULL){
        printf("\ntree is empty\n");
        return;
    }
    queue q;
    elem* nw = (elem*)malloc(sizeof(elem));

    Node** arr = (Node**)malloc(sizeof(Node*));
    nw->level = 0;
    nw->cur = root;
    nw->next = NULL;
    q.back = nw;
    q.front = nw;
    q.size = 1;
    int curlvl = 0;
    
    elem* cur;
    while(q.size > 0){
        cur = q.front;
        if(cur->level > curlvl){
            printf("\n\n");
            curlvl += 1;
        }

        if(cur->cur == NULL){
            printf("NULL\t");
        }
        else{
            printf("%4d\t", cur->cur->key);
            push_back(cur->cur->left, cur->level + 1, &q);
            push_back(cur->cur->right, cur->level + 1, &q);
            q.size += 2;
        }

        if(cur != NULL)
            q.front = cur->next;
        q.size -= 1;

    } */