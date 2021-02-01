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
#include <cmath>        // needed for "fmod"
#include <utility>      // needed for "swap"
#include <iostream>     // DEBUG!!!!!





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
		this->graph_state[vertex_1] = NeighbourhoodState(graph.edges[vertex_1].connected_vertices.size(), EdgeState{AgentInstanceList(), false});
	
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
				this->graph_state[vertex_1_i][vertex_2_i].agents.push_back({0.0L, true});
			if ((curr_neighbourhood.connected_vertices[vertex_2_i] == start_vertex) && (curr_neighbourhood.is_directed[vertex_2_i] == false))
				this->graph_state[vertex_1_i][vertex_2_i].agents.push_back({curr_neighbourhood.lengths[vertex_2_i], false});
		}
	
	// 4. Run simulation
	while (true)
	{
		for (uint32_t vertex_1 = 0; vertex_1 < this->graph_state.size(); ++vertex_1)
			for (uint32_t vertex_2 = 0; vertex_2 < this->graph_state[vertex_1].size(); ++vertex_2)
				this->updateEdgeState(vertex_1, vertex_2, epsilon, time_delta);
	}

	return 0.0L;
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



bool const rand_walks::Wander::updateEdgeState(uint32_t const vertex_1, uint32_t const vertex_2, long double const epsilon, long double const time_delta)
{
	AgentInstanceList   &agents             = this->graph_state[vertex_1][vertex_2].agents;
	long double const    length             = this->graph.edges[vertex_1].lengths[vertex_2];
	bool const           is_directed        = this->graph.edges[vertex_1].is_directed[vertex_2];
	bool                 is_saturated       = true;
	bool                 should_reverse;
	uint32_t             agent_j;

	// 1. Update position of each AgentInstance while preserving their ascending order
	for (uint32_t agent_i = 0; agent_i < agents.size(); ++agent_i)
	{
		agents[agent_i].position = (agents[agent_i].direction) ? (agents[agent_i].position + time_delta) : (agents[agent_i].position - time_delta);
		should_reverse = (agents[agent_i].position > length) || (agents[agent_i].position < 0);
		agents[agent_i].position = (agents[agent_i].position < 0) ? (-agents[agent_i].position) : (  (agents[agent_i].position > length) ? (length - std::fmod(agents[agent_i].position, length)) : (agents[agent_i].position)  );
		agents[agent_i].direction = (should_reverse) ? (!agents[agent_i].direction) : (agents[agent_i].direction);
		if ((is_directed) && (!agents[agent_i].direction))
		{
			agents.erase(agents.begin() + (agent_i--));
			continue;
		}
		for (agent_j = agent_i; (agent_j > 0) && (agents[agent_j].position < agents[agent_j - 1].position); --agent_j)
			std::swap(agents[agent_j], agents[agent_j - 1]);
		is_saturated &= (agent_j == 0) || (agents[agent_j].position - agents[agent_j - 1].position < 2 * epsilon);
	}

	std::cout << agents[0].position << '\n';

	// 2. Final checks for epsilon-net
	is_saturated &= (agents.size() > 0) && (agents[0].position < epsilon) && (length - agents.back().position < epsilon);
	this->graph_state[vertex_1][vertex_2].is_saturated = is_saturated;

	return is_saturated;
}
