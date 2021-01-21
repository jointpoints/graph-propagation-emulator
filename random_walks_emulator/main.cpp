/**
 * \file
 *       main.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "metric_graph/metric_graph.hpp"

#include <iostream>





int main(void)
{
	rand_walks::MetricGraph graph;
	graph.fromFile("Saved files/Sample graph 1.rweg");
	/*graph.addEdge(0, 1, 1.0);
	graph.addEdge(1, 2, 1.4142135623730950488016887242097);
	graph.toFile();*/
	std::cout << graph.getVertexCount() << '\n';

	return 0;
}
