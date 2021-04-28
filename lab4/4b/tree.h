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

#define N 10

typedef struct Node{
    List* info;
    struct Node* left; 
    struct Node* right; 
    struct Node* parent;
    char key[N];
    int balance;
}Node;

typedef struct Elem{
    int birth_time;
    int alive;
    Node* node;
}Elem;

#define buf_size 10
#define max_time 5

typedef struct Buf{
    int rsize;
    int rtime;
    Elem elements[buf_size];
}Buf;


Node* insert(char* key, Info info, Node** root, Node* Elist);
Node* find_key(Node* node, char* key, Node* Elist); 
void find_diff(Node* node, char* key, Node* Elist);
void delete(Node** root, char* key, Node* Elist, Buf* bufer);
void delete_all(Node* node, Node* Elist);
void output(Node* root, Node* Elist);
void output_in_range(Node* root, char* k1, char* k2, Node* Elist);
void print_tree(Node* node, int lvl, Node* Elist);
void print_node(Node* node);
void count_time(Node** root, Node* Elist, Buf* bufer);
void get_num(int* x);
char* get_str(int* x);
void show_tree(Node* node, FILE* graph, int* null_numb, Node* Elist);

int find_buf(Buf* bufer, char* key);
void insert_buf(Buf* bufer, Node* node);
void delete_buf(Buf* bufer, char* key);

#endif