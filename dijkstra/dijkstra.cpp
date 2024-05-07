#include <iostream>
#include <queue>
#include <climits>

#include "mmio.h"

using namespace std;

int main(int argc, char *argv[]){
    int ret_code;
    FILE* f;
    MM_typecode matcode;
    int M, N, nz;   
    int firstVert;

/*
    if (argc == 2) { 
        if ((f = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "%s is not a valid mtx filename\n", argv[1]);
            exit(1);
        }

        firstVert = 0;  
    }*/

    if (argc < 3 && argc != 2){
		fprintf(stderr, "Usage: %s [martix-market-filename] [initial vertex]\n", argv[0]);
		exit(1);
	}   
    else { 
        if ((f = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "%s is not a valid mtx filename\n", argv[1]);
            exit(1);
        }

        if (atoi(argv[2])-1 >= 0){
            firstVert = atoi(argv[2])-1;
        }else{
            fprintf(stderr, "Vertex index starts in 1\n", argv[1]);
            exit(1);
        }
    }

    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0){
        printf("Error while reading matrix size and type\n");
        exit(1);
    }

    vector<vector<double>> adjacency(M, vector<double>(N+2, 0));
    int fi, fj, cont;

    for (int i = 0; i < nz; i++){
        fscanf(f, "%d %d ", &fi, &fj);
        --fi;
        --fj;
        fscanf(f, "%lg\n", &adjacency[fi][fj]);
        adjacency[fj][fi] = adjacency[fi][fj];
    }
    fclose(f);

    vector<int> route;
    vector<int> cost;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> heap;
    pair<int,int> auxVert;

    // Initializing variables
    route.resize(M, -1);
    cost.resize(M, INT_MAX);

    auxVert.first = 0;
    auxVert.second = firstVert;
    heap.push(auxVert);
    route[firstVert] = firstVert;
    cost[firstVert] = 0;

    int weight, index;
    while (!heap.empty()){
        auxVert = heap.top();
        heap.pop();
        weight = auxVert.first;
        index = auxVert.second;

        for (int i = 0; i < M; i++){
            if (adjacency[index][i] != 0 && cost[i] > adjacency[index][i] + weight){
                route[i] = index;
                cost[i] = adjacency[index][i] + weight;
                heap.push({cost[i], i});
            }
        }
    }

    printf("Costs:\n");
    for (int i = 0; i < M; i++){
        printf("%d %d\n", i+1, cost[i]);
    }

    printf("\n\nRoute:\n");
    for (int i = 0; i < M; i++){
        printf("%d %d\n", i+1, route[i]);
    }

    return 0;
}