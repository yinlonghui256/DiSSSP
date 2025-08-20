#include "../Length.h"
#include <random>

int main() {
	std::vector<int> cache1 = { 5, 3, 8, 6, 2, 7, 4, 1 };

	linearLocateMinQ<int>(cache1, 3, 0, cache1.size(), 1);
	std::cout << cache1[0] << std::endl; // Should print the 3rd smallest element, which is 3.

	// generate a random vector for testing.
	std::vector<int> cache2;

	std::random_device rd;
	std::mt19937 gen2(rd());
	std::uniform_int_distribution<int> prg2(0, 99);

	cache2.reserve(41);
	cache2.push_back(100);
	for (int i = 0; i < 20; ++i) {
		cache2.push_back(prg2(gen2));
		cache2.push_back(prg2(gen2) + 101);
	}

	linearLocateMinQ<int>(cache2, 21, 0, cache2.size(), 1);
	std::cout << cache2[0] << std::endl; // Should print the 21st smallest element, which is 100.


	// generate a long random vector for testing.
	std::vector<int> cache3;
	std::mt19937 gen3(rd());

	for (int i = 0; i < 2001; ++i) {
		cache3.push_back(i);
	}
	std::shuffle(cache3.begin(), cache3.end(), gen3);

	linearLocateMinQ<int>(cache3, 1001);
	std::cout << cache3[0] << std::endl; // Should print the 1001st smallest element, which is 1000.


	// generate a very long random vector for testing.
	std::vector<int> cache4;
	std::mt19937 gen4(rd());

	for (int i = 0; i < 40001; ++i) {
		cache4.push_back(i);
	}
	std::shuffle(cache4.begin(), cache4.end(), gen4);

	linearLocateMinQ<int>(cache4, 30001);
	std::cout << cache4[0] << std::endl; // Should print the 30001st smallest element, which is 30000.


	return 0;
}