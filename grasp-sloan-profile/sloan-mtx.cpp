#include <iostream>
#include <vector>
#include <queue>
#include <string>

#include "mmio.h"

typedef std::vector<std::vector<double>> adjGraph;

struct Node{
	int status;
	float priority;
	int originalLabel;
	int degree;
	int distanceEnd;
};

int calcProfile(adjGraph matrixAdj, int matrixSize){
	int jmin;
	int profile = 0;

	for (int i = 0; i < matrixSize; i++){
		jmin = -1;
		for (int j = 0; jmin == -1 && j < i; j++){
			if (matrixAdj[i][j] != 0){
				jmin = j;
			}
		}
		if (jmin != -1){
			profile += i - jmin + 1;	
		}else{
			profile += 1;
		}
	}

	return profile;
}

int calcProfile(adjGraph matrixAdj, int matrixSize, std::vector<int> ordering){
	int jmin;
	int profile = 0;

	for (int i = 0; i < matrixSize; i++){
		jmin = -1;
		for (int j = 0; jmin == -1 && j < i; j++){
			if (matrixAdj[ordering[i]][ordering[j]] != 0){
				jmin = j;
			}
		}
		if (jmin != -1){
			profile += i - jmin + 1;	
		}else{
			profile += 1;
		}
	}
		
	return profile;
}

std::pair<int,int> pseudoPeripheral(adjGraph adjacency, std::vector<Node>& nodes){
	// Variables for pseudo peripheral node
	int levelR, levelX, currLevel;
	int peripNodeR, peripNodeX, currNode;
	std::queue<std::pair<int,int>> peripQueue;
	std::vector<bool> alreadyVisitedR, alreadyVisitedX;
	int iter = 0;
	int graphSize = adjacency.size();

	peripNodeR = 1;
	peripNodeX = 1;
	do{
		peripNodeR = peripNodeX;
		levelR = 0;
		peripQueue.push({peripNodeR,levelR});
		alreadyVisitedR.clear();
		alreadyVisitedR.resize(graphSize, false);
		alreadyVisitedR[peripNodeR] = true;
		while (peripQueue.size() > 0){
			currNode = peripQueue.front().first;
			currLevel = peripQueue.front().second;
			peripQueue.pop();
			for (int i = 0; i < graphSize; i++){
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
		alreadyVisitedX.resize(graphSize, false);
		alreadyVisitedX[peripNodeX] = true;
		nodes[peripNodeX].distanceEnd = 0;
		while (peripQueue.size() > 0){
			currNode = peripQueue.front().first;
			currLevel = peripQueue.front().second;
			peripQueue.pop();
			for (int i = 0; i < graphSize; i++){
				if (adjacency[currNode][i] != 0 && alreadyVisitedX[i] == false){
					alreadyVisitedX[i] = true;
					peripQueue.push({i, currLevel+1});
					nodes[i].distanceEnd = currLevel + 1;
					if (currLevel+1 > levelX){
						levelX = currLevel + 1;
					}
				}
			}
		}
		iter++;
	}while (levelX > levelR && iter < 5);
	
	return {peripNodeR, peripNodeX};
}

#define inactive 	1
#define preactive 	2
#define active 		3
#define postactive 	4

void sloanAlgo(adjGraph adjacency, std::vector<int>& solution, int w1 = 1, int w2 = 2){
	// Sloan
	int graphSize = adjacency.size();

	// Passing through all nodes setting initial properties
	std::vector<Node> nodes(graphSize);
	int cont;
	for (int  i = 0; i < graphSize; i++){
		nodes[i].priority = 0;
		nodes[i].originalLabel = i;
		cont = 0;
		for (int j = 0; j < graphSize; j++){
			if (adjacency[i][j] != 0) cont++;
		}
		nodes[i].degree = cont;
		nodes[i].distanceEnd = -1;
	}

	// Getting peripheral nodes from George and Liu method
	std::pair<int,int> peripheral;
	peripheral = pseudoPeripheral(adjacency, nodes);
	
	for (int i = 0; i < graphSize; i++){
		nodes[i].status = inactive;
		nodes[i].priority = w1 * nodes[i].distanceEnd - w2 * (nodes[i].degree + 1);
	}
	
	std::priority_queue<std::pair<int, int>> heap, temp_heap;

	nodes[peripheral.first].status = preactive;
	heap.push({nodes[peripheral.first].priority, peripheral.first});

	//solution.push_back(peripheral.first);
	
	int currNode;
	while (!heap.empty()){
		currNode = heap.top().second;
		heap.pop();

		if (nodes[currNode].status == preactive){
			for (int j = 0; j < graphSize; j++){
				if (adjacency[currNode][j] != 0){
					nodes[j].priority += w2;
					if (nodes[j].status == inactive){
						nodes[j].status = preactive;
						heap.push({nodes[j].priority, j});
					}
				}
			}
		}
		solution.push_back(currNode);
		nodes[currNode].status = postactive;
		
		for (int j = 0; j < graphSize; j++){
			if (adjacency[currNode][j] != 0 && nodes[j].status == preactive){
				nodes[j].status = active;
				nodes[j].priority += w2;
				
				for (int k = 0; k < graphSize; k++){
					if (adjacency[j][k] != 0 && nodes[k].status != postactive){
						nodes[k].priority += w2;
						if (nodes[k].status == inactive){
							nodes[k].status = preactive;
							heap.push({nodes[k].priority, k});
						}
					}
				}
			}
		}
		
		int index;
		if (!temp_heap.empty()) std::cout << "temp_heap arriving wrong" << std::endl;
		while (!heap.empty()){
			index = heap.top().second;
			heap.pop();
			
			temp_heap.push({nodes[index].priority, index});
		}
		heap.swap(temp_heap);
	}
	
	for (int i = 0; i < graphSize; i++){
		if (nodes[i].status != postactive) {
			solution.push_back(i);
		}
	}
}

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

	std::vector<std::vector<double>> adjacency(M, std::vector<double>(N, 0));
	int fi, fj;

	for (int i = 0; i < nz; i++){
		fscanf(f, "%d %d ", &fi, &fj);
		--fi;
		--fj;
		//fscanf(f, "%lg\n", &adjacency[fi][fj]);
		adjacency[fi][fj] = 1;
		adjacency[fj][fi] = 1;
	}
	fclose(f);

	std::cout << "---------------" << std::endl;	
	std::string instance_name(argv[1]);
	instance_name = instance_name.substr(instance_name.find("/")+1);
	std::cout << instance_name << std::endl;
	std::cout << std::endl;

	printf("Profile inicial = %d\n", calcProfile(adjacency, M));

	// Auxiliary varibles for the bfs algorithm
	std::queue<int> bfsQueue;
	std::vector<int> result;
	std::vector<bool> alreadyVisited;
	int currDistance;
	int currVertex;
/*	
	std::vector<Node> aaa(adjacency.size());
	int startNode = pseudoPeripheral(adjacency, aaa).first;

	// Initialization for aux variables
	bfsQueue.push(startNode);
	alreadyVisited.resize(M, false);
	alreadyVisited[startNode] = true;

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
	printf("Profile final rbfs-gl = %d\n", calcProfile(adjacency, M, result));
*/	
	sloanAlgo(adjacency, result);

	std::cout << "Profile final sloan = " << calcProfile(adjacency, M, result) << std::endl;
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
