#include "DiSSSP.h"

int main()
{

	/*
		The structure of the project:
		DiSSSP:
			class Graph
				handles input of a directed graph
				the graph is represented in the way of adjacency lists.
				index vertex from 0 to n-1.
				reforming into a cconstant degree graph
				reading in edges and out edges from a vertex 
	
			function: BMSSP
				need parameter:
				S unordered_set of indexes of vertices.
				l layer which decides the parameter M to be used.
				k globally decided parameter of BF-depth
				
			class FrontierManager
				Quantified by parameter M = 2^{lt} (if l=0, M=1)
				Maintains Frontier into separate Blocks.
				Each block contains an unordered set of VertexIndex
					No, block need to be implemented by linked list, not unordered_set.
					maintains lower_bound, upper_bound, and num_of_items.
					It always contains its elements in [lower_bound, upper_bound) pattern.
					Any block at any time contains no more than M items, and at least 1 item.
					Supports: in linear time, find the first k items. (including finding the first [m/2] items)
				In the block level, they are sorted and into two containers:
					D1 is a monotone linked list, receives input of batch-prepend.
					D2 is a red-black tree (binary balanced tree)
						but in addition to std::map, it need to support Split operation so we have to manually implement it.

				supported operations:

				Insert(Single) insert a (K, V) pair.
					time: log(N/M)
					if it hasn't been added into the frontier, add it.
					if it already exists in the frontier:
						if the new value V is smaller: remove the old copy and insert the new instance;
						else, do nothing.

				Batch-prepend insert an unordered set L of keys taking time L\log(L/M).
					requires: every key in L are smaller than every key existing in the frontier; ensured by algorithm.
					needs to call linear median algorithm to split them into O(L/M) blocks and prepend them to D1.

				Pull pull a Block S which are the smallest items in the frontier.
					if the frontier contains more than M items, |S|=M.
					else, pull all items in the frontier.
				
				Each vertex is either in D0 (linked list) or D2 (BST)
					so we let each vertex to store their corresponded block.
				

	*/

	return 0;
}
