/**
 * \file
 *       rw_space.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "rw_space.hpp"

#include <stdexcept>    // needed for exceptions
#include <algorithm>    // needed for "lower_bound"
#include <cmath>        // needed for "fmod", "floor"
#include <utility>      // needed for "swap"
#include <thread>       // needed for "thread"
#include <queue>        // needed for "queue"





// Constructors and destructors





rwe::RWSpace::RWSpace(MetricGraph &graph) :
	graph(graph), wander_state(invalid)
{
	graph.associated_wanders.push_back(this);
	this->reset();
}



rwe::RWSpace::~RWSpace(void)
{
	for (uint32_t wander_i = 0; wander_i < this->graph.associated_wanders.size(); ++wander_i)
		if (this->graph.associated_wanders[wander_i] == this)
			this->graph.associated_wanders.erase(this->graph.associated_wanders.begin() + (wander_i--));
}





// Operators





rwe::RWSpace & rwe::RWSpace::operator=(rwe::RWSpace &&other)
{
	*this = std::move(other);

	return *this;
}





// Modifiers





void rwe::RWSpace::reset(void)
{
	// 1. Process the RWSpace object accordingly
	switch (this->wander_state)
	{
		case ready:
			break;
		case active:
			throw std::logic_error("Active RWSpace object cannot be reset. Invalidate it, if you want to interrupt the emulation.");
		case invalid:
			this->graph_state = GraphState(graph.edges.size());
			for (uint32_t vertex_1 = 0; vertex_1 < this->graph_state.size(); ++vertex_1)
				this->graph_state[vertex_1] = NeighbourhoodState(graph.edges[vertex_1].adjacents.size(), EdgeState{AgentInstanceList(), false});
			break;
		case dead:
			throw std::logic_error("RWSpace object is dead.");
	}

	// 2. Update wander state
	this->wander_state = WanderState::ready;

	return;
}



long double const rwe::RWSpace::run_saturation(uint32_t const start_vertex, long double const epsilon, long double const time_delta, bool const use_skip_forward, Concurrency concurrency_type)
{
	auto                                agent_comparator    = [](AgentInstance const &agent, long double const position){return agent.position < position;};
	long double                         runtime             = 0.0L;
	bool                                is_saturated        = false;
	std::priority_queue<long double, std::vector<long double>, std::greater<long double>>    skip_forward_timestamps;
	uint32_t const                      threads_count       = (concurrency_type == Concurrency::cpu) ? (std::thread::hardware_concurrency()) : (0);

	// 1.1. Check if wander state is "dead"
	if (this->wander_state == WanderState::dead)
		throw std::logic_error("RWSpace object is dead.");
	// 1.2. Check if wander state is "ready"
	if (this->wander_state != WanderState::ready)
		throw std::logic_error("RWSpace object needs to be reset before running the emulation.");
	// 1.3. Check if <start_vertex> is valid
	if (!this->graph.checkVertex(start_vertex))
		throw std::invalid_argument("Vetrex " + std::to_string(start_vertex) + " does not exist in the specified graph.");
	// 1.4. Check if concurrency is supported
	if ((threads_count < 3) && (concurrency_type == Concurrency::cpu))
		throw std::runtime_error("CPU concurrency is not supported on this platform.");
	
	// 2. Update wander state
	this->wander_state = WanderState::active;
	
	// 3. Place a single AgentInstance on each edge incident to the <start_vertex>
	for (uint32_t vertex_1_i = 0; vertex_1_i < this->graph_state.size(); ++vertex_1_i)
		for (uint32_t vertex_2_i = 0; vertex_2_i < this->graph_state[vertex_1_i].size(); ++vertex_2_i)
		{
			MetricGraph::VertexView curr_vertex = this->graph.edges[vertex_1_i];
			if (curr_vertex.id == start_vertex)
			{
				this->graph_state[vertex_1_i][vertex_2_i].agents.push_back({0.0L, true});
				if (use_skip_forward)
					skip_forward_timestamps.push(curr_vertex.lengths[vertex_2_i]);
			}
			if ((curr_vertex.adjacents[vertex_2_i] == start_vertex) && (curr_vertex.is_directed[vertex_2_i] == false))
			{
				this->graph_state[vertex_1_i][vertex_2_i].agents.push_back({curr_vertex.lengths[vertex_2_i], false});
				if (use_skip_forward)
					skip_forward_timestamps.push(curr_vertex.lengths[vertex_2_i]);
			}
		}
	
	// 4. Run simulation
	// 4.1. Use "skip forward", if it is allowed
	while (use_skip_forward)
	{
		EdgeUpdateResult update_results;
		is_saturated = true;

		// Check if current state satisfies the necessary condition
		for (uint32_t vertex_1 = 0; (is_saturated) && (vertex_1 < this->graph_state.size()); ++vertex_1)
			for (uint32_t vertex_2 = 0; (is_saturated) && (vertex_2 < this->graph_state[vertex_1].size()); ++vertex_2)
				is_saturated &= (this->graph_state[vertex_1][vertex_2].agents.size() >= floor(this->graph.edges[vertex_1].lengths[vertex_2] / (2 * epsilon) + 1));
		if (is_saturated)
			break;
		
		for (uint32_t vertex_1 = 0; vertex_1 < this->graph_state.size(); ++vertex_1)
		{
			for (uint32_t vertex_2 = 0; vertex_2 < this->graph_state[vertex_1].size(); ++vertex_2)
			{
				EdgeUpdateResult curr_results = this->updateEdgeState(vertex_1, vertex_2, epsilon, skip_forward_timestamps.top() - runtime);
				if (curr_results.collision_occured)
				{
					update_results.target_edges.insert(update_results.target_edges.end(), curr_results.target_edges.begin(), curr_results.target_edges.end());
					update_results.init_positions.insert(update_results.init_positions.end(), curr_results.init_positions.begin(), curr_results.init_positions.end());
					update_results.init_directions.insert(update_results.init_directions.end(), curr_results.init_directions.begin(), curr_results.init_directions.end());
					
					skip_forward_timestamps.push(skip_forward_timestamps.top() + this->graph.edges[vertex_1].lengths[vertex_2]);
				}
				/*std::cout << this->graph.edges[vertex_1].id << ' ' << this->graph.edges[vertex_1].adjacents[vertex_2] << '\n';
				for (uint32_t i = 0; i < this->graph_state[vertex_1][vertex_2].agents.size(); ++i)
					std::cout << this->graph_state[vertex_1][vertex_2].agents[i].position << ' ';
				std::cout << "\n---------------------------\n";*/
			}
		}
		
		while (!update_results.target_edges.empty())
		{
			MetricGraph::Edge const &curr_edge = update_results.target_edges.front();
			auto agent_insert_position = std::lower_bound(this->graph_state[curr_edge.first][curr_edge.second].agents.begin(), this->graph_state[curr_edge.first][curr_edge.second].agents.end(), update_results.init_positions.front(), agent_comparator);
			
			this->graph_state[curr_edge.first][curr_edge.second].agents.insert(agent_insert_position, AgentInstance{update_results.init_positions.front(), update_results.init_directions.front()});

			skip_forward_timestamps.push(skip_forward_timestamps.top() + this->graph.edges[curr_edge.first].lengths[curr_edge.second]);

			update_results.target_edges.pop_front();
			update_results.init_positions.pop_front();
			update_results.init_directions.pop_front();
		}

		runtime = skip_forward_timestamps.top();
		skip_forward_timestamps.pop();
		
		//std::cout << "Runtime ended: " << runtime << ' ' << skip_forward_timestamps.top() << "\n================================\n";
	}
	// 4.2. Precise emulation
	is_saturated = false;
	while (!is_saturated)
	{
		EdgeUpdateResult update_results;
		is_saturated = true;
		
		for (uint32_t vertex_1 = 0; vertex_1 < this->graph_state.size(); ++vertex_1)
		{
			for (uint32_t vertex_2 = 0; vertex_2 < this->graph_state[vertex_1].size(); ++vertex_2)
			{
				EdgeUpdateResult curr_results = this->updateEdgeState(vertex_1, vertex_2, epsilon, time_delta);
				update_results.target_edges.insert(update_results.target_edges.end(), curr_results.target_edges.begin(), curr_results.target_edges.end());
				update_results.init_positions.insert(update_results.init_positions.end(), curr_results.init_positions.begin(), curr_results.init_positions.end());
				update_results.init_directions.insert(update_results.init_directions.end(), curr_results.init_directions.begin(), curr_results.init_directions.end());
				is_saturated &= this->graph_state[vertex_1][vertex_2].is_saturated;
				/*std::cout << this->graph.edges[vertex_1].id << ' ' << this->graph.edges[vertex_1].adjacents[vertex_2] << '\n';
				for (uint32_t i = 0; i < this->graph_state[vertex_1][vertex_2].agents.size(); ++i)
					std::cout << this->graph_state[vertex_1][vertex_2].agents[i].position << ' ';
				std::cout << "\n---------------------------\n";*/
			}
		}
		
		while (!update_results.target_edges.empty())
		{
			MetricGraph::Edge const &curr_edge = update_results.target_edges.front();
			auto agent_insert_position = std::lower_bound(this->graph_state[curr_edge.first][curr_edge.second].agents.begin(), this->graph_state[curr_edge.first][curr_edge.second].agents.end(), update_results.init_positions.front(), agent_comparator);
			
			this->graph_state[curr_edge.first][curr_edge.second].agents.insert(agent_insert_position, AgentInstance{update_results.init_positions.front(), update_results.init_directions.front()});
			update_results.target_edges.pop_front();
			update_results.init_positions.pop_front();
			update_results.init_directions.pop_front();
		}

		runtime += time_delta;

		//std::cout << "Runtime ended: " << runtime << "\n================================\n";
	}

	// 5. Update wander state
	this->wander_state = WanderState::invalid;

	return runtime;
}



void rwe::RWSpace::invalidate(void)
{
	if (this->wander_state != WanderState::dead)
		this->wander_state = WanderState::invalid;

	return;
}



void rwe::RWSpace::kill(void)
{
	this->wander_state = WanderState::dead;

	return;
}



rwe::RWSpace::EdgeUpdateResult rwe::RWSpace::updateEdgeState(uint32_t const vertex_1, uint32_t const vertex_2, long double const epsilon, long double const time_delta)
{
	// 1.1. Check if wander state is "dead"
	if (this->wander_state == WanderState::dead)
		throw std::logic_error("RWSpace object is dead.");
	// 1.2. Check if wander state is "active"
	if (this->wander_state != WanderState::active)
		throw std::logic_error("RWSpace object needs to be reset before running the emulation.");
	
	AgentInstanceList       &agents             = this->graph_state[vertex_1][vertex_2].agents;
	long double const        length             = this->graph.edges[vertex_1].lengths[vertex_2];
	bool const               is_directed        = this->graph.edges[vertex_1].is_directed[vertex_2];
	bool                     is_saturated       = true;
	uint32_t                 agent_j;
	EdgeUpdateResult         result;

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

			result.collision_occured = true;
			while (!curr_departures.empty())
			{
				if ((curr_departures.front().first != vertex_1) || (curr_departures.front().second != vertex_2))
				{
					result.init_positions.push_back((this->graph.edges[curr_departures.front().first].id == hit_vertex) ? (delta_distance) : (this->graph.edges[curr_departures.front().first].lengths[curr_departures.front().second] - delta_distance));
					result.init_directions.push_back(this->graph.edges[curr_departures.front().first].id == hit_vertex);
					result.target_edges.push_back(curr_departures.front());
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

	return result;
}
