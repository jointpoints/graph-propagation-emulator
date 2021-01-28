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
	/*graph.updateEdge(1, 2, 1.0, true);
	graph.outputEdgeList(std::cout);
	std::cout << "--------------------\n";
	graph.updateEdge(1, 0, 2.0, false);
	graph.outputEdgeList(std::cout);
	std::cout << "--------------------\n";
	graph.updateEdge(2, 1, 1.4142135623730951, true);
	graph.outputEdgeList(std::cout);
	std::cout << "--------------------\n";
	graph.updateEdge(0, 1, 1.0, true);
	graph.outputEdgeList(std::cout);
	std::cout << "--------------------\n";
	graph.toFile();*/
	graph.fromFile("Saved files/Sample graph 1");
	graph.outputEdgeList(std::cout);

	return 0;
}
