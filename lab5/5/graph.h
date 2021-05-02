#ifndef GRAPH_H
#define GRAPH_H

typedef struct Edge{
    double dist;
    int vert_pos;
    struct Edge* next;
    //struct Info* prev;
    //double vert_x;
    //double vert_y;
}Edge;

/* typedef struct List{
    Info* first;
    Info* last;
}List; */

typedef struct Vertex{
    int busy;
    Edge* edges;
    int x;
    int y;
}Vertex;

typedef struct Graph{
    int size;
    Vertex* vertexes;
}Graph; 

//------------------------------
typedef struct q_elem{
    int vert;
    struct q_elem* next;
}q_elem;

typedef struct queue{
    q_elem* first;
    q_elem* last;
}queue;

int add_vertex(Graph* G, int x, int y);
int add_edge(Graph* G, int x, int y, int x2, int y2);
void print_graph(Graph G);
void clean(Graph* G);
void delete_edge(Graph* G, int x, int y, int x2, int y2);
void delete_vertex(Graph* G, int x, int y);
void show_tree(Graph* g, FILE* graph);

void BFS(Graph* G, int x, int y, int x2, int y2);

#endif //GRAPH_H