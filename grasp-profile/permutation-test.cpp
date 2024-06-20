#include <iostream>
#include <vector>

using namespace std;

int main(){
    vector<vector<int>> teste = {{1,1,0,0}
                                ,{1,1,1,1}
                                ,{0,1,1,0}
                                ,{0,1,0,1}};

    vector<int> permutation = {1,0,2,3};

    int matrixLen = teste.size();
    vector<vector<int>> result(matrixLen, vector<int>(matrixLen));
    for (int i = 0; i < matrixLen; i++){
        for (int j = 0 ; j < matrixLen; j++){
            result[i][j] = teste[permutation[i]][j];
        }
    }

    teste = result;
    for (int j = 0; j < matrixLen; j++){
        for (int i = 0 ; i < matrixLen; i++){
            result[i][j] = teste[i][permutation[j]];
        }
    }

    for (int j = 0; j < matrixLen; j++){
        for (int i = 0 ; i < matrixLen; i++){
            cout << result[i][j] << ' ';
        }
        cout << endl;
    }

    return 0;
}