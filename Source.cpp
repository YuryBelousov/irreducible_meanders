#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <thread>
#include <map>
#include <set>
#include <atomic>
#include <numeric>

#include "submeander.h"
#include "iterated_snakes.h"


atomic<long long> meander_number[max_n][max_n];


void brute_forse_step(submeander& m, int pt) {
	m.go_throught_point(pt);
	if (m.finished() && m.get_n() > 3) {
		int n = m.get_n() - 1;
		++meander_number[n][m.get_cups()];
		if (n > 0) { 
			m.step_back();
			return; 
		}
	}
	for (auto e : m.available_points())
		brute_forse_step(m, e);
	m.step_back();
}

void brute_forse(bool store_all_meanders = false) {
	int step = 0;
	int n = ceil(cur_n / 2.);
	submeander m[max_n];

	thread threads[max_n];
	for (int i = 1; i < n; ++i)
		threads[i] = thread(brute_forse_step, ref(m[i]), 2 * i);
	for (int i = 1; i < n; ++i)
		threads[i].join();
}


int main() {	
	cout << "Write number of intersection points from 1 to " << max_n << ": ";
	cin >> cur_n;
	ofstream file_with_mnd("irr.txt");
	brute_forse();
	for (int i = 0; i < cur_n; ++i)
		file_with_mnd << i + 1 << " " << meander_number[i] << std::endl;
	file_with_mnd.close();

	int iterated_snakes_n = 36;
	ofstream file_with_ni("iterated_snakes.txt");
	iterated_snakes[0] = iterated_snakes[1] = 1;
	for (int i = 3; i <= iterated_snakes_n; ++i)
		recursion(i);
	for (int i = 0; i < iterated_snakes_n; ++i)
		file_with_ni << iterated_snakes[i] << std::endl;
	file_with_ni.close();

	return 0;
}