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
	graph.addEdge(1, 0, 1.0, false);
	graph.outputEdgeList(std::cout);
	graph.addEdge(0, 1, 2.0, true);
	graph.outputEdgeList(std::cout);

	return 0;
}
