from scipy.cluster.hierarchy import DisjointSet
import heapq
from scipy.io import mmread
import sys

# read command argument
if (len(sys.argv) >= 2):
    filename_input = sys.argv[1]
else:
    print("Usage: python kruskal [filename].mtx")
    exit()

# input
graph_read = mmread(filename_input)

# Creating priority queue (heap)
heap = []
for i,j,v in zip(graph_read.row, graph_read.col, graph_read.data):
    heap.append((v, i, j))
heapq.heapify(heap)

# Auxiliary set created as disjoint set datatype
disjoint_florest = DisjointSet(range(graph_read.shape[0]))

minimum_tree_index = []

while(disjoint_florest.n_subsets > 1):
    (val, v1, v2) = heapq.heappop(heap)
    if (disjoint_florest.connected(v1,v2) == False):
        minimum_tree_index.append((v1,v2))
        disjoint_florest.merge(v1,v2)

minimum_tree_index.sort()

for i,j in minimum_tree_index:
    print(i, j)
