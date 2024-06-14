#include <iostream>
#include <vector>
#include <queue>

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
    int fi, fj;

    for (int i = 0; i < nz; i++){
        fscanf(f, "%d %d ", &fi, &fj);
        --fi;
        --fj;
        fscanf(f, "%lg\n", &adjacency[fi][fj]);
        adjacency[fj][fi] = adjacency[fi][fj];
    }
    fclose(f);

    // Variables for pseudo peripheral node
    int levelR, levelX, currLevel;
    int peripNodeR, peripNodeX, currNode;
    std::queue<std::pair<int,int>> peripQueue;
    std::vector<bool> alreadyVisitedR, alreadyVisitedX;
    int iter = 0;

    peripNodeR = 1;
    peripNodeX = 1;
    do{
        levelR = 0;
        peripQueue.push({peripNodeR,levelR});
        alreadyVisitedR.clear();
        alreadyVisitedR.resize(M, false);
        alreadyVisitedR[peripNodeR] = true;
        while (peripQueue.size() > 0){
            currNode = peripQueue.front().first;
            currLevel = peripQueue.front().second;
            peripQueue.pop();
            for (int i = 0; i < M; i++){
                if (adjacency[currNode][i] != 0 && alreadyVisitedR[i] == false){
                    alreadyVisitedR[i] = true;
                    peripQueue.push({i, currLevel+1});
                    if (currLevel+1 > levelR){
                        levelR = currLevel + 1;
                        peripNodeX = i;
                    }
                }
            }
        }

        levelX = 0;
        peripQueue.push({peripNodeX,levelX});
        alreadyVisitedX.clear();
        alreadyVisitedX.resize(M, false);
        alreadyVisitedX[peripNodeX] = true;
        while (peripQueue.size() > 0){
            currNode = peripQueue.front().first;
            currLevel = peripQueue.front().second;
            peripQueue.pop();
            for (int i = 0; i < M; i++){
                if (adjacency[currNode][i] != 0 && alreadyVisitedX[i] == false){
                    alreadyVisitedX[i] = true;
                    peripQueue.push({i, currLevel+1});
                    if (currLevel+1 > levelX){
                        levelX = currLevel + 1;
                    }
                }
            }
        }
        iter++;
    }while (levelX > levelR && iter < 5);

    // Auxiliary varibles for the bfs algorithm
    std::queue<int> bfsQueue;
    std::vector<int> result;
    std::vector<bool> alreadyVisited;
    int currDistance;
    int currVertex;

    // Initialization for aux variables
    bfsQueue.push(peripNodeX);
    alreadyVisited.resize(M, false);
    alreadyVisited[peripNodeX] = true;

    // BFS main loop
    while (!bfsQueue.empty()){
        currVertex = bfsQueue.front();
        bfsQueue.pop();
        result.push_back(currVertex);

        for (int i = 0; i < N; i++){
            if (!alreadyVisited[i] && adjacency[currVertex][i] != 0){
                bfsQueue.push(i);
                alreadyVisited[i] = true;
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