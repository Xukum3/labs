#ifndef TREE_H
#define TREE_H

typedef struct Info{
    int d1;
    int d2;
    char* s1;
}Info;

typedef struct Info2{
    int d1;
    int d2;
    int len_s1;
    int fin;
}Info2;

typedef struct List{
    Info info;
    struct List* next;
}List;

typedef struct Node{
    List* info;
    struct Node* left; 
    struct Node* right; 
    struct Node* parent;
    struct Node* next;
    struct Node* prev;

    unsigned int key;
}Node;

/*typedef struct queue{
    struct elem* front;
    struct elem* back;
    int size;
}queue;

typedef struct elem{
    Node* cur;
    struct elem* next;
    int level;
}elem;*/

void insert(int key, Info info, Node** root);
Node* find_min(Node* node);
Node* find_max(Node* node);
Node* find_next(Node* node);
Node* find_prev(Node* node);
Node* find_key(Node* node, int key);
void find_diff(Node* node, int key);
void delete(Node** root, int key);
void delete_all(Node* node);
void output(Node* root);
void print_tree(Node* node, int lvl);
void write_tree(Node* node, FILE* file);
void read_tree(Node** node, FILE* file);
double count_time(Node* root);
void get_num(int* x);

#endif