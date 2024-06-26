#include <random>
#include <chrono>
#include <iostream>

void gera(){
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 generator (seed);
	
	std::cout << generator() << std::endl;
}

int main(){
	for (int i = 0; i < 10; i++)	gera();
	
	return 0;
}
