#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <climits>

//extern "C" {
  #include "mmio.h"
//}

int main(int argc, char *argv[])
{
    int ret_code;
    FILE* f;
    MM_typecode matcode;
    int M, N, nz;   
    int firstVert;

    if (argc < 3)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename] [initial vertex]\n", argv[0]);
		exit(1);
	}
    else    
    { 
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

    std::vector<std::vector<double>> adjacency(M, std::vector<double>(N+2, 0));
    int fi, fj;

    for (int i = 0; i < nz; i++){
        fscanf(f, "%d %d ", &fi, &fj);
        --fi;
        --fj;
        fscanf(f, "%lg\n", &adjacency[fi][fj]);
        adjacency[fj][fi] = adjacency[fi][fj];
    }
    fclose(f);

    // Auxiliary varibles for the dfs algorithm
    std::stack<int> dfsQueue;
    std::vector<int> order;
    std::vector<bool> alreadyVisited;
    int currOrder;
    int currVertex;

    // Initialization for aux variables
    dfsQueue.push(firstVert);
    order.resize(M, INT_MAX);
    order[firstVert] = 0;
    alreadyVisited.resize(M, false);
    alreadyVisited[firstVert] = true;
    currOrder = 0;

    // DFS main loop
    while (!dfsQueue.empty()){
        currVertex = dfsQueue.top();
        dfsQueue.pop();

        for (int i = 0; i < N; i++){
            if (!alreadyVisited[i] && adjacency[currVertex][i] != 0){
                dfsQueue.push(i);
                alreadyVisited[i] = true;
                currOrder++;
                order[i] = currOrder;
            }
        }
    }

    for (int i = 0; i < M; i++){
        printf("%d %d\n", i+1, order[i]);
    }
/*
    FILE* outputFile;
    outputFile = fopen("outputBreadth.mtx", "w");
    if (outputFile == NULL) {
        std::cout << "\nError writing file\n";
        return 1;
    }
    mm_write_banner(outputFile, matcode); 
    mm_write_mtx_crd_size(outputFile, M, N, nz);

    for (int i = 0; i < M; i++){
        for (int j = 0; j < N; j++){
            if (adjacency[i][j] != 0){
                fprintf(outputFile, "%d %d %10.3g\n", i+1, j+1, adjacency[i][j]);
            }
        }
    }

    fclose(outputFile);
*/
    return 0;
}
