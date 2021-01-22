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
	typedef enum uint8_t
	{
		none,
		cpu
	}
	Concurrency;



	typedef struct
	{
		long double const   time_delta;
		Concurrency const   mode;
	}
	WanderInfo;



	class Wander
	{
	public:
		// Constructors and destructors
		Wander(MetricGraph const &, WanderInfo const &, uint32_t const);
		~Wander(void);
	private:
		MetricGraph const   &graph;
		WanderInfo const    &info;
	};
} // rand_walks





#endif
