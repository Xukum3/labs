#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>
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

double dist(double a, double b, double x, double y){
    return sqrt((a-x)*(a-x)+(b-y)*(b-y));
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
    G->vertexes[pos1].edges->dist = dist((double)x, (double)y, (double)x2, (double)y2);
    //G->vertexes[pos1].edges->dist = sqrt((double)(x2-x)*(x2-x)+(y2-y)*(y2-y));

    if(pos1 != pos2){
        tmp = G->vertexes[pos2].edges;
        G->vertexes[pos2].edges = (Edge*)malloc(sizeof(Edge));
        G->vertexes[pos2].edges->next = tmp;
        G->vertexes[pos2].edges->vert_pos = pos1;
        G->vertexes[pos2].edges->dist = G->vertexes[pos1].edges->dist;
        //G->vertexes[pos2].edges->dist = dist((double)x, (double)y, (double)x2, (double)y2);
        //G->vertexes[pos2].edges->dist = sqrt((double)(x2-x)*(x2-x)+(y2-y)*(y2-y));

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
            printf("\n%lf %lf\n", G.vertexes[i].x, G.vertexes[i].y);
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
void print_way(int start, int vertex, int* way, Graph* G){
    if(vertex != start)
        print_way(start, way[vertex], way, G);
    printf("(%.2lf %.2lf)\n", G->vertexes[vertex].x, G->vertexes[vertex].y);
}

int BFS(Graph* G, int start, int finish, int* way, int* ans){
    if(start >= 0 || finish >= 0 || start < -G->size || finish < -G->size){
        return -2;
    }

    if(finish == start){
        return -1;
    }

    int* visited = calloc(G->size, sizeof(int));
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
                    //free queue
                    *ans = dist[finish];
                    while(q.first != NULL){
                        q_elem* tmp = q.first->next;
                        free(q.first);
                        q.first = tmp;
                    }
                    //free queue

                    free(visited);
                    free(dist); 
                    return 1;
                }
            }
            e = e->next;
        }
    }
    free(visited);
    free(dist);
    return 0;
}

int Dijkstra(Graph* G, int start, int finish, int* way, double* ans){
    if(start >= 0 || finish >= 0 || start < -G->size || finish < -G->size){
        return -2;
    }

    if(finish == start){
        return -1;
    }

    start = -start - 1;
    finish = -finish - 1;

    int* visited = calloc(G->size, sizeof(int));
    double* dist = malloc(G->size * sizeof(double));

    //-----------initialization-------------
    for(int i = 0; i < G->size; i++){
        dist[i] = INT_MAX;
    }

    visited[start] = 1;
    dist[start] = 0;
    //-----------initialization-------------

    int u = start;

    while(1){
        Edge* e = G->vertexes[u].edges;
        while(e != NULL){
            if(visited[e->vert_pos] == 0 && dist[e->vert_pos] > dist[u] + e->dist){
                dist[e->vert_pos] = dist[u] + e->dist;
                way[e->vert_pos] = u;
            }
            e = e->next;
        }

        visited[u] = 1;

        //---------find_next_vertex------------
        int w = INT_MAX;
        u = -1;

        for(int i = 0; i < G->size; i++){
            if(visited[i] == 0 && dist[i] < w){
                u = i;
                w = dist[i];
            }
        }
        //---------find_next_vertex------------

        if(u == -1){
            free(visited);
            free(dist);
            return 0;
        }

        if(u == finish){
            *ans = dist[finish];
            free(visited);
            free(dist);
            return 1;
        }
    }
}

void path(int* next, int a, int b, Graph* G){
    if(*(next + a*G->size + b) == -1){
        printf("\nNo way\n");
        return;
    }
    int i = 0;
    printf("Way: ");
    while(a != b){
        printf("%d->", a);
        a = *(next + a*G->size + b);
        i += 1;
        if(i == 10){
            break;
        }
    }
    printf("%d", b);
}

void Fl_V(Graph* G, int* next, e_FL* elems){
    double* matrix = (double*)malloc(G->size * G->size * sizeof(double));

    for (int i = 0; i < G->size; i++) {
        for(int j = 0; j < G->size; j++){
            if(i == j){ 
                *(matrix + i*G->size + j) = 0;
                *(next + i*G->size + j) = i;
            }
            else{
                *(matrix + i*G->size + j) = INT_MAX;
                *(next + i*G->size + j) = -1;
            }
        }
    }

    for(int i = 0; i < G->size; i++){
        Edge* e = G->vertexes[i].edges;
        while(e != NULL){
            *(matrix + i*G->size + e->vert_pos) = e->dist;
            *(next + i*G->size + e->vert_pos) = e->vert_pos;
            e = e->next;
        }
    }

    for(int k = 0; k < G->size; k++){
        for(int i = 0; i < G->size; i++){
            for(int j = 0; j < G->size; j++){
                if(*(matrix + i * G->size + j) >
                         *(matrix + i * G->size + k) + *(matrix + k * G->size + j)){

                    *(matrix + i * G->size + j) =
                                 *(matrix + i * G->size + k) + *(matrix + k * G->size + j);

                    *(next + i*G->size + j) = *(next + i*G->size + k);
                }
            }
        }
    }

    /* for (int i = 0; i < G->size; i++) {
        for(int j = 0; j < G->size; j++){
            printf("%.2lf ", *(matrix + i * G->size + j));
        }
        printf("\n");
    }

    for (int i = 0; i < G->size; i++) {
        for(int j = 0; j < G->size; j++){
            printf("%4d ", *(next + i * G->size + j));
        }
        printf("\n");
    } */

    elems[0].dist = INT_MAX;
    elems[1].dist = INT_MAX;
    elems[2].dist = INT_MAX;

    elems[0].pos1 = -1;
    elems[1].pos1 = -1;
    elems[2].pos1 = -1;

    elems[0].pos2 = -1;
    elems[1].pos2 = -1;
    elems[2].pos2 = -1;

    for(int i = 0; i < G->size - 1; i++){
        for(int j = i + 1; j < G->size; j++){
            if(*(matrix + i * G->size + j) < elems[0].dist){

                elems[2].pos1 = elems[1].pos1;
                elems[2].pos2 = elems[1].pos2;
                elems[2].dist = elems[1].dist;

                elems[1].pos1 = elems[0].pos1;
                elems[1].pos2 = elems[0].pos2;
                elems[1].dist = elems[0].dist;

                elems[0].pos1 = i;
                elems[0].pos2 = j;
                elems[0].dist = *(matrix + i * G->size + j);
            }

            else if(*(matrix + i * G->size + j) < elems[1].dist){
                elems[2].pos1 = elems[1].pos1;
                elems[2].pos2 = elems[1].pos2;
                elems[2].dist = elems[1].dist;

                elems[1].pos1 = i;
                elems[1].pos2 = j;
                elems[1].dist = *(matrix + i * G->size + j);
            }

            else if(*(matrix + i * G->size + j) < elems[2].dist){
                elems[2].pos1 = i;
                elems[2].pos2 = j;
                elems[2].dist = *(matrix + i * G->size + j);
            }
        }
    }
    free(matrix);
}
//----------------------other-------------------------

void show_graph(Graph* G, FILE* graph){

    for(int i = 0; i < G->size; i++){
        if(G->vertexes[i].busy == 1){
            fprintf(graph, "\t\"%d\" [pos=\"%.2lf,%.2lf!\"]\n", i, G->vertexes[i].x, G->vertexes[i].y);    
        }
    }
    for(int i = 0; i < G->size; i++){
        if(G->vertexes[i].busy == 1){
            if(G->vertexes[i].edges == NULL){
                //fprintf(graph, "\t%d %d;\n", G->vertexes[i].x, G->vertexes[i].y);
                continue;
            }

            Edge* e = G->vertexes[i].edges;
            while(e != NULL){
                if(i <= e->vert_pos)
                    //fprintf(graph, "\t\"%d %d\" -- \"%d %d\" [label = \"%.2lf\"];\n", \
                    G->vertexes[i].x, G->vertexes[i].y, G->vertexes[e->vert_pos].x, G->vertexes[e->vert_pos].y, e->dist);

                    fprintf(graph, "\t\"%d\" -- \"%d\";\n", i, e->vert_pos);
                e = e->next;
            }
        }
    }
}

void generate_graph(Graph* G, int v, int e){
    srand(time(NULL));
    int x, y, pos1, pos2;
    G->size = v;
    G->vertexes = (Vertex*)calloc(v, sizeof(Vertex));
    for(int i = 0; i < v; i++){
        x = rand() % v;
        y = rand() % v;

        int is_exist = 0;

        for(int j = 0; j < i; j++){
            if(G->vertexes[j].busy == 0)
                break;
            if(G->vertexes[j].busy != 0 && G->vertexes[j].x == x && G->vertexes[j].y == y){
                is_exist = 1;
            }
        }
        if(is_exist == 1){
            i -= 1;
            continue;
        }
        G->vertexes[i].x = x;
        G->vertexes[i].y = y;
        G->vertexes[i].busy = 1;
        G->vertexes[i].edges = NULL;
    }

    for(int i = 0; i < e; i++){
        pos1 = rand() % v;
        pos2 = rand() % v;
        add_edge(G, G->vertexes[pos1].x, G->vertexes[pos1].y, G->vertexes[pos2].x, G->vertexes[pos2].y);
    }
}

void write_graph(Graph* G, FILE* file){
    fwrite(&(G->size), sizeof(int), 1, file);
    for(int i = 0; i < G->size; i++){
        fwrite(&(G->vertexes[i].busy), sizeof(int), 1, file);
        //fprintf(file, "%d", G->vertexes[i].busy);
        /* if(G->vertexes[i].busy == 1){ */
        fwrite(&(G->vertexes[i].x), sizeof(int), 1, file);
        fwrite(&(G->vertexes[i].y), sizeof(int), 1, file);
        //fprintf(file, "%d%d", G->vertexes[i].x, G->vertexes[i].y);
    }
    for(int i = 0; i < G->size; i++){
        Edge* e = G->vertexes[i].edges;
        while(e != NULL){
            if(i <= e->vert_pos){
                fwrite(&(i), sizeof(int), 1, file);
                fwrite(&(e->vert_pos), sizeof(int), 1, file);
                fwrite(&(e->dist), sizeof(double), 1, file);
                //fprintf(file, "%d%d%lf", i, e->vert_pos, e->dist);
            }
            e = e->next;
        }
    }
}

int read_number(char* str, FILE* file){
    for(int i = 0; i < 15; i++){
        fread(str + i, 1, 1, file);
        if(str[i] != 45 && (str[i] < 48 || str[i] > 57)){
            str[i] = '\0';
            return atoi(str);
        }
    }
}

double read_double(char* str, FILE* file){
    for(int i = 0; i < 15; i++){
        fread(str + i, 1, 1, file);
        if((str[i] < 48 || str[i] > 57) && str[i] != 46 && str[i] != 45){
            str[i] = '\0';
            return atof(str);
        }
    }
}

void read_graph(Graph* G, FILE* file){
    rewind(file);
    char str[15];
    G->size = read_number(str, file);
    G->vertexes = (Vertex*)malloc(G->size * sizeof(Vertex));

    char buf;
    int rubbish;
    for(int i = 0; i < G->size; i++){
        rubbish = read_number(str, file);
        G->vertexes[i].x = read_double(str, file);
        G->vertexes[i].y = read_double(str, file);
        G->vertexes[i].edges = NULL;
        G->vertexes[i].busy = 1;
    }
    int edge_n = read_number(str, file);

    for(int i = 0; i < edge_n; i++){
        int rubbish;
        int pos1, pos2;
        double dist;
        rubbish = read_number(str, file);
        pos1 = read_number(str, file);
        pos2 = read_number(str, file);
        dist = read_double(str, file);

        Edge* tmp = G->vertexes[pos1].edges;
        G->vertexes[pos1].edges = (Edge*)malloc(sizeof(Edge));
        G->vertexes[pos1].edges->next = tmp;
        G->vertexes[pos1].edges->vert_pos = pos2;
        G->vertexes[pos1].edges->dist = dist;

        if(pos1 != pos2){
            tmp = G->vertexes[pos2].edges;
            G->vertexes[pos2].edges = (Edge*)malloc(sizeof(Edge));
            G->vertexes[pos2].edges->next = tmp;
            G->vertexes[pos2].edges->vert_pos = pos1;
            G->vertexes[pos2].edges->dist = dist;
        }
    }
}

void count_time(Graph* G){
    clean(G);
    srand(time(NULL));
    int avgBFS, avgDij, avgFl_V;
    clock_t begin, end;

    int num = 100, pos1, pos2;
    G->size = num;
    int size;

    G->vertexes = (Vertex*)calloc(G->size, sizeof(Vertex));

    for(int i = 0; i < 100; i++){
        G->vertexes[i].x = i;
        G->vertexes[i].y = i + 1;
        G->vertexes[i].busy = 1;
        G->vertexes[i].edges = NULL;
    }

    for(int i = 0; i < 500; i++){
        pos1 = rand() % G->size;
        pos2 = rand() % G->size;
        add_edge(G, G->vertexes[pos1].x, G->vertexes[pos1].y, G->vertexes[pos2].x, G->vertexes[pos2].y);
    }

    for(int step = 0; step < 10; step++){
        printf("\n\tFor %d elements:\n", num);
        avgBFS = 0; avgDij = 0; avgFl_V = 0;

        int* way = calloc(G->size, sizeof(int));

        for(int k = 0; k < 1000; k++){
            int start1 = -(rand() % G->size) - 1;
            int finish1 = -(rand() % G->size) - 1;
            int ans, result;
            begin = clock();
            result = BFS(G, start1, finish1, way, &ans);
            end = clock();
            /* if(result == 0)
                printf("\nNo way through this two vertexes\n");
            else if(result == -2)
                printf("\nNo such vertexes\n");
            else if(result == -1)
                printf("\nVertexes are equal\n");
            else{
                printf("\nSuccessfull\n");
                printf("Distance: %d\n\t", ans);
                print_way(-start1 - 1, -finish1 - 1, way, G);
                printf("\n");
            } */
            avgBFS += (end - begin);
        }
        printf("\navgBFS time: %lf", (double)avgBFS / 1000);
        for(int i = 0; i < G->size; i++){
            way[i] = 0;
        }

        for(int k = 0; k < 50; k++){
            int start1 = -(rand() % G->size) - 1;
            int finish1 = -(rand() % G->size) - 1;
            double ans;
            int result;
            begin = clock();
            result = Dijkstra(G, start1, finish1, way, &ans);
            end = clock();
            avgDij += (end - begin);
            /* if(result == 0)
                printf("\nNo way through this two vertexes\n");
            else if(result == -2)
                printf("\nNo such vertexes\n");
            else if(result == -1)
                printf("\nVertexes are equal\n");
            else{
                printf("\nSuccessfull\n");
                printf("Distance: %lf\n\t", ans);
                print_way(-start1 - 1, -finish1 - 1, way, G);
                printf("\n");
            } */
        }
        printf("\navgDij time: %lf", (double)avgDij / 50);

        for(int k = 0; k < 10; k++){
            int* next = (int*)malloc(G->size * G->size * sizeof(int));
            e_FL elems[3];
            begin = clock();
            Fl_V(G, next, elems);
            end = clock();
            avgFl_V += (end - begin);
            free(next);
        }
        printf("\navgFl_v time: %lf", (double)avgFl_V / 10000);
        free(way);
        if(step != 9){
            printf("\ngoing to new test...\n");
            G->vertexes = (Vertex*)realloc(G->vertexes, (G->size + 100)*sizeof(Vertex));
            for(int i = 0; i < 100; i++){
                G->vertexes[num + i].x = num + i;
                G->vertexes[num + i].y =  num + i + 1;
                G->vertexes[num + i].busy = 1;
                G->vertexes[num + i].edges = NULL; 
            }

            num += 100;
            G->size += 100;

            for(int i = 0; i < 500; i++){
                pos1 = rand() % G->size;
                pos2 = rand() % G->size;
                add_edge(G, G->vertexes[pos1].x, G->vertexes[pos1].y, G->vertexes[pos2].x, G->vertexes[pos2].y);
            }
            printf("successfull! new test...\n");
        }
    }
}

 /*  while(1){
            fread(&buf, 1, 1, file);
            if(str[i] == 45 || (str[i] >= 48 && str[i] <= 57)){
                fseek(file, -1, SEEK_SET);
                break;
            }
        } */

