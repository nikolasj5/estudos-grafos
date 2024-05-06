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

    if (argc == 2) { 
        if ((f = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "%s is not a valid mtx filename\n", argv[1]);
            exit(1);
        }

        firstVert = 1;  
    }
/*
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
    }*/

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
/*
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (adjacency[i][j] < 0) cout << adjacency[i][j] << endl;
        }
    }
*/

    vector<int> previousVert;
    vector<bool> alreadyVisited;
    vector<int> key;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> heap;
    pair<int,int> auxVert;

    // Initializing variables
    previousVert.resize(M, -1);
    alreadyVisited.resize(M, false);
    key.resize(M, INT_MAX);

    /*
    auxVert.first = INT_MAX;
    for (int i = 0; i < M; i++){
        if (i != firstVert){
            auxVert.second = i;
            heap.push(auxVert);
        }
    }
    */

    auxVert.first = 0;
    auxVert.second = firstVert;
    heap.push(auxVert);
    key[firstVert] = 0;

    int cost, index;
    while (!heap.empty()){
        auxVert = heap.top();
        heap.pop();
        cost = auxVert.first;
        index = auxVert.second;

        if (alreadyVisited[index] != true){
            alreadyVisited[index] = true;

            for (int i = 0; i < M; i++){
                if (alreadyVisited[i] == false && key[i] > adjacency[index][i]){
                    key[i] = adjacency[index][i];
                    heap.push({key[i], i});
                    previousVert[i] = index;
                }
            }
        }
    }

    for (int i = 0; i < M; i++){
        printf("%d %d\n", i+1, (previousVert[i]==-1)?(-1):previousVert[i]+1);
    }

    return 0;
}