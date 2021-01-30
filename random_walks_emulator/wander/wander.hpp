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
		using EdgeState             = std::vector<AgentInstance>;
		using NeighbourhoodState    = std::vector<EdgeState>;
		using GraphState            = std::vector<NeighbourhoodState>;

		MetricGraph const   &graph;
		GraphState           graph_state;
		WanderState          wander_state;
	};





} // rand_walks





#endif
