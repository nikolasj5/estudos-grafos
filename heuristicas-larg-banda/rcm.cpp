#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

#include "mmio.h"

int main(int argc, char *argv[])
{
    int ret_code;
    FILE* f;
    MM_typecode matcode;
    int M, N, nz;   

    if (argc < 2)
	{
		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
		exit(1);
	}
    else    
    { 
        if ((f = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "%s is not a valid mtx filename\n", argv[1]);
            exit(1);
        }
    }
    
    if (mm_read_banner(f, &matcode) != 0)
    {
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0){
        exit(1);
    }

    std::vector<std::vector<double>> adjacency(M, std::vector<double>(N+2, 0));
    std::vector<int> degrees;
    int fi, fj;

    degrees.resize(M, 0);
    for (int i = 0; i < nz; i++){
        fscanf(f, "%d %d ", &fi, &fj);
        --fi;
        --fj;
        fscanf(f, "%lg\n", &adjacency[fi][fj]);
        adjacency[fj][fi] = adjacency[fi][fj];
        degrees[fi]++;
        degrees[fj]++;
    }
    fclose(f);

    // Auxiliary varibles for the RCM algorithm
    std::queue<int> bfsQueue;
    std::vector<int> result;
    std::vector<bool> alreadyVisited;
    std::vector<std::pair<int,int>> orderAdj;
    int currDistance;
    int currVertex;

    // Initialization for aux variables
    alreadyVisited.resize(M, false);

    //First iteration with simple RCM lowest degree vertex
    std::pair<int,int> lowest = {degrees[0], 0};
    for (int i = 1; i < M; i++){
        if (degrees[i] < lowest.first){
            lowest = {degrees[i], i};
        }
    }
    result.push_back(lowest.second);
    alreadyVisited[lowest.second] = true;

    orderAdj.clear();    
    for (int i = 0; i < M; i++){
        if (adjacency[lowest.second][i] != 0){
            orderAdj.push_back({degrees[i],i});
        }
    }
    std::sort(orderAdj.begin(), orderAdj.end());

    for (int i = 0; i < orderAdj.size(); i++){
        bfsQueue.push(orderAdj[i].second);
    }

    // RCM main loop
    while (!bfsQueue.empty()){
        currVertex = bfsQueue.front();
        bfsQueue.pop();
        if (alreadyVisited[currVertex] == false){
            result.push_back(currVertex);
            alreadyVisited[currVertex] = true;

            orderAdj.clear();    
            for (int i = 0; i < M; i++){
                if (adjacency[currVertex][i] != 0 && alreadyVisited[i] == false){
                    orderAdj.push_back({degrees[i],i});
                }
            }
            std::sort(orderAdj.begin(), orderAdj.end());

            for (int i = 0; i < orderAdj.size(); i++){
                bfsQueue.push(orderAdj[i].second);
            }
        }
    }

    for (int i = 0; i < M; i++){
        printf("%d %d\n", i, result[i]+1);
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