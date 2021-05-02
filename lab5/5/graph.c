#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "graph.h"

//------------Graph-----------------------------

int find_vertex(Graph* G, int x, int y){
    int poss_place = -1;
    for(int i = 0; i < G->size; i++){
        if(G->vertexes[i].busy == 0)
            poss_place = i;
        if(G->vertexes[i].busy != 0 && G->vertexes[i].x == x && G->vertexes[i].y == y){
            return -i-1;
        }
    }
    if(poss_place != -1){
        return poss_place;
    }
    return G->size;
}

int add_vertex(Graph* G, int x, int y){
    int place = find_vertex(G, x, y);
    if(place < 0)
        return -place-1;
    else{
        if(place == G->size){
            G->size += 1;
            G->vertexes = (Vertex*)realloc(G->vertexes, (G->size)*sizeof(Vertex));
        }
        G->vertexes[place].busy = 1;
        G->vertexes[place].x = x;
        G->vertexes[place].y = y;
        G->vertexes[place].edges = NULL;
    }
    return place;
}

double dist(int a, int b, int x, int y){
    return pow(((x-a)*(x-a)+(y-b)*(y-b)), 1/2);
}

 
int add_edge(Graph* G, int x, int y, int x2, int y2){
    int pos1, pos2;
    pos1 = add_vertex(G, x, y);
    pos2 = add_vertex(G, x2, y2);
    if(pos1 < 0) pos1 = -pos1 - 1;
    if(pos2 < 0) pos2 = -pos2 - 1;

    Edge* e = G->vertexes[pos1].edges;
    while(e != NULL){
        if(e->vert_pos == pos2){
            return -1;
        }
        e = e->next;
    }
    Edge* tmp = G->vertexes[pos1].edges;
    G->vertexes[pos1].edges = (Edge*)malloc(sizeof(Edge));
    G->vertexes[pos1].edges->next = tmp;
    G->vertexes[pos1].edges->vert_pos = pos2;
    G->vertexes[pos1].edges->dist = dist(x, y, x2, y2);

    if(pos1 != pos2){
        tmp = G->vertexes[pos2].edges;
        G->vertexes[pos2].edges = (Edge*)malloc(sizeof(Edge));
        G->vertexes[pos2].edges->next = tmp;
        G->vertexes[pos2].edges->vert_pos = pos1;
        G->vertexes[pos2].edges->dist = dist(x, y, x2, y2);
    }

    return 0;
} 


void print_graph(Graph G){
    if(G.size == 0){
        printf("\nGraph is empty\n");
        return;
    }
    for(int i = 0; i < G.size; i++){
        if(G.vertexes[i].busy != 0){
            printf("\n%d %d\n", G.vertexes[i].x, G.vertexes[i].y);
            Edge* e = G.vertexes[i].edges;
            if(e == NULL){
                printf("\tVertex is isolated\n");
            }
            while(e != NULL){
                printf("\t%d -> %d\n", i, e->vert_pos);
                e = e->next;
            }
        }
        else{
            printf("\nNULL\n");
        }
    }
}

void delete_edge(Graph* G, int x, int y, int x2, int y2){
    int pos1 = find_vertex(G, x, y);
    int pos2 = find_vertex(G, x2, y2);
    if(pos1 >= 0 || pos2 >= 0){
        printf("\nNo such edge\n");
        return;
    }
    pos1 = -pos1 - 1;
    pos2 = -pos2 - 1;

    Edge* e = G->vertexes[pos1].edges;
    if(e == NULL){
        printf("\nNo such edge\n");
        return;
    }

    if(e->vert_pos == pos2){
        Edge* tmp = e->next;
        free(e);
        G->vertexes[pos1].edges = tmp;
    }
    else{
        int is_exist = 0;
        Edge* nxt = e->next;
        while(nxt != NULL){
            if(nxt->vert_pos == pos2){
                is_exist = 1;
                e->next = nxt->next;
                free(nxt);
                break;
            }
            e = e->next;
            nxt = nxt->next;
        }
        if(is_exist == 0){
            printf("\nNo such edge\n");
            return;
        }
    }

    if(pos1 == pos2)
        return;

    e = G->vertexes[pos2].edges;

    if(e->vert_pos == pos1){
        Edge* tmp = e->next;
        free(e);
        G->vertexes[pos2].edges = tmp;
        return;
    }
    else{
        Edge* nxt = e->next;
        nxt = e->next;
        while(nxt != NULL){
            if(nxt->vert_pos == pos1){
                e->next = nxt->next;
                free(nxt);
                break;
            }
            e = e->next;
            nxt = nxt->next;
        }
    }
}

void delete_vertex(Graph* G, int x, int y){
    int pos = find_vertex(G, x, y);
    if(pos >= 0){
        printf("\nNo such vertex\n");
        return;
    }

    pos = -pos - 1;
    G->vertexes[pos].busy = 0;
    Edge* e = G->vertexes[pos].edges;
    while(e != NULL){
        Edge* e_nxt = e->next;

        if(e->vert_pos != pos){
            Edge* f = G->vertexes[e->vert_pos].edges;
            if(f->vert_pos == pos){
                Edge* tmp = f->next;
                free(f);
                G->vertexes[e->vert_pos].edges = tmp;
            }

            else{
                Edge* nxt = f->next;
                nxt = f->next;
                while(nxt != NULL){
                    if(nxt->vert_pos == pos){
                        f->next = nxt->next;
                        free(nxt);
                        break;
                    }
                    f = f->next;
                    nxt = nxt->next;
                }
            }
        }

        free(e);
        e = e_nxt;
    }
    G->vertexes[pos].edges = NULL;
}

void clean(Graph* G){
    for(int i = 0; i < G->size; i++){
        Edge* e = G->vertexes[i].edges;
        while(e != NULL){
            Edge* nxt = e->next;
            free(e);
            e = nxt;
        }
    }
    free(G->vertexes);
}

//------------queue-----------------------------
int pop(queue* q){
    if(q->first == NULL){
        return -1;
    }
    int vert = q->first->vert;

    if(q->first == q->last){
        free(q->first);
        q->first = NULL;
        q->last = NULL;
    }
    else{
        q_elem* tmp = q->first->next;
        free(q->first);
        q->first = tmp;
    }
    return vert;
}

void push_back(queue* q, int vertex){
    if(q->first == NULL){
        q->first = malloc(sizeof(q_elem));
        q->first->next = NULL;
        q->first->vert = vertex;
        q->last = q->first;
    }
    else{
        q->last->next = malloc(sizeof(q_elem));
        q->last = q->last->next;
        q->last->next = NULL;
        q->last->vert = vertex;
    }
}

//------------Algorithms------------------------
void print_way(int start, int vertex, int* way){
    if(vertex != start)
        print_way(start, way[vertex], way);
    printf("%d ", vertex);
}

void BFS(Graph* G, int x, int y, int x2, int y2){
    int start = find_vertex(G, x, y);
    int finish = find_vertex(G, x2, y2);

    if(start >= 0 || finish >= 0){
        printf("\nNo such vertexes\n");
        return;
    }

    if(finish == start){
        printf("\nVertexes are equal\n");
        return;
    }

    int* visited = calloc(G->size, sizeof(int));
    int* way = calloc(G->size, sizeof(int));
    int* dist = calloc(G->size, sizeof(int));

    queue q;
    q.first = NULL;
    q.last = NULL;

    start = -start - 1;
    finish = -finish - 1;

    push_back(&q, start);
    visited[start] = 1; 

    while(q.first != NULL){
        int vertex = pop(&q);
        Edge* e = G->vertexes[vertex].edges;
        while(e != NULL){
            if(visited[e->vert_pos] == 0){
                visited[e->vert_pos] = 1;
                push_back(&q, e->vert_pos);
                dist[e->vert_pos] = dist[vertex] + 1;
                way[e->vert_pos] = vertex;
                if(e->vert_pos == finish){
                    printf("\nSuccessfull\n");
                    printf("Distance: %d\n", dist[finish]);
                    print_way(start, finish, way);
                    printf("\n");

                    //free queue
                    while(q.first != NULL){
                        q_elem* tmp = q.first->next;
                        free(q.first);
                        q.first = tmp;
                    }
                    //free queue
                    free(visited);
                    free(way);
                    free(dist); 
                    return;
                }
            }
            e = e->next;
        }
    }
    printf("\nNo way through this two vertexes\n");
    free(visited);
    free(way);
    free(dist);
}


void show_tree(Graph* G, FILE* graph){

    for(int i = 0; i < G->size; i++){
        if(G->vertexes[i].busy != 0){
            if(G->vertexes[i].edges = NULL)
                fprintf(graph, "%d\n", i);
        }
    }
    /* if(node->left != Elist){
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
    } */
}