Fast and Accurate Graph Stream Summarization
=======
GSS.h
------------
Graph Stream Sketch user interface:
* insert: Insert one item
* edgeQuery: Edge Query
* transquery: Reachability Query
* nodeValueQuery: Node Value Query to compute the summary of the weights of all edges
  with s1. When s1 is the source node,type = 0. s1 is the destination node,type = 1.
* nodeDegreeQuery: Node  Degree Query to compute the number of the precursors(type=1)/successors(type=0) of s1.

TCM.h
------------
TCM user interface:
* insert : Insert one item
* edgeQuery(string s1, string s2) : Edge Query
* transquery: Reachability Query
* nodeValueQuery: Node Value Query to compute the summary of the weights of all edges
  with s1. When s1 is the source node,type = 0. s1 is the destination node,type = 1.
* nodeDegreeQuery: Node  Degree Query to compute the number of the precursors(type=1)/successors(type=0) of s1.

Graph.h
------------
Adjacency list to store graph

hashfunction.h
------------
hashfunction

hashTable.h
------------
hashTable

main.cpp
------------
run experiments, 1-hop precursors/successors as an example
