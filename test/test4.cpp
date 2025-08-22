#include "../BMSSP.h"

int main() {

	//Graph g1("../../../test/graph1.txt");
	//BMSSP bmsspc1(g1.transform2ConstDeg());
	//bmsspc1.solve();
	//bmsspc1.printDhat();

	//Graph g2("../../../test/graph2.txt");
	//BMSSP bmsspc2(g2.transform2ConstDeg());
	//bmsspc2.solve();
	//bmsspc2.printDhat();

	//genRandGraph2File("../../../test/graph3.txt", 200, 600);
	//Graph g3("../../../test/graph3.txt");
	//BMSSP bmsspc3(g3.transform2ConstDeg());
	//bmsspc3.solve();
	//bmsspc3.printDhat();

	//genRandGraph2File("../../../test/graph4.txt", 1000, 3000);
	//Graph g4("../../../test/graph4.txt");
	//BMSSP bmsspc4(g4.transform2ConstDeg());
	//bmsspc4.solve();
	//bmsspc4.printDhat();

	//genRandGraph2File("../../../test/graph5.txt", 10000, 30000);
	Graph g5("../../../test/graph5.txt");
	BMSSP bmsspc5(std::move(g5));
	bmsspc5.solve();
	bmsspc5.printLengths();

	std::cin.get();
	return 0;
}