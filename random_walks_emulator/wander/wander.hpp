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
		using Concurrency = enum ConcurrencyEnum {none, cpu};
		using WanderState = enum WanderStateEnum {ready, active, invalid, dead};

		// Constructors and destructors
		Wander(MetricGraph const &graph);
		~Wander(void);

		// Modifiers
		void                reset       (void);
		long double const   run         (uint32_t const start_vertex, long double const epsilon, long double const time_delta = 1e-6L, Concurrency concurrency_type = none);
		void                invalidate  (void);
		void                kill        (void);
	private:
		using AgentInstance         = struct {long double position; bool direction : 1;};
		using AgentInstanceList     = std::vector<AgentInstance>;
		using EdgeState             = struct {AgentInstanceList agents; bool is_saturated : 1;};
		using NeighbourhoodState    = std::vector<EdgeState>;
		using GraphState            = std::vector<NeighbourhoodState>;

		MetricGraph const   &graph;
		GraphState           graph_state;
		WanderState          wander_state;

		// Modifiers
		bool const updateEdgeState(uint32_t vertex_1, uint32_t vertex_2, long double const epsilon, long double const time_delta);
	};





} // rand_walks





#endif
