#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tree.h"

//--------------------------------------------------
int hash(char* str){
    int hs = 0;
    for(int i = 0; i < strlen(str); i++){
        hs = hs^str[i] + hs;
    }
    return abs(hs%buf_size);   
}

void print_buf(Buf* bufer){
    for(int i = 0; i < buf_size; i++){
        if(bufer->elements[i].alive == 0){
            printf("(nil)\n");
            continue;
        }
        printf("%p\n", bufer->elements[i].node);
    }
}

/* void clean_buf(Buf* bufer){
    for(int i = 0; i < buf_size; i++){
        if(bufer->rtime - bufer->elements[i].birth_time > max_time || bufer->elements[i].node == NULL){
            bufer->elements[i].alive = 0;
            bufer->rsize -= 1;
        }
    }
    //print_buf(bufer);
} */


int find_buf(Buf* bufer, char* key){
    int j = hash(key);
    int n = 0;
    while(bufer->elements[j].alive != 0 && n < buf_size){
        if(bufer->elements[j].node != NULL && strcmp(key, bufer->elements[j].node->key) == 0){
            return j;
        }
        j = (j+1)%buf_size;
        n += 1;
    }
    return -1;
}

void insert_buf(Buf* bufer, Node* node){
    /* if(bufer->rsize == buf_size)
        clean_buf(bufer); */

    int j = hash(node->key);
    int n = 0;
    while(bufer->elements[j].alive != 0 && n < buf_size\
    && bufer->rtime - bufer->elements[j].birth_time <= max_time){
        j = (j+1)%buf_size;
        n += 1;
    }
    if(n < buf_size){
        bufer->elements[j].alive = 1;
        bufer->elements[j].birth_time = bufer->rtime;
        bufer->elements[j].node = node;
        bufer->rsize += 1;
        //print_buf(bufer);
    }
}

void delete_buf(Buf* bufer, char* key){
    int j = hash(key);
    int n = 0;
    while(bufer->elements[j].alive != 0 && n < buf_size){
        if(bufer->elements[j].node != NULL && strcmp(key, bufer->elements[j].node->key) == 0){
            bufer->elements[j].alive = 0;
        }
        j = (j+1)%buf_size;
        n += 1;
    }
}



//--------------------------------------------------
Node* find_min(Node* node, Node* Elist){
    while(node->left != Elist)
        node = node->left;
    return node;
}
Node* find_max(Node* node, Node* Elist){
    while(node->right != Elist)
        node = node->right;
    return node;
}

Node* find_next(Node* node, Node* Elist){
    if(node->right != Elist)
        return find_min(node->right, Elist);

    Node* ptr;
    ptr = node->parent;
    while(ptr != Elist && node == ptr->right){
        node = ptr;
        ptr = ptr->parent;
    }
    return ptr;
}

Node* find_prev(Node* node, Node* Elist){
    if(node->left != Elist)
        return find_max(node->left, Elist);
    Node* ptr;
    ptr = node->parent;
    while(ptr != Elist && node == ptr->left){
        node = ptr;
        ptr = ptr->parent;
    }
    return ptr;
}

Node* find_key(Node* root, char* key, Node* Elist){
    Node* ptr = root;
    while(ptr != Elist){
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


void find_diff(Node* root, char* key, Node* Elist){
    if(root == NULL){
        printf("\ntree is empty\n");
        return;
    }
    Node* min = find_min(root, Elist);
    Node* max = find_max(root, Elist);
    if(strcmp(key, root->key) < 0)
        print_node(max);
    else if(strcmp(key, root->key) > 0){
        print_node(min);
    }
    else{
        print_node(min);
        if(min != max)
            print_node(max);
    }
}
//--------------------------------------------------
void left_rotate(Node** root, Node* node, Node* Elist){
    Node* y = node->right;
    Node* par = node->parent;

    //step 1
    node->right = y->left;

    if(y->left != Elist)
        y->left->parent = node;
    
    //step 2
    y->parent = par;
    if(par == Elist){
        *root = y;
        Elist->left = y;
        Elist->right = y;
    }
    else{
        if(par->left == node)
            par->left = y;
        else
            par->right = y;
    }

    //step 3
    y->left = node;
    node->parent = y;
}

void right_rotate(Node** root, Node* node, Node* Elist){
    Node* y = node->left;
    Node* par = node->parent;

    //step 1
    node->left = y->right;

    if(y->right != Elist)
        y->right->parent = node;
    
    //step 2
    y->parent = par;
    if(par == Elist){
        *root = y;
        Elist->left = y;
        Elist->right = y;
    }
    else{
        if(par->right == node)
            par->right = y;
        else
            par->left = y;
    }

    //step 3
    y->right = node;
    node->parent = y;
}


void left_correct(Node* nw, Node* Elist, Node** root){
    if(nw->balance == -1){
        nw->balance = 0;
        nw->parent->balance = 0;
        right_rotate(root, nw->parent, Elist);
    }
    else{
        if(nw->right->balance == -1){
            nw->balance = 0;
            nw->right->balance = 0;
            nw->parent->balance = 1;
            left_rotate(root, nw, Elist);
            right_rotate(root, nw->parent->parent, Elist);
        }
        else if(nw->right->balance == 1)
        {
            nw->balance = -1;
            nw->right->balance = 0;
            nw->parent->balance = 0;
            left_rotate(root, nw, Elist);
            right_rotate(root, nw->parent->parent, Elist);
        }
        else{
            nw->balance = 0;
            nw->right->balance = 0;
            nw->parent->balance = 0;
            left_rotate(root, nw, Elist);
            right_rotate(root, nw->parent->parent, Elist);
        }
    }
}

void right_correct(Node* nw, Node* Elist, Node** root){
    if(nw->balance == 1){
        nw->balance = 0;
        nw->parent->balance = 0;
        left_rotate(root, nw->parent, Elist);
    }
    else{
        if(nw->left->balance == 1){
            nw->balance = 0;
            nw->left->balance = 0;
            nw->parent->balance = -1;
            right_rotate(root, nw, Elist);
            left_rotate(root, nw->parent->parent, Elist);
        }
        else if(nw->left->balance == -1){
            nw->balance = 1;
            nw->left->balance = 0;
            nw->parent->balance = 0;
            right_rotate(root, nw, Elist);
            left_rotate(root, nw->parent->parent, Elist);
        }
        else{
            nw->balance = 0;
            nw->left->balance = 0;
            nw->parent->balance = 0;
            right_rotate(root, nw, Elist);
            left_rotate(root, nw->parent->parent, Elist);
        }
    }
}

void balanse_tree(Node* nw, Node* Elist, Node** root){
    Node* p = nw->parent;
    while(p != Elist && p->balance == 0){
        if(p->left == nw)
            p->balance -= 1;
        else
            p->balance += 1;
        nw = p;
        p = nw->parent;
    }
    if(p == Elist)
        return;
    
    if(p->left == nw && p->balance == 1){
        p->balance = 0;
        return;
    }
    if(p->right == nw && p->balance == -1){
        p->balance = 0;
        return;
    }
    if(p->left == nw)
        left_correct(nw, Elist, root);
    if(p->right == nw)
        right_correct(nw, Elist, root);
    return;
}


void* add_info(Info info, Node* node){
    List* nw = (List*)malloc(sizeof(List));
    nw->next = node->info;
    nw->info.d1 = info.d1;
    nw->info.d2 = info.d2;
    nw->info.s1 = (char*)malloc(strlen(info.s1) + 1);
    memcpy(nw->info.s1, info.s1, strlen(info.s1) + 1);

    node->info = nw;
}

Node* insert(char* key, Info info,  Node** root, Node* Elist){
    if(*root == Elist){
        *root = (Node*)malloc(sizeof(Node));
        (*root)->info = NULL;
        add_info(info, *root);
        (*root)->parent = Elist;
        (*root)->left = Elist;
        (*root)->right = Elist;
        memcpy((*root)->key, key, strlen(key) + 1);
        (*root)->balance = 0;
        Elist->left = *root;
        Elist->right = *root;
        return (*root);
    }
    Node* ptr = *root;
    Node* par = Elist;
    int st;
    while(ptr != Elist){
        st = strcmp(key, ptr->key);
        if(st == 0){
            add_info(info, ptr);
            return ptr;
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
    ptr->info = NULL;
    add_info(info, ptr);
    memcpy(ptr->key, key, strlen(key) + 1);
    ptr->parent = par;
    ptr->left  = Elist;
    ptr->right = Elist;
    ptr->balance = 0;
    if(st < 0){
        par->left = ptr;
    }
    else{
        par->right = ptr;
    }

    balanse_tree(ptr, Elist, root);
    return ptr;
}
//--------------------------------------------------
void del_balanse_tree(Node* x, Node* Elist, Node** root);

void left_del_correct_tree(Node* par, Node* Elist, Node** root){
    if(par->right->balance == 1){
        par->balance = 0;
        par->right->balance = 0;
        left_rotate(root, par, Elist);
        del_balanse_tree(par->parent, Elist, root);
        return;
    }
    else if(par->right->balance == -1){
        if(par->right->left->balance == 1){
            par->right->left->balance = 0;
            par->balance = -1;
            par->right->balance = 0;
        }
        else if(par->right->left->balance == 1){
            par->right->left->balance = 0;
            par->right->balance = 1;
            par->balance = 0;
        }
        else{
            par->right->left->balance = 0;
            par->right->balance = 0;
            par->balance = 0;
        }
        right_rotate(root, par->right, Elist);
        left_rotate(root, par, Elist);
        del_balanse_tree(par->parent, Elist, root);
        return;
    }
    else{
        par->balance = 1;
        par->right->balance = -1;
        left_rotate(root, par, Elist);
        return;
    }
}

void right_del_correct_tree(Node* par, Node* Elist, Node** root){
    if(par->left->balance == -1){
        par->balance = 0;
        par->left->balance = 0;
        right_rotate(root, par, Elist);
        del_balanse_tree(par->parent, Elist, root);
        return;
    }

    else if(par->left->balance == 1){
        if(par->left->right->balance == -1){
            par->left->right->balance = 0;
            par->balance = 1;
            par->left->balance = 0;
        }
        else if(par->left->right->balance == 1){
            par->left->right->balance = 0;
            par->left->balance = -1;
            par->balance = 0;
        }
        else{
            par->left->right->balance = 0;
            par->left->balance = 0;
            par->balance = 0;
        }
        left_rotate(root, par->left, Elist);
        right_rotate(root, par, Elist);
        del_balanse_tree(par->parent, Elist, root);
        return;
    }

    else{
        par->balance = -1;
        par->left->balance = 1;
        right_rotate(root, par, Elist);
        return;
    }
}


void del_balanse_tree(Node* x, Node* Elist, Node** root){
    //printf("balanse");
    Node* par = x->parent;
    while(par != Elist){
        if(par->left != Elist || par->right != Elist){
            if(par->left == x)
                par->balance += 1;
            else /* if(par->right == x) */
                par->balance -= 1;
        }
        else
            par->balance = 0;
        
        if(abs(par->balance) > 1){
            if(par->right == x)
                right_del_correct_tree(par, Elist, root);
            else
                left_del_correct_tree(par, Elist, root);
            return;
        }
        else if(abs(par->balance) == 1){
            return;
        }
        else{
            x = par;
            par = x->parent;
        }
    }
}

void delete(Node** root, char* key, Node* Elist, Buf* bufer){
    Node* node = find_key(*root, key, Elist);
    //printf("%s\n", node->key);
    if(node == Elist){
        printf("\nno such element\n");
        return;
    }
    
    if(node->info->next != NULL){
        //printf("1");
        List* ex = node->info;
        node->info = ex->next;
        free(ex->info.s1);
        free(ex);
        return;
    }

    free(node->info->info.s1);
    free(node->info);
    Node* del;
    Node* par;
    Node* p;
    //1 step
    if(node->left == Elist || node->right == Elist)
        del = node;
    else
        del = find_next(node, Elist);

    //2 step
    if(del->left != Elist)
        p = del->left;
    else
        p = del->right;
    par = del->parent;

    //3 step
    //if(p != Elist)
        p->parent = par;
    
    //4 step
    if(par == Elist)
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
        memcpy(node->key, del->key, strlen(del->key) + 1);
        //node->key = del->key;
        node->info = del->info;
    }

    del_balanse_tree(p, Elist, root);
    free(del);
}

void delete_all(Node* node, Node* Elist){
    if(node->left != Elist)
        delete_all(node->left, Elist);
    if(node->right != Elist)
        delete_all(node->right, Elist);
    
    List* ptr = node->info;
    List* ptr_nw;
    while(ptr != NULL){
        ptr_nw = ptr->next;
        free(ptr->info.s1);
        free(ptr);
        ptr = ptr_nw;
    }
    free(node);
}

//--------------------------------------------------
void print_node(Node* node){
    printf("\n%s:\n", node->key);
    List* lptr = node->info;
    while(lptr != NULL){
        printf("\t%d %d %s\n", lptr->info.d1, lptr->info.d2, lptr->info.s1);
        lptr = lptr->next;
    }
}


void output(Node* node, Node* Elist){
    if(node->left != Elist)
        output(node->left, Elist);
    print_node(node);
    if(node->right != Elist)
        output(node->right, Elist);
}

void print_tree(Node* node, int lvl, Node* Elist){
    for(int i = 0; i < lvl; i++)
        printf("|  ");
    if(node == Elist)
        printf("|->Elist\n");
    else{
        printf("|->%s %d\n", node->key, node->balance);
        if(node->left != Elist || node->right != Elist){
            print_tree(node->right, lvl + 1, Elist);
            print_tree(node->left, lvl + 1, Elist);
        }
    }
    for(int i = 0; i <= lvl; i++)
        printf("|  ");
    printf("\n");
}

void output_in_range(Node* root, char* k1, char* k2, Node* Elist){
    if(strcmp(k1, k2) == 0)
        printf("\nError.Keys are equal\n");
    else if(strcmp(k1, k2) < 0){
        Node* ptr = root;
        Node* find = Elist;
        while(1){
            if(strcmp(k1, ptr->key) < 0){
                find = ptr;
                if(ptr->left != Elist)
                    ptr = ptr->left;
                else
                    break;
            }
            else if(ptr->right != Elist){
                ptr = ptr->right;
            }
            else
                break;
        }
        
        int out = 0;
        ptr = find;

        while(ptr != Elist && strcmp(ptr->key, k2) < 0){
            print_node(ptr); 
            out = 1;
            ptr = find_next(ptr, Elist);
        }
        if(out == 0)
            printf("\nNo elements in this range\n");
    }
    else {
        printf("\nReverse order\n");
        Node* ptr = root;
        Node* find = Elist;
        while(1){
            if(strcmp(k1, ptr->key) > 0){
                find = ptr;
                if(ptr->right != Elist)
                    ptr = ptr->right;
                else
                    break;
            }
            else if(ptr->left != Elist){
                ptr = ptr->left;
            }
            else break;
        }
        
        int out = 0;
        ptr = find;

        while(ptr != Elist && strcmp(ptr->key, k2) > 0){
            print_node(ptr); 
            out = 1;
            ptr = find_prev(ptr, Elist);
        }
        if(out == 0)
            printf("\nNo elements in this range\n");
    }
}

//--------------------------------------------------

void show_tree(Node* node, FILE* graph, int* null_numb, Node* Elist){
    if(node->left != Elist){
        fprintf(graph, "\t\"%s %d\"->\"%s %d\";\n", node->key, node->balance, node->left->key, node->left->balance);
        if(node->right != Elist){
            fprintf(graph, "\t\"%s %d\"->\"%s %d\";\n", node->key, node->balance, node->right->key, node->right->balance);
            show_tree(node->left, graph, null_numb, Elist);
            show_tree(node->right, graph, null_numb, Elist);
        }
        else{
            fprintf(graph, "\tnull%d[shape=point];\n", *null_numb);
            fprintf(graph, "\t\"%s %d\"->\"null%d\";\n", node->key, node->balance, (*null_numb)++);
            show_tree(node->left, graph, null_numb, Elist);
        }
        
    }
    else{
        if(node->right != Elist){
            if(node->left != Elist)
                fprintf(graph, "\t\"%s %d\"->\"%s %d\";\n", node->key, node->balance, node->left->key, node->left->balance);
            else{
                fprintf(graph, "\tnull%d[shape=point];\n", *null_numb);
                fprintf(graph, "\t\"%s %d\"->\"null%d\";\n", node->key, node->balance, (*null_numb)++);
            }

            fprintf(graph, "\t\"%s %d\"->\"%s %d\";\n", node->key, node->balance, node->right->key, node->right->balance);

            if(node->left != Elist){
                show_tree(node->left, graph, null_numb, Elist);
                show_tree(node->right, graph, null_numb, Elist);
            }
            else
                show_tree(node->right, graph, null_numb, Elist);
        }
    }
}

void count_time(Node** root, Node* Elist, Buf* bufer){
    if(*root != Elist)
        delete_all(*root, Elist);
    srand(time(NULL));
    int ans;
    int avgfind, avgins, avgdel;
    clock_t start, finish;
    clock_t start2, finish2;

    int num = 100000;
    int size;
    Info info;
    info.d1 = 1;
    info.d2 = 1;
    info.s1 = (char*)malloc(2);
    info.s1[0] = '1';
    info.s1[1] = '\0';
    char s[5];
    s[4] = '\0';
    for(int i = 0; i < 100000; i++){
        for(int j = 0; j < 4; j++){
            s[j] = rand() % 43 + 48;
        }
        insert(s, info, root, Elist);
    }

    for(int step = 0; step < 7; step++){
        printf("\n\tFor %d elements:\n", num);
        avgfind = 0; avgins = 0; avgdel = 0;
        for(int k = 0; k < 500; k++){
            Node* find;
            for(int j = 0; j < 4; j++){
                s[j] = rand() % 93 + 33;
            }
            start = clock();
            find = find_key(*root, s, Elist);
            finish = clock();
            avgfind += (finish - start);
        }
        printf("\navgfind time: %lf", (double)avgfind / 500);

        for(int k = 0; k < 500; k++){
            for(int j = 0; j < 4; j++){
                s[j] = rand() % 93 + 33;
            }
            start = clock();
            insert(s, info, root, Elist);
            finish = clock();
            avgins += (finish - start);
            start2 = clock();
            delete(root, s, Elist, bufer);
            finish2 = clock();
            avgdel += (finish2 - start2);
        }

        printf("\navgins time: %lf", (double)avgins / 500);
        printf("\navgdel time: %lf", (double)avgdel / 300);


        /* if(num < 30000){
            printf("\ngoing to new test...\n");
            for(int i = 0; i < 10000; i++){
                for(int j = 0; j < 4; j++){
                    s[j] = rand() % 93 + 33;
                }
                insert(s, info, root, Elist);
            }
            printf("successfull! new test...\n");
            num += 10000;
        }
        else if (num < 300000){
            printf("\ngoing to new test...\n");
            for(int i = 0; i < 100000; i++){
                for(int j = 0; j < 4; j++){
                    s[j] = rand() % 93 + 33;
                }
                insert(s, info, root, Elist);
            }
            printf("successfull! new test...\n");
            num += 100000;
        }
        else{*/
            if(step != 6){
                printf("\ngoing to new test...\n");
                for(int i = 0; i < num * 2; i++){
                    for(int j = 0; j < 5; j++){
                        s[j] = rand() % 93 + 33;
                    }
                    insert(s, info, root, Elist);
                }
                printf("successfull! new test...\n");
                num *= 2;
            }
        /*}*/
    }
    free(info.s1);
    //delete_all(*root, Elist);
}