/**
 * \file
 *       main.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "wander/wander.hpp"

#include <iostream>





int main(void)
{
	rand_walks::MetricGraph graph;
	graph.fromFile("Saved files/Sample graph 1");
	graph.outputEdgeList(std::cout);

	rand_walks::Wander my_wander(graph);
	std::cout << "Epsilon = 0.1: " << my_wander.run(0, 0.1) << '\n';
	my_wander.reset();
	std::cout << "Epsilon = 0.5: " << my_wander.run(0, 0.5) << '\n';
	my_wander.reset();
	std::cout << "Epsilon = 0.9: " << my_wander.run(0, 0.9) << '\n';

	return 0;
}
