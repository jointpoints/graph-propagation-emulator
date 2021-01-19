#include "metric_graph/metric_graph.hpp"

#include <iostream>





int main(void)
{
	rand_walks::MetricGraph graph;
	graph.addEdge(0, 1, 1.5);
	graph.addEdge(2, 1, 1.5);
	std::cout << graph.getVertexCount() << '\n';

	return 0;
}
