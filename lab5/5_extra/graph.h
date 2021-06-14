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
    double x;
    double y;
}Vertex;

typedef struct Graph{
    int size;
    Vertex* vertexes;
}Graph;


typedef struct e_Fl{
    int pos1;
    int pos2;
    double dist;
}e_FL;

//------------------------------
typedef struct q_elem{
    int vert;
    struct q_elem* next;
}q_elem;

typedef struct queue{
    q_elem* first;
    q_elem* last;
}queue;


int find_vertex(Graph* G, int x, int y);
int add_vertex(Graph* G, int x, int y);
int add_edge(Graph* G, int x, int y, int x2, int y2);
void print_graph(Graph G);
void clean(Graph* G);
void delete_edge(Graph* G, int x, int y, int x2, int y2);
void delete_vertex(Graph* G, int x, int y);
void show_graph(Graph* g, FILE* graph);

//void BFS(Graph* G, int x, int y, int x2, int y2);
int BFS(Graph* G, int start, int finish, int* way, int* ans);
int Dijkstra(Graph* G, int start, int finish, int* way, double* ans);
void print_way(int start, int vertex, int* way, Graph* G);
void Fl_V(Graph* G, int* next, e_FL* elems);
void path(int* next, int a, int b, Graph* G);

void generate_graph(Graph* G, int v, int e);
void write_graph(Graph* G, FILE* file);
void read_graph(Graph* G, FILE* file);
void count_time(Graph* G);

#endif //GRAPH_H