/**
 * @file propagation.cpp
 * @author JointPoints, 2021, github.com/jointpoints
 */
#include "propagation.hpp"
#include <iostream>





// Util





/**
 * @struct Edge
 * @brief Represents a single unique edge of graph
 *
 * Used in propagation emulation for better concurrency.
 */
using Edge = struct Edge
{
	size_t source;
	size_t target;
	bool is_directed;
	long double length;
	Edge(void) :
		source(0), target(0), is_directed(false), length(0.0L){};
	Edge(size_t const source, gpe::Graph::Edge const &graph_edge) :
		source(source), target(graph_edge.target), is_directed(true), length(graph_edge.length){};
	inline bool const operator==(Edge const &other)
	{
		return ((this->source == other.source) && (this->target == other.target)) || ((this->source == other.target) && (this->target == other.source));
	};
};

/**
 * @struct EdgeList
 * @brief Vector of all edges of graph
 */
using EdgeList = std::vector<Edge>;





/**
 * @brief Extract unique edges
 *
 * Extracts a vector of edges with different incident vertices from a given graph.
 */
EdgeList extract_unique_edges(gpe::Graph const &graph)
{
	EdgeList answer(graph.get_edge_count(), Edge());
	EdgeList::iterator curr_end = answer.begin();

	for (size_t vertex_i = 0; vertex_i < graph.get_vertex_count(); ++vertex_i)
	{
		std::vector<gpe::Graph::Edge> graph_edges = graph.get_out_edges(vertex_i);

		for (auto graph_edge = graph_edges.begin(); graph_edge < graph_edges.end(); ++graph_edge)
		{
			Edge edge(vertex_i, *graph_edge);
			auto present_edge = std::find(answer.begin(), curr_end, edge);
			if (present_edge != curr_end)
				present_edge->is_directed = false;
			else
			{
				*curr_end = edge;
				++curr_end;
			}
		}
	}

	return answer;
}





/**
 * @brief Consecutive propagation emulation
 *
 * Propagation emulation for the first \f$\varepsilon\f$-saturation moment calculation
 * without the use of concurrency.
 */
void first_saturation_consecutive(EdgeList const &edge_list, size_t const start_vertex_i, long double const epsilon, gpe::FirstSaturationInfo const &info)
{
	// ...
}





// API functions





void gpe::first_saturation(gpe::Graph const &graph, std::string const start_vertex_id, long double const epsilon, gpe::FirstSaturationInfo const &info)
{
	// 1. Extract unique edges from the graph
	EdgeList edge_list = extract_unique_edges(graph);

	// 2. Call different emulation functions depending on availability of concurrency
	if (info.use_concurrency)
		return;
	else
		return;

	return;
}
