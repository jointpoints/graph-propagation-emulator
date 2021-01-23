/**
 * \file
 *       metric_graph.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "metric_graph.hpp"

#include <algorithm>    // needed for "find_if", "find", "min", "max", "binary_search"
#include <fstream>      // needed for "fstream"





// Constructors and destructors





rand_walks::MetricGraph::MetricGraph(void) :
	edges()
{
	// Intended to be empty
}



rand_walks::MetricGraph::~MetricGraph(void)
{
	// Intended to be empty
}





// Access





uint32_t const rand_walks::MetricGraph::getVertexCount(void) const
{
	return 0; // TODO
}



long double const rand_walks::MetricGraph::getEdgeLength(uint32_t const out_vertex, uint32_t in_vertex) const
{
	/*auto out_comparator     = [](VertexNeighbourhood const curr_neighbourhood, uint32_t const value){return curr_neighbourhood.vertex_id < value;};

	// 1. Try to find an indirected edge between <out_vertex> and <in_vertex>
	uint32_t const  out_vertex_new      = std::min(out_vertex, in_vertex);
	uint32_t const  in_vertex_new       = std::max(out_vertex, in_vertex);
	auto            out_lower_bound     = std::lower_bound(this->edges.begin(), this->edges.end(), out_vertex_new, out_comparator);

	if ((out_lower_bound != this->edges.end()) && (out_lower_bound->vertex_id == out_vertex_new))
	{
		auto in_lower_bound = std::lower_bound(out_lower_bound->connected_vertices.begin(), out_lower_bound->connected_vertices.end(), in_vertex_new);

		if ((in_lower_bound != out_lower_bound->connected_vertices.end()) && (*in_lower_bound == in_vertex_new))
			if ((out_lower_bound->is_directed[std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound)] == false) || (out_vertex == out_vertex_new))
				return out_lower_bound->lengths[std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound)];
	}

	// 2. If previous attempt is unsuccessful, try to find a directed edge between <out_vertex> and <in_vertex>
	out_lower_bound = std::lower_bound(this->edges.begin(), this->edges.end(), out_vertex, out_comparator);

	if ((out_lower_bound != this->edges.end()) && (out_lower_bound->vertex_id == out_vertex))
	{
		auto in_lower_bound = std::lower_bound(out_lower_bound->connected_vertices.begin(), out_lower_bound->connected_vertices.end(), in_vertex);

		if ((in_lower_bound != out_lower_bound->connected_vertices.end()) && (*in_lower_bound == in_vertex))
			return out_lower_bound->lengths[std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound)];
	}*/
	// 1. Try to find corresponding edge
	EdgeSpecifier edge = this->getEdge(out_vertex, in_vertex);

	if (edge.first != this->edges.end())
		return edge.first->lengths[edge.second];

	// 2. If both attempts are unsuccessful, there is no edge between <out_vertex> and <in_vertex>
	return std::numeric_limits<long double const>::infinity();
}



rand_walks::MetricGraph::EdgeSpecifier rand_walks::MetricGraph::getEdge(uint32_t const out_vertex, uint32_t const in_vertex, bool const is_directed = true, bool const strict_mode = false) const
{
	uint32_t const  out_vertex_new      = (is_directed) ? (out_vertex) : (std::min(out_vertex, in_vertex));
	uint32_t const  in_vertex_new       = (is_directed) ? (in_vertex)  : (std::max(out_vertex, in_vertex));
	auto            out_comparator      = [](VertexNeighbourhood const curr_neighbourhood, uint32_t const value){return curr_neighbourhood.vertex_id < value;};
	auto            out_lower_bound     = std::lower_bound(this->edges.begin(), this->edges.end(), out_vertex_new, out_comparator);

	// 1. Try to find a direct match: <out_vertex_new> ---> <in_vertex_new>
	if ((out_lower_bound != this->edges.end()) && (out_lower_bound->vertex_id == out_vertex_new))
	{
		auto in_lower_bound = std::lower_bound(out_lower_bound->connected_vertices.begin(), out_lower_bound->connected_vertices.end(), in_vertex_new);

		if ((in_lower_bound != out_lower_bound->connected_vertices.end()) && (*in_lower_bound == in_vertex_new))
		{
			if (strict_mode)
			{
				if (out_lower_bound->is_directed[std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound)] == is_directed)
					return std::make_pair(out_lower_bound, std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound));
				else
					return std::make_pair(this->edges.end(), 0);
			}
			else
				return std::make_pair(out_lower_bound, std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound));
		}
	}

	// 2. If this is NOT a strict mode and the edge is directed, we may try to find <in_vertex_new> ---- <out_vertex_new>
	if ((!strict_mode) && (is_directed) && (in_vertex_new > out_vertex_new))
	{
		out_lower_bound = std::lower_bound(this->edges.begin(), this->edges.end(), in_vertex_new, out_comparator);

		if ((out_lower_bound != this->edges.end()) && (out_lower_bound->vertex_id == in_vertex_new))
		{
			auto in_lower_bound = std::lower_bound(out_lower_bound->connected_vertices.begin(), out_lower_bound->connected_vertices.end(), out_vertex_new);

			if ((in_lower_bound != out_lower_bound->connected_vertices.end()) && (*in_lower_bound == in_vertex_new) && (!out_lower_bound->is_directed[std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound)]))
				return std::make_pair(out_lower_bound, std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound));
		}
	}

	// 3. Otherwise, such edge doesn't exist
	return std::make_pair(this->edges.end(), 0);
}





// Modifiers





void rand_walks::MetricGraph::addEdge(uint32_t const out_vertex, uint32_t const in_vertex, long double const length, bool const is_directed = false)
{
	uint32_t const  out_vertex_new  = (is_directed) ? (out_vertex) : (std::min(out_vertex, in_vertex));
	uint32_t const  in_vertex_new   = (is_directed) ? (in_vertex)  : (std::max(out_vertex, in_vertex));

	// TODO
	
	return;
}





// Save/load





void rand_walks::MetricGraph::toFile(std::string const file_name = "Saved files/My metric graph") const
{
	std::string const   file_format     = ".rweg";
	std::string         file_name_new   = file_name;
	std::fstream        out_file;

	// 1. Check if specified file already exists
	out_file.open(file_name_new + file_format, std::fstream::in | std::fstream::binary);
	if (out_file.is_open())
	{
		uint8_t file_number = 1;

		out_file.close();
		file_name_new += " (1)";
		out_file.open(file_name_new + file_format, std::fstream::in | std::fstream::binary);
		while (out_file.is_open())
		{
			out_file.close();
			file_name_new = file_name + " (" + std::to_string(++file_number) + ")";
			out_file.open(file_name_new + file_format, std::fstream::in | std::fstream::binary);
		}
	}
	out_file.close();

	// 2. Dump information about graph into this file
	out_file.open(file_name_new + file_format, std::fstream::out | std::fstream::binary);
	for (uint32_t vertex_i = 0; vertex_i < this->edges.size(); ++vertex_i)
	{
		for (uint32_t adjacent_vertex_i = 0; adjacent_vertex_i < this->edges[vertex_i].connected_vertices.size(); ++adjacent_vertex_i)
		{
			VertexNeighbourhood const &curr_vertex = this->edges[vertex_i];
			out_file.write(reinterpret_cast<char const *const>(&curr_vertex.vertex_id), sizeof(curr_vertex.vertex_id));
			out_file.write(reinterpret_cast<char const *const>(&curr_vertex.connected_vertices[adjacent_vertex_i]), sizeof(curr_vertex.connected_vertices[adjacent_vertex_i]));
			out_file.write(reinterpret_cast<char const *const>(&curr_vertex.lengths[adjacent_vertex_i]), sizeof(curr_vertex.lengths[adjacent_vertex_i]));
		}
	}
	out_file.close();
	
	return;
}



void rand_walks::MetricGraph::fromFile(std::string const file_name)
{
	std::string const   file_format     = ".rweg";
	std::fstream        in_file;

	// 1. Open file and read data
	in_file.open((file_name.substr(file_name.size() - file_format.size()) == file_format) ? (file_name) : (file_name + file_format), std::fstream::in | std::fstream::binary);
	if (in_file.is_open())
	{
		uint32_t        out_vertex(0), in_vertex(0);
		long double     length(0.0);

		in_file.read(reinterpret_cast<char *>(&out_vertex), sizeof(out_vertex));
		in_file.read(reinterpret_cast<char *>(&in_vertex), sizeof(in_vertex));
		in_file.read(reinterpret_cast<char *>(&length), sizeof(length));
		while (!in_file.fail())
		{
			this->addEdge(out_vertex, in_vertex, length, true);
			in_file.read(reinterpret_cast<char *>(&out_vertex), sizeof(out_vertex));
			in_file.read(reinterpret_cast<char *>(&in_vertex), sizeof(in_vertex));
			in_file.read(reinterpret_cast<char *>(&length), sizeof(length));
		}
	}
	in_file.close();

	return;
}
