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
	my_wander.run(0, 0.5, 0.01);

	return 0;
}
