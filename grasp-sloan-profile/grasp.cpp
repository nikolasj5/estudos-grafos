#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <climits>
#include <random>
#include <chrono>
#include <algorithm>

#include "mmio.h"

#define graspMax 5
#define alpha 0.5 // between 0 and 1

typedef std::vector<std::vector<double>> adjGraph;

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

void Construcao(std::vector<int>& solution, adjGraph graph){
	solution.clear();
	std::vector<std::pair<int,int>> candidates;
	int rank, graphSize = graph.size();

	for (int i = 0; i < graphSize; i++){
		rank = 0;
		for (int j = 0; j < graphSize; j++){
			if (graph[i][j] != 0)	rank++;
		}
		candidates.push_back({rank, i});
	}
	std::sort(candidates.begin(), candidates.end());

	int g_min, g_max;
	std::vector<std::pair<int,int>> LCR;
	std::pair<int,int> element;
	float upper_bound;
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator (seed);
	
	while (!candidates.empty()){
		g_min = candidates.front().first;
		g_max = candidates.back().first; 

		upper_bound = g_min + alpha*(g_max-g_min);
		LCR.clear();
		for (int i = 0; i < candidates.size() && candidates[i].first <= upper_bound; i++){
			LCR.push_back(candidates[i]);
		}

		element = LCR[generator() % (LCR.size())];
		solution.push_back(element.second);
		candidates.erase(std::find(candidates.begin(), candidates.end(), element));
	}
}

void BuscaLocal(std::vector<int>& solution, adjGraph graph){
	int bestProfile = calcProfile(graph, graph.size(), solution);
	int candidateProfile;
	std::vector<int> candidate(solution);

	for (int i = 0; i < solution.size(); i++){
		std::rotate(candidate.begin(), candidate.begin()+1, candidate.end());
		candidateProfile = calcProfile(graph, graph.size(), candidate);

		if (candidateProfile < bestProfile){
			bestProfile = candidateProfile;
			solution = candidate;
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

	//int mSize = M>=N ? M : N;
	adjGraph adjacency(M, std::vector<double>(N, 0));
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

	// Starts with original profile and ordering
	int bestProfile = calcProfile(adjacency, M);
	printf("Profile inicial = %d\n", bestProfile);
	std::vector<int> bestSolution;
	for (int i = 0; i < M; i++) bestSolution.push_back(i);

	std::vector<int> currSolution;
	int currProfile;
	for (int graspIter = 0; graspIter < graspMax; graspIter++){
		Construcao(currSolution, adjacency);
		BuscaLocal(currSolution, adjacency); 

		currProfile = calcProfile(adjacency, M, currSolution);

		if (currProfile < bestProfile){
			bestSolution = currSolution;
			bestProfile = currProfile;
		}
	}

	printf("Profile final = %d\n", bestProfile);
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
