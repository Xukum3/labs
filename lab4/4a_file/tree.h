#ifndef TREE_H
#define TREE_H

typedef struct Info{
    int numb;
}Info;

typedef struct Info2{
    int d1;
    int d2;
    int len_s1;
    int fin;
}Info2;

/* typedef struct List{
    Info info;
    struct List* next;
}List; */

typedef struct Node{
    Info info;
    struct Node* left; 
    struct Node* right; 
    struct Node* parent;
    struct Node* next;
    struct Node* prev;

    char* key;
}Node;


void insert(char* key, Node** root);
Node* find_min(Node* node);
Node* find_max(Node* node);
Node* find_next(Node* node);
Node* find_prev(Node* node);
Node* find_key(Node* node, char* key);
/* void find_diff(Node* node, char* key); */
void delete(Node** root, char* key);
void delete_all(Node* node);
void output(Node* root);
void output_2(Node* root);
void print_tree(Node* node, int lvl);
/* void write_tree(Node* node, FILE* file);
void read_tree(Node** node, FILE* file); */
double count_time(Node* root);
void get_num(int* x);
void show_tree(Node* node, FILE* graph, int* null_numb);

#endif