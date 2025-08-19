#include "../Graph.h"

int main() {
	Graph g(5);
	g.addEdge(0, 1, 1.2);
	g.addEdge(1, 2, 2.3);
	g.addEdge(2, 3, 3.4);
	g.addEdge(3, 4, 4.5);
	g.addEdge(4, 0, 5.6);
	std::cout << "Original Graph:" << std::endl;
	std::cout << g;
	Graph transformedGraph = g.transform2ConstDeg();
	std::cout << "Transformed Graph:" << std::endl;
	std::cout << transformedGraph;

	Graph g2(7);
	g2.addEdge(0, 1, 4);
	g2.addEdge(1, 2, 1);
	g2.addEdge(0, 3, 8);
	g2.addEdge(2, 3, 2);
	g2.addEdge(3, 4, 3);
	g2.addEdge(2, 4, 6);
	g2.addEdge(0, 5, 2);
	g2.addEdge(6, 0, 9);
	std::cout << "Original Graph 2:" << std::endl;
	std::cout << g2;
	Graph transformedGraph2 = g2.transform2ConstDeg();
	std::cout << "Transformed Graph 2:" << std::endl;
	std::cout << transformedGraph2;


	genRandGraph2File("test_graph.txt", 10, 20, 1.0, 10.0, 1);
	Graph g3("test_graph.txt");
	std::cout << "Original Graph 3:" << std::endl;
	std::cout << g3;
	Graph transformedGraph3 = g3.transform2ConstDeg();
	std::cout << "Transformed Graph 3:" << std::endl;
	std::cout << transformedGraph3;

	return 0;
}