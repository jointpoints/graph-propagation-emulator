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
	// 1. Check if wander state is "invalid"
	if (this->wander_state != WanderState::invalid)
		throw std::logic_error("Wander object needs to be in invalid state in order to be reset.");

	// 2. Initialise the graph state
	this->graph_state = GraphState(graph.edges.size());
	for (uint32_t vertex_1 = 0; vertex_1 < this->graph_state.size(); ++vertex_1)
		this->graph_state[vertex_1] = NeighbourhoodState(graph.edges[vertex_1].adjacents.size(), EdgeState{AgentInstanceList(), false});
	
	// 3. Update wander state
	this->wander_state = WanderState::ready;

	return;
}



long double const rand_walks::Wander::run(uint32_t const start_vertex, long double const epsilon, long double const time_delta, Concurrency concurrency_type)
{
	auto            agent_comparator    = [](AgentInstance const &agent, long double const position){return agent.position < position;};
	long double     runtime             = 0.0L;
	bool            is_saturated        = false;

	// 1.1. Check if wander state is "dead"
	if (this->wander_state == WanderState::dead)
		throw std::logic_error("Wander object is dead.");
	// 1.2. Check if wander state is "ready"
	if (this->wander_state != WanderState::ready)
		throw std::logic_error("Wander object needs to be reset before running the simulation.");
	// 1.3. Check if <start_vertex> is valid
	if (!this->graph.checkVertex(start_vertex))
		throw std::invalid_argument("Vetrex " + std::to_string(start_vertex) + " does not exist in the specified graph.");
	
	// 2. Update wander state
	this->wander_state = WanderState::active;
	
	// 3. Place a single AgentInstance on each edge incident to the <start_vertex>
	for (uint32_t vertex_1_i = 0; vertex_1_i < this->graph_state.size(); ++vertex_1_i)
		for (uint32_t vertex_2_i = 0; vertex_2_i < this->graph_state[vertex_1_i].size(); ++vertex_2_i)
		{
			MetricGraph::VertexView curr_vertex = this->graph.edges[vertex_1_i];
			if (curr_vertex.id == start_vertex)
				this->graph_state[vertex_1_i][vertex_2_i].agents.push_back({0.0L, true});
			if ((curr_vertex.adjacents[vertex_2_i] == start_vertex) && (curr_vertex.is_directed[vertex_2_i] == false))
				this->graph_state[vertex_1_i][vertex_2_i].agents.push_back({curr_vertex.lengths[vertex_2_i], false});
		}
	
	// 4. Run simulation
	while (!is_saturated)
	{
		AgentCreationRequest requests;
		
		is_saturated = true;
		
		for (uint32_t vertex_1 = 0; vertex_1 < this->graph_state.size(); ++vertex_1)
		{
			for (uint32_t vertex_2 = 0; vertex_2 < this->graph_state[vertex_1].size(); ++vertex_2)
			{
				AgentCreationRequest curr_requests = this->updateEdgeState(vertex_1, vertex_2, epsilon, time_delta);
				requests.target_edges.insert(requests.target_edges.end(), curr_requests.target_edges.begin(), curr_requests.target_edges.end());
				requests.init_positions.insert(requests.init_positions.end(), curr_requests.init_positions.begin(), curr_requests.init_positions.end());
				requests.init_directions.insert(requests.init_directions.end(), curr_requests.init_directions.begin(), curr_requests.init_directions.end());
				is_saturated &= this->graph_state[vertex_1][vertex_2].is_saturated;
				/*std::cout << this->graph.edges[vertex_1].id << ' ' << this->graph.edges[vertex_1].adjacents[vertex_2] << '\n';
				for (uint32_t i = 0; i < this->graph_state[vertex_1][vertex_2].agents.size(); ++i)
					std::cout << this->graph_state[vertex_1][vertex_2].agents[i].position << ' ';
				std::cout << "\n---------------------------\n";*/
			}
		}
		
		while (!requests.target_edges.empty())
		{
			MetricGraph::Edge const &curr_edge = requests.target_edges.front();
			auto agent_insert_position = std::lower_bound(this->graph_state[curr_edge.first][curr_edge.second].agents.begin(), this->graph_state[curr_edge.first][curr_edge.second].agents.end(), requests.init_positions.front(), agent_comparator);
			
			this->graph_state[curr_edge.first][curr_edge.second].agents.insert(agent_insert_position, AgentInstance{requests.init_positions.front(), requests.init_directions.front()});
			requests.target_edges.pop_front();
			requests.init_positions.pop_front();
			requests.init_directions.pop_front();
		}

		runtime += time_delta;

		//std::cout << "Runtime ended: " << runtime << "\n================================\n";
	}

	return runtime;
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



rand_walks::Wander::AgentCreationRequest rand_walks::Wander::updateEdgeState(uint32_t const vertex_1, uint32_t const vertex_2, long double const epsilon, long double const time_delta)
{
	// 1.1. Check if wander state is "dead"
	if (this->wander_state == WanderState::dead)
		throw std::logic_error("Wander object is dead.");
	// 1.2. Check if wander state is "active"
	if (this->wander_state != WanderState::active)
		throw std::logic_error("Wander object needs to be reset before running the simulation.");
	
	AgentInstanceList       &agents             = this->graph_state[vertex_1][vertex_2].agents;
	long double const        length             = this->graph.edges[vertex_1].lengths[vertex_2];
	bool const               is_directed        = this->graph.edges[vertex_1].is_directed[vertex_2];
	bool                     is_saturated       = true;
	uint32_t                 agent_j;
	AgentCreationRequest     request;

	// 2. Update position of each AgentInstance while preserving their ascending order
	for (uint32_t agent_i = 0; agent_i < agents.size(); ++agent_i)
	{
		agents[agent_i].position = (agents[agent_i].direction) ? (agents[agent_i].position + time_delta) : (agents[agent_i].position - time_delta);

		// 2.1. If agent hit the vertex of the graph
		if ((agents[agent_i].position >= length) || (agents[agent_i].position <= 0))
		{
			uint32_t const                  hit_vertex          = (agents[agent_i].position <= 0) ? (this->graph.edges[vertex_1].id) : (this->graph.edges[vertex_1].adjacents[vertex_2]);
			long double const               delta_distance      = (agents[agent_i].position <= 0) ? (-agents[agent_i].position) : (std::fmod(agents[agent_i].position, length));
			std::deque<MetricGraph::Edge>   curr_departures     = this->graph.getDepartingEdges(hit_vertex);

			while (!curr_departures.empty())
			{
				if ((curr_departures.front().first != vertex_1) || (curr_departures.front().second != vertex_2))
				{
					request.init_positions.push_back((this->graph.edges[curr_departures.front().first].id == hit_vertex) ? (delta_distance) : (this->graph.edges[curr_departures.front().first].lengths[curr_departures.front().second] - delta_distance));
					request.init_directions.push_back(this->graph.edges[curr_departures.front().first].id == hit_vertex);
					request.target_edges.push_back(curr_departures.front());
				}
				curr_departures.pop_front();
			}

			if (is_directed)
			{
				agents.erase(agents.begin() + (agent_i--));
				continue;
			}
			agents[agent_i].position = (agents[agent_i].position <= 0) ? (delta_distance) : (length - delta_distance);
			agents[agent_i].direction = !agents[agent_i].direction;
		}

		// 2.2. Keep agents sorted
		for (agent_j = agent_i; (agent_j > 0) && (agents[agent_j].position < agents[agent_j - 1].position); --agent_j)
			std::swap(agents[agent_j], agents[agent_j - 1]);
		is_saturated &= (agent_j == 0) ? (  (agents.size() > 1) ? (agents[1].position - agents[0].position < 2 * epsilon) : (true)  ) : (agents[agent_j].position - agents[agent_j - 1].position < 2 * epsilon);
	}

	// 3. Final checks for epsilon-net
	is_saturated &= (agents.size() > 0) && (agents[0].position < epsilon) && (length - agents.back().position < epsilon);
	this->graph_state[vertex_1][vertex_2].is_saturated = is_saturated;

	return request;
}
