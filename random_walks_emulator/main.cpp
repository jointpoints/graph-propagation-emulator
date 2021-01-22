/**
 * \file
 *       main.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "wander/wander.hpp"

#include <iostream>





int main(void)
{
	rand_walks::MetricGraph graph;
	graph.fromFile("Saved files/Sample graph 1.rweg");

	rand_walks::WanderInfo wander_info{0.1, rand_walks::Concurrency::none};
	rand_walks::Wander wander(graph, wander_info, 0);

	return 0;
}
