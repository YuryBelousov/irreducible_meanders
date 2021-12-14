#pragma once
#include <unordered_set>
#include <vector>
#include <cmath>
#include <algorithm>

using namespace std;

int cur_n = 0;
const int tail_point = -1;			//Special symbol for the end of submeander
const int max_n = 50;				//Maximum meander size

//Check if min(a1, a2) < x < max(a1,a2)
inline bool is_between(int x, int a1, int a2) { 
	if (a1 > a2) swap(a1, a2);
	return (a1 < x) && (x < a2);
}

class submeander {
private:
	//Meander code
	int code[max_n];	//Meander code
	int code_size;		//Size of code

	int tail_index;		//Number of last visired point
	int n;				//Crossing point with maximum number

	bool next_point_odd;					//Parity of next point
	unordered_set<int> not_visited_even;	//Point we have not visited
	unordered_set<int> not_visited_odd;		//Point we have not visited

	int upper_areas[max_n];					//Array that describe upper plane splitting
	int lower_areas[max_n];					//Array that describe lower plane splitting
	int areas_hight[max_n + 2];				//"Height" of arcs

	int edges[(max_n + 2)*(max_n + 3)];		//edges[i*max_n +j] is number of edges from upper_areas[i] to lower_areas[j]
	int verticies[max_n + 3];				//Array with incidence indeces of areas
	int edge_num;							//Total number of different edges
	int vert_num;							//Total number of verticies with non zero incidence index

private:
	void remove_edge(int from, int to) {
		if (--edges[from * max_n + to] == 0) {
			--edge_num;
			if (--verticies[from] == 0)
				--vert_num;
			if (--verticies[to] == 0)
				--vert_num;
		}
	}

	void add_edge(int from, int to) {
		if (++edges[from * max_n + to] == 1) {
			++edge_num;
			if (++verticies[from] == 1)
				++vert_num;
			if (++verticies[to] == 1)
				++vert_num;
		}
	}

	//Check if interval from i1 to i2 contains all points
	bool is_closed(int i1, int i2) const {
		int min_ = max_n, max_ = -max_n;
		for (int i = i1; i <= i2; ++i) {
			if (code[i] < min_)
				min_ = code[i];
			if (code[i] > max_)
				max_ = code[i];
		}
		return (max_ - min_ == i2 - i1);
	}

public:
	//Create an empty submeander with n possible crossings 
	submeander() : n(0), next_point_odd(false), tail_index(tail_point), code_size(0), edge_num(1), vert_num(2){
		//Create upper and lower half-plane
		for (int i = 0; i < cur_n; ++i) {
			if (i % 2 == 0)
				not_visited_even.insert(i);
			else
				not_visited_odd.insert(i);
			upper_areas[i] = 0;
			lower_areas[i] = 1;
		}
		areas_hight[0] = areas_hight[1] = cur_n + 3;
		verticies[upper_areas[0]] = verticies[lower_areas[0]] = 1;
		edges[max_n*upper_areas[0] + lower_areas[0]] = cur_n + 2;
	}

	//Create submeander based on permutation
	submeander(const std::vector<int>& perm) : submeander() {
		for (int pt : perm)
			go_throught_point(pt);
	}

	//Create submeander that continue submeander parent passing throught new_point
	submeander(const submeander& parent, int new_point) : n(parent.n), code_size(parent.code_size),
		tail_index(parent.tail_index), next_point_odd(parent.next_point_odd),
		not_visited_odd(parent.not_visited_odd), not_visited_even(parent.not_visited_even),
		edge_num(parent.edge_num), vert_num(parent.vert_num)	{
		//Copy arrays
		memcpy(code, parent.code, sizeof(int)*parent.code_size);
		memcpy(upper_areas, parent.upper_areas, sizeof(int)*(cur_n));
		memcpy(lower_areas, parent.lower_areas, sizeof(int)*(cur_n));
		memcpy(areas_hight, parent.areas_hight, sizeof(int)*(cur_n + 2));

		memcpy(edges, parent.edges, sizeof(int)*((max_n + 2)*(max_n + 3)));
		memcpy(verticies, parent.verticies, sizeof(int)*(cur_n + 3));

		go_throught_point(new_point);
	}

	//Change submeander passing throught point new_point
	void go_throught_point(int new_point) noexcept {
		n = max(n, new_point + 1);
		//Choose half plane that will change
		int* half_plane = next_point_odd ? lower_areas : upper_areas;
		unordered_set<int>* half_plane_points = next_point_odd ? &not_visited_odd : &not_visited_even;
		//Add new area
		int new_area_height = abs(tail_index - new_point);
		areas_hight[code_size + 2] = new_area_height;
		//Remove an edge in new_point
		remove_edge(upper_areas[new_point], lower_areas[new_point]);
		//Points between now lead to new area
		for (int i = min(tail_index, new_point) + 1; i < max(tail_index, new_point); ++i)
			if (areas_hight[half_plane[i]] > new_area_height) {
				if (i % 2 == 0 && not_visited_even.find(i) != not_visited_even.end()
					|| i % 2 == 1 && not_visited_odd.find(i) != not_visited_odd.end()) {
					remove_edge(upper_areas[i], lower_areas[i]);
					if (next_point_odd)
						add_edge(upper_areas[i], code_size + 2);
					else
						add_edge(code_size + 2, lower_areas[i]);
				}
				half_plane[i] = code_size + 2;
			}
		//Delete new_point from not_visited unordered_set
		half_plane_points->erase(new_point);
		//Change other info
		tail_index = new_point;
		next_point_odd = !next_point_odd;
		code[code_size] = new_point;
		++code_size;
	}

	//Delete last visited point from submeander
	void step_back() {
		if (code_size < 2)
			tail_index = -1;
		else
			tail_index = code[code_size - 2];
		if (n == code[code_size - 1] + 1) {
			n = -1;
			for (size_t i = 0; i < code_size - 1; ++i)
				n = max(n, code[i] + 1);
		}
		if (code_size % 2 == 0)
			not_visited_odd.insert(code[code_size - 1]);
		else
			not_visited_even.insert(code[code_size - 1]);
		int* half_plane = next_point_odd ? upper_areas : lower_areas;
		int prev_area = half_plane[code[code_size - 1]];
		for (int i = min(tail_index, code[code_size - 1]); i < max(tail_index, code[code_size - 1]); ++i) 
			if (half_plane[i] == code_size + 1) {
				half_plane[i] = prev_area;
				if (i % 2 == 0 && not_visited_even.find(i) != not_visited_even.end()
					|| i % 2 == 1 && not_visited_odd.find(i) != not_visited_odd.end()) {
					add_edge(upper_areas[i], lower_areas[i]);
					if (next_point_odd)
						remove_edge(code_size + 1, lower_areas[i]);
					else
						remove_edge(upper_areas[i], code_size + 1); 
				}
			}
		next_point_odd = !next_point_odd;
		--code_size;
		add_edge(upper_areas[code[code_size]], lower_areas[code[code_size]]);		
	}

	//Find all points, that are available for crossing
	vector<int> available_points(bool irreducibility = true) noexcept {
		if (vert_num != edge_num + 1)
			return {};
		//Choose half plane where we a looking for avaliable points
		int* half_plane = next_point_odd ? lower_areas : upper_areas;
		unordered_set<int>* not_visited_points = next_point_odd ? &not_visited_odd : &not_visited_even;

		vector <int> res;
		//Number of tail area
		int tail_area = (tail_index == tail_point) ? 0 : half_plane[tail_index];
		for (auto p : *not_visited_points)
			if ((half_plane[p] == tail_area) && (!irreducibility || irreducibility_save(p)))
				res.push_back(p);
		return res;
	}


	bool irreducibility_save(int pt) noexcept {
		/*
		First check if adding new point don't break irreducibility.
		We start from the end of code and try to find sequence {a_i} of length k (k>2) with the property that max{a_i}-min{a_i}=k.
		*/
		//If code is to small or first point is 0
		if (code_size < 2)
			return true;
		if (code[0] == 0)
			return false;
		//We imitate adding pt to code 
		code[code_size] = pt;
		//++code_size;
		int new_n = max(n, pt + 1);
		//Start cheking
		int min_p = pt;
		int max_p = pt;
		for (int i = code_size - 1; i > -1; --i) {
			if (min_p > code[i]) 
				min_p = code[i];
			else if (max_p < code[i]) 
				max_p = code[i];
			if (code_size - i < 2)
				continue;
			if ((i == 0) && (code_size + 1 == new_n))
				break;
			if ((code_size - i) == max_p - min_p)
				return false;
		}
		return true;
	}

	//Check if it is an finished meander
	bool finished() const noexcept {
		if (code_size < n)
			return false;
		if (next_point_odd)
			return (lower_areas[tail_index] == 1);
		else
			return (upper_areas[tail_index] == 0);
	}


	//Comparison operator needed to store submeanders in set
	friend bool operator<(const submeander& m1, const submeander& m2) {
		if (m1.code_size != m2.code_size)
			return m1.code_size < m2.code_size;
		for (size_t i = 0; i < m1.code_size; ++i)
			if (m1.code[i] != m2.code[i])
				return m1.code[i] < m2.code[i];
		return false;
	}

	friend ostream& operator<<(ostream& out, const submeander& m) noexcept {
		for (size_t i = 0; i < m.code_size; ++i)
			out << m.code[i] + 1 << "; ";
		//out << m.code[m.code_size - 1] + 1;
		return out;
	}

	//Baisc get-functions
	int get_first_point() const noexcept {
		return code[0];
	}

	int get_n() const noexcept {
		return n;
	}

	int get_cups() const noexcept {
		int cups_number = 0;
		for (int i = 1; i < code_size; ++i)
			if (abs(code[i] - code[i - 1]) == 1)
				++cups_number;
		return cups_number;
	}

	int get_last_point() const noexcept {
		return code[code_size - 1];
	}

	int get_tail_area() const noexcept {
		if (next_point_odd)
			return lower_areas[tail_index];
		else
			return upper_areas[tail_index];
	}

	int operator[] (int i) const {
		return code[i];
	}
};