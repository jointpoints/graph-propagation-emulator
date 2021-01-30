/**
 * \file
 *       wander.hpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#ifndef __WANDER_HPP__
#define __WANDER_HPP__





#include "../metric_graph/metric_graph.hpp"





namespace rand_walks
{
	class Wander
	{
	public:
		using Concurrency = enum uint8_t {none, cpu};

		// Constructors and destructors
		Wander(MetricGraph const &graph);
		~Wander(void);

		// Modifiers
		void reset(void);
	private:
		using AgentInstance         = struct {long double position; bool direction : 1;};
		using EdgeState             = std::vector<AgentInstance>;
		using NeighbourhoodState    = std::vector<EdgeState>;
		using GraphState            = std::vector<NeighbourhoodState>;

		MetricGraph const   &graph;
		GraphState           state;
	};
} // rand_walks





#endif
