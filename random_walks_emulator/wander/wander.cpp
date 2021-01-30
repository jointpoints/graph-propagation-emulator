/**
 * \file
 *       wander.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "wander.hpp"





// Constructors and destructors





rand_walks::Wander::Wander(MetricGraph const &graph) :
	graph(graph)
{
	// 1. Initialise the graph state
	this->state.resize(graph.edges.size());
	for (uint32_t vertex_1 = 0; vertex_1 < this->state.size(); ++vertex_1)
		this->state[vertex_1] = NeighbourhoodState(graph.edges[vertex_1].connected_vertices.size(), EdgeState());
}



rand_walks::Wander::~Wander(void)
{
	// Intended to be empty
}





// Modifiers





void rand_walks::Wander::reset(void)
{
	// 1. Initialise the graph state
	this->state = GraphState(graph.edges.size());
	for (uint32_t vertex_1 = 0; vertex_1 < this->state.size(); ++vertex_1)
		this->state[vertex_1] = NeighbourhoodState(graph.edges[vertex_1].connected_vertices.size(), EdgeState());
}
