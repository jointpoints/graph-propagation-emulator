#include "metric_graph.hpp"





rand_walks::MetricGraph::MetricGraph(void) :
	edges()
{
	// Intended to be empty
}





rand_walks::MetricGraph::~MetricGraph(void)
{
	// Intended to be empty
}





uint32_t const rand_walks::MetricGraph::getVertexCount(void)
{
	return this->edges.size();
}
