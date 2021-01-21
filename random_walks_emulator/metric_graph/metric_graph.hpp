/**
 * \file
 *       metric_graph.hpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#ifndef __METRIC_GRAPH_HPP__
#define __METRIC_GRAPH_HPP__





#include <vector>   // needed for "vector"
#include <cstdint>  // needed for "int*_t" and "uint*_t" types
#include <string>   // needed for "string"





namespace rand_walks
{
	class MetricGraph
	{
	public:
		// Constructors and destructors
		MetricGraph(void);
		~MetricGraph(void);

		// Capacity
		uint32_t const getVertexCount(void);

		// Modifiers
		void addEdge(uint32_t, uint32_t, long double, bool is_directed = false);

		// Save/load
		void toFile(std::string file_name = "Saved files/My metric graph");
		void fromFile(std::string);
	private:
		using VertexList            = std::vector<uint32_t>;
		using LengthList            = std::vector<long double>;
		using VertexNeighbourhood   = struct {uint32_t vertex_id; VertexList connected_vertices; LengthList lengths;};
		using EdgeList              = std::vector<VertexNeighbourhood>;

		EdgeList    edges;
	};
} // rand_walks





#endif // __METRIC_GRAPH_HPP__
