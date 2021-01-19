#ifndef __METRIC_GRAPH_HPP__
#define __METRIC_GRAPH_HPP__





#include <vector>   // needed for "vector"
#include <utility>  // needed for "pair"
#include <cstdint>  // needed for "int*_t" and "uint*_t" types





namespace rand_walks
{
	class MetricGraph
	{
	public:
		MetricGraph(void);
		~MetricGraph(void);

		uint32_t const getVertexCount(void);
	private:
		using VertexList            = std::vector<uint32_t>;
		using VertexNeighbourhood   = std::pair<uint32_t, VertexList>;
		using EdgeList              = std::vector<VertexNeighbourhood>;

		EdgeList    edges;
	};
} // rand_walks





#endif // __METRIC_GRAPH_HPP__
