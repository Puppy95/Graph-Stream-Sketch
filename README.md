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
* nodeSuccessorQuery: Query for the successor of a given node
* nodePrecursorQuery: Query for the precursor of a given node
* TriangleCouning: Count the number of triangles in a graph
Details can be seen in the comments in the code
Note that the number of rooms in each bucket is controlled by a macro definition Roomnum.

GSS_label.h
------------
Graph Stream Sketch for subgraph matching, revised a bit for label storage. It takes integer as node ID and labels. The node ID should be int type from 1 to n where n is the number of nodes, so is the queried graph. ID map may be needed when using it.

TCM.h
------------
TCM user interface:
* insert : Insert one item
* edgeQuery(string s1, string s2) : Edge Query
* transquery: Reachability Query
* nodeValueQuery: Node Value Query to compute the summary of the weights of all edges
  with s1. When s1 is the source node,type = 0. s1 is the destination node,type = 1.
* nodeDegreeQuery: Node  Degree Query to compute the number of the precursors(type=1)/successors(type=0) of s1.

ListGraph.h
------------
Adjacency list to store graph

hashfunction.h
------------
hashfunction

hashTable.h
------------
hashTable
------------
The code of TRIEST in experiment of the paper is downloaded from https://github.com/aepasto/triest provided by the authors. The code for SJtree is on https://github.com/Puppy95/timingsubg. Further arrangement of the code will be made before publication.
