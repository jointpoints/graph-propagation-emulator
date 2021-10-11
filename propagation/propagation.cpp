/**
 * @file propagation.cpp
 * @author JointPoints, 2021, github.com/jointpoints
 */
#include "propagation.hpp"
#include <cmath>
#include <iostream>





// Util





/**
 * @class Edge
 * @brief Represents a single unique edge of graph
 *
 * Used in propagation emulation for better concurrency.
 */
class Edge final
{



public:

	size_t source;
	size_t target;
	bool is_directed;
	long double length;
	std::vector<long double> agents;

	/// @name Constructors & destructors
	/// @{

	/**
	 * @brief Default constructor
	 *
	 * Sets everything to zero.
	 */
	Edge(void) :
		source(0), target(0), is_directed(false), length(0.0L){};
	
	/**
	 * @brief From-graph constructor
	 *
	 * Constructs a copy of Graph edge suitable for concurrent computing.
	 * 
	 * @param source        The index of source vertex.
	 * @param graph_edge    gpe::Graph::Edge object containing all data
	 *                      about the edge.
	 */
	Edge(size_t const source, gpe::Graph::Edge const &graph_edge) :
		source(source), target(graph_edge.target), is_directed(true), length(graph_edge.length){};
	
	/**
	 * @brief Default destructor
	 *
	 * Deletes the edge.
	 */
	~Edge(void) = default;
	
	/// @}
	


	/// @name Operators
	/// @{

	/**
	 * @brief Equality operator
	 *
	 * Enables comparison of two edges. Edges are considered equal, if they have the same
	 * source an target vertices (not necessarily respectively).
	 * 
	 * @param other Another edge.
	 * 
	 * @return @c true if edges are equal, @c false otherwise.
	 */
	inline bool const operator==(Edge const &other)
	{
		return ((this->source == other.source) && (this->target == other.target)) || ((this->source == other.target) && (this->target == other.source));
	};

	///@}



	/// @name Modifiers
	/// @{

	/**
	 * @brief Adds new agent
	 *
	 * Adds new agent to one of the vertices incident to this edge (i.e., either
	 * @c source or @c target).
	 * 
	 * @param vertex_i Graph-local index of the vertex where a new agent needs to be
	 *                 placed.
	 * 
	 * @throw 
	 */
	void new_agent(size_t const vertex_i)
	{
		if (vertex_i == this->source)
			agents.insert(agents.begin(), 0.L);
		else
			if (vertex_i == this->target)
				agents.push_back(this->length);
			else
				throw 0;
		return;
	}

	/// @}



}; // class Edge

/**
 * Vector of all edges of graph
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
	auto a = [](EdgeList::const_iterator const edge_iter, long double const birthtime, long double const t)
	{
		return edge_iter->length * (1 - std::abs( 2 * std::fmod( (t - birthtime) / (2 * edge_iter->length), 1.L) - 1 ));
	};
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
