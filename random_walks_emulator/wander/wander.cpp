/**
 * \file
 *       wander.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "wander.hpp"

#include <stdexcept>    // needed for exceptions
#include <algorithm>    // needed for "lower_bound"





// Constructors and destructors





rand_walks::Wander::Wander(MetricGraph const &graph) :
	graph(graph)
{
	this->reset();
}



rand_walks::Wander::~Wander(void)
{
	// Intended to be empty
}





// Modifiers





void rand_walks::Wander::reset(void)
{
	// 1. Initialise the graph state
	this->graph_state = GraphState(graph.edges.size());
	for (uint32_t vertex_1 = 0; vertex_1 < this->graph_state.size(); ++vertex_1)
		this->graph_state[vertex_1] = NeighbourhoodState(graph.edges[vertex_1].connected_vertices.size(), EdgeState());
	
	// 2. Update wander state
	this->wander_state = WanderState::ready;

	return;
}



long double const rand_walks::Wander::run(uint32_t const start_vertex, long double const epsilon, long double const time_delta, Concurrency concurrency_type)
{
	// 1.1. Check if wander state is "ready"
	if (this->wander_state != WanderState::ready)
		throw std::logic_error("Wander object needs to be reset before running the simulation.");
	// 1.2. Check if <start_vertex> is valid
	if (!this->graph.checkVertex(start_vertex))
		throw std::invalid_argument("Vetrex " + std::to_string(start_vertex) + " does not exist in the specified graph.");
	
	// 2. Update wander state
	this->wander_state = WanderState::active;
	
	// 3. Place a single AgentInstance on each edge incident to the <start_vertex>
	for (uint32_t vertex_1_i = 0; vertex_1_i < this->graph_state.size(); ++vertex_1_i)
		for (uint32_t vertex_2_i = 0; vertex_2_i < this->graph_state[vertex_1_i].size(); ++vertex_2_i)
		{
			MetricGraph::VertexNeighbourhood curr_neighbourhood = this->graph.edges[vertex_1_i];
			if (curr_neighbourhood.vertex_id == start_vertex)
				this->graph_state[vertex_1_i][vertex_2_i].push_back({0.0L, true});
			if ((curr_neighbourhood.connected_vertices[vertex_2_i] == start_vertex) && (curr_neighbourhood.is_directed[vertex_2_i] == false))
				this->graph_state[vertex_1_i][vertex_2_i].push_back({curr_neighbourhood.lengths[vertex_2_i], false});
		}
}



void rand_walks::Wander::invalidate(void)
{
	this->wander_state = WanderState::invalid;

	return;
}



void rand_walks::Wander::kill(void)
{
	this->wander_state = WanderState::dead;

	return;
}
