/**
 * \file
 *       wander.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "wander.hpp"





// Constructors and destructors





rand_walks::Wander::Wander(MetricGraph const &graph, WanderInfo const &wander_info, uint32_t const start_vertex) :
	graph(graph), info(wander_info)
{
	// Intended to be empty
}



rand_walks::Wander::~Wander(void)
{
	// Intended to be empty
}
