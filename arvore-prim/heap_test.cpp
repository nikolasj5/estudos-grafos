#include <iostream>
#include <queue>

using namespace std;

// Creates a min-heap. Used in Prim algorithm during Sanderson graph class
void createHeap(){
    int arr[] = {3, 4, 2, 9, 11, 9, 0, -1, -3, -111, 400,2, 1, 5, 10, 4, 10, 20};
    int n = sizeof(arr)/sizeof(arr[0]);
    priority_queue<int, vector<int>, greater<int>> heap;
    //priority_queue<int> heap;

    for (int i = 0; i < n; i++){
        heap.push(arr[i]);
    }

    while (!heap.empty()){
        cout << heap.top() << endl;
        heap.pop();
    }
}

int main(int argc, char *argv[]){
    pair<int,int> x = {1,1};
    pair<int,int> y = {1,0};

    cout << "x > y = " << (x > y) << endl;
    cout << "x < y = " << (x < y) << endl;
    cout << "x == y = " << (x == y) << endl;
    cout << "x >= y = " << (x >= y) << endl;
    cout << "x <= y = " << (x <= y) << endl;

    return 0;
}