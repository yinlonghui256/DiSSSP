#include "../BMSSP.h"

int main() {

	//Graph g1("../../../test/graph1.txt");
	//BMSSP bmsspc1(g1.transform2ConstDeg());
	//bmsspc1.solve();
	//bmsspc1.printDhat();

	Graph g2("../../../test/graph2.txt");
	BMSSP bmsspc2(g2.transform2ConstDeg());
	bmsspc2.solve();
	bmsspc2.printDhat();


	return 0;
}