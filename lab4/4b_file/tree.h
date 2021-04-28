#ifndef TREE_H
#define TREE_H

typedef struct Info{
    long int str;
    long int pos;
    struct Info* next;
}Info;

typedef struct List{
    Info* first;
    Info* last;
}List;

#define N 10

typedef struct Node{
    List* info;
    struct Node* left; 
    struct Node* right; 
    struct Node* parent;
    char* key;
    int balance;
}Node;


void insert(char* key, Info info, Node** root, Node* Elist);
Node* find_key(Node* node, char* key, Node* Elist); 
void find_diff(Node* node, char* key, Node* Elist);
void delete(Node** root, char* key, Node* Elist);
void delete_all(Node* node, Node* Elist);
void output(Node* root, Node* Elist);
void output_in_range(Node* root, char* k1, char* k2, Node* Elist);
void print_tree(Node* node, int lvl, Node* Elist);
void print_node(Node* node);
void write_tree(Node* node, FILE* file, Node* Elist);
void read_tree(Node** node, FILE* file, Node* Elist);
double count_time(Node* root, Node* Elist);
void get_num(int* x);
char* get_str(int* x);
void show_tree(Node* node, FILE* graph, int* null_numb, Node* Elist);

#endif