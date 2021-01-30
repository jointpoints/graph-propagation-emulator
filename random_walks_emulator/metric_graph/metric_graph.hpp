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
#include <iostream> // needed for "iostream"





namespace rand_walks
{
	class MetricGraph
	{
	public:
		// Constructors and destructors
		MetricGraph     (void);
		~MetricGraph    (void);

		// Access
		bool const          checkVertex     (uint32_t const vertex)                             const;
		uint32_t const      getVertexCount  (void)                                              const;
		long double const   getEdgeLength   (uint32_t const out_vertex, uint32_t in_vertex)     const;
		void                outputEdgeList  (std::ostream &output_stream)                       const;

		// Modifiers
		void updateEdge(uint32_t const out_vertex, uint32_t const in_vertex, long double const length, bool const is_directed = false);

		// Save/load
		void    toFile      (std::string const file_name = "Saved files/My metric graph")   const;
		void    fromFile    (std::string const file_name);
	private:
		friend class Wander;

		using VertexList            = std::vector<uint32_t>;
		using LengthList            = std::vector<long double>;
		using DirectionList         = std::vector<bool>;
		using VertexNeighbourhood   = struct {uint32_t vertex_id; VertexList connected_vertices; LengthList lengths; DirectionList is_directed;};
		using EdgeList              = std::vector<VertexNeighbourhood>;
		using Edge                  = std::pair<uint32_t, uint32_t>;

		EdgeList    edges;

		// Access
		Edge getEdge(uint32_t const out_vertex, uint32_t const in_vertex, bool const is_directed = true, bool const strict_mode = false) const;
	};
} // rand_walks





#endif // __METRIC_GRAPH_HPP__
