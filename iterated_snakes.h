#pragma once
#include <vector>
#include <algorithm>

//Here is the file with functions to calculate the number of iterated snakes.

using big_int = long long;	//A custom class for long arithmetic can be used here.

const int max_N = 500;
big_int iterated_snakes[max_N];

//Calculate binomial coefficient
big_int binomial_coef(int n, int k) {
	std::vector<big_int> C(k + 1, 0);
	C[0] = 1;
	for (int i = 1; i <= n; i++) {
		for (int j = std::min(i, k); j > 0; j--)
			C[j] = C[j] + C[j - 1];
	}
	return C[k];
}

//Function that take a partition and turn it to the next partition (with respect to standart ordering)
//It returns false if given partition it the last one
bool get_next_partition(std::vector<int>& partition) {
	int max_i = -1;
	int n = partition.size();
	for (int i = 0; i < n; ++i) {
		if (partition[n - 1] - partition[i] >= 2)
			max_i = std::max(max_i, i);
	}
	if (max_i == -1)
		return false;
	int delta = 0;
	for (int i = n - 1; i >= max_i; --i) {
		delta += partition[i] - partition[max_i] - 1;
		partition[i] = partition[max_i] + 1;
	}
	partition[n - 1] += delta;
	return true;
}


//The main recursion function (it just repeat the formula from the paper)
big_int recursion(int n) {
	big_int res = 0;
	for (int root_weight = 2 + n % 2; root_weight < n; root_weight += 2)
		for (int number_of_children = 1; number_of_children <= root_weight && (n - root_weight) / 2 >= number_of_children; ++number_of_children) {
			std::vector<int> children_weights(number_of_children - 1, 1);
			children_weights.push_back((n - root_weight) / 2 - number_of_children + 1); // partition of the form (1, 1, ..., K) 
			do {
				big_int local_res = 1;
				int k = 0;
				int n_residual = root_weight;
				int cur_value = children_weights[0];
				for (int ind = 0; ind < number_of_children; ++ind) {
					if (cur_value == children_weights[ind])
						++k;
					else {
						local_res *= binomial_coef(n_residual, k);
						n_residual -= k;
						k = 1;
					}
					local_res *= iterated_snakes[2 * children_weights[ind]] / 2;
				}
				local_res *= binomial_coef(n_residual, k);
				res += local_res;
			} while (get_next_partition(children_weights));
		}
	iterated_snakes[n - 1] = (res + 1) * (1 + n % 2);
	return res;
}
