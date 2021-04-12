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
        return find_max(node->right);
    Node* ptr;
    ptr = node->parent;
    while(ptr != NULL && node == ptr->left){
        node = ptr;
        ptr = ptr->parent;
    }
    return ptr;
}

Node* find_key(Node* root, int key){
    Node* ptr = root;
    while(ptr != NULL){
        if(ptr->key == key)
            break;

        if(key < ptr->key)
            ptr = ptr->left;
        else
            ptr = ptr->right;
    }
    return ptr;
}

//--------------------------------------------------
void* add_info(Info info, Node* node){
    List* nw = (List*)malloc(sizeof(List));
    nw->next = node->info;
    nw->info.d1 = info.d1;
    nw->info.d2 = info.d2;
    nw->info.s1 = (char*)malloc(strlen(info.s1) + 1);
    memcpy(nw->info.s1, info.s1, strlen(info.s1) + 1);

    node->info = nw;
}

void insert(int key, Info info,  Node** root){
    if(*root == NULL){
        *root = (Node*)malloc(sizeof(Node));
        (*root)->info = NULL;
        add_info(info, *root);
        (*root)->next = NULL;
        (*root)->prev = NULL;
        (*root)->parent = NULL;
        (*root)->left = NULL;
        (*root)->right = NULL;
        (*root)->key = key;
        return;
    }
    Node* ptr = *root;
    Node* par = NULL;

    while(ptr != NULL){
        if(key == ptr->key){
            add_info(info, ptr);
            return;
        }
        par = ptr;
        if(key < ptr->key){
            ptr = ptr->left;
        }
        else{
            ptr = ptr->right;
        }
    }
    ptr = (Node*)malloc(sizeof(Node));
    ptr->info = NULL;
    add_info(info, ptr);
    ptr->key = key;
    ptr->parent = par;
    ptr->left = NULL;
    ptr->right = NULL;
    if(ptr->key < par->key){
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
void delete(Node** root, int key){
    Node* node = find_key(*root, key);
    if(node == NULL){
        printf("\nno such element\n");
        return;
    }
    
    if(node->info->next != NULL){
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
        node->key = del->key;
        node->info = del->info;
    }
    free(del);
}

void delete_all(Node* node){
    if(node->left != NULL)
        delete_all(node->left);
    if(node->right != NULL)
        delete_all(node->right);
    
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
    printf("\n%d:\n", node->key);
    List* lptr = node->info;
    while(lptr != NULL){
        printf("\t%d %d %s\n", lptr->info.d1, lptr->info.d2, lptr->info.s1);
        lptr = lptr->next;
    }
}

void find_diff(Node* root, int key){
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
}

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

void print_tree(Node* node, int lvl){
    for(int i = 0; i < lvl; i++)
        printf("|  ");
    if(node == NULL)
        printf("|->NULL\n");
    else{
        printf("|->%d\n", node->key);
        if(node->left != NULL || node->right != NULL){
            print_tree(node->left, lvl + 1);
            print_tree(node->right, lvl + 1);
        }
    }
    for(int i = 0; i <= lvl; i++)
        printf("|  ");
    printf("\n");
}

//--------------------------------------------------
void write_tree(Node* node, FILE* file){
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
        printf("%d\n", key);
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
}




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