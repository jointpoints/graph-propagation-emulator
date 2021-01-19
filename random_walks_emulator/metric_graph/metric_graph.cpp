#include "metric_graph.hpp"

#include <algorithm>    // needed for "find_if", "find"





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





// Modifiers





bool rand_walks::MetricGraph::addEdge(uint32_t out_vertex, uint32_t in_vertex, long double length, bool is_directed)
{
	auto out_vertex_neighbourhood = std::find_if(this->edges.begin(), this->edges.end(), [out_vertex](VertexNeighbourhood neighbourhood){return neighbourhood.vertex_id == out_vertex;});
	if (out_vertex_neighbourhood != this->edges.end())
	{
		auto in_vertex_iterator = std::find(out_vertex_neighbourhood->connected_vertices.begin(), out_vertex_neighbourhood->connected_vertices.end(), in_vertex);
		if (in_vertex_iterator != out_vertex_neighbourhood->connected_vertices.end())
			out_vertex_neighbourhood->lengths[std::distance(out_vertex_neighbourhood->connected_vertices.begin(), in_vertex_iterator)] = length;
		else
		{
			out_vertex_neighbourhood->connected_vertices.push_back(in_vertex);
			out_vertex_neighbourhood->lengths.push_back(length);
		}
	}
	else
		this->edges.push_back({out_vertex, VertexList({in_vertex}), LengthList({length})});
	if (!is_directed)
		this->addEdge(in_vertex, out_vertex, length, true);
	return true;
}
