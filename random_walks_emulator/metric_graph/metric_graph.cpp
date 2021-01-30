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





bool const rand_walks::MetricGraph::checkVertex(uint32_t const vertex) const
{
	// 1. Check if <vertex> is present in the <vertex_id>'s of neighbourhoods
	auto    comparator          = [](VertexNeighbourhood const curr_neighbourhood, uint32_t const value){return curr_neighbourhood.vertex_id < value;};
	auto    vertex_lower_bound  = std::lower_bound(this->edges.begin(), this->edges.end(), vertex, comparator);

	if ((vertex_lower_bound != this->edges.end()) && (vertex_lower_bound->vertex_id == vertex))
		return true;

	// 2. Try looking for <vertex> among adjacent vertices
	for (uint32_t neighbourhood_i = 0; neighbourhood_i < std::distance(this->edges.begin(), vertex_lower_bound); ++neighbourhood_i)
	{
		VertexList const    &neighbourhood          = this->edges[neighbourhood_i].connected_vertices;
		auto                 adjacent_lower_bound   = std::lower_bound(neighbourhood.begin(), neighbourhood.end(), vertex);

		if ((adjacent_lower_bound != neighbourhood.end()) && (*adjacent_lower_bound == vertex))
			return true;
	}

	return false;
}



uint32_t const rand_walks::MetricGraph::getVertexCount(void) const
{
	return 0; // TODO
}



long double const rand_walks::MetricGraph::getEdgeLength(uint32_t const out_vertex, uint32_t in_vertex) const
{
	// 1. Try to find corresponding edge
	Edge edge = this->getEdge(out_vertex, in_vertex);

	if (edge.first != this->edges.size())
		return this->edges[edge.first].lengths[edge.second];

	// 2. If there is no edge between <out_vertex> and <in_vertex>, return infinity
	return std::numeric_limits<long double const>::infinity();
}



void rand_walks::MetricGraph::outputEdgeList(std::ostream &output_stream) const
{
	for (uint32_t vertex_1 = 0; vertex_1 < this->edges.size(); ++vertex_1)
	{
		VertexNeighbourhood const &curr_neighbourhood = this->edges[vertex_1];
		for (uint32_t vertex_2 = 0; vertex_2 < curr_neighbourhood.connected_vertices.size(); ++vertex_2)
			output_stream << curr_neighbourhood.vertex_id << ((curr_neighbourhood.is_directed[vertex_2]) ? (" ---> ") : (" ---- ")) << curr_neighbourhood.connected_vertices[vertex_2] << '\t' << curr_neighbourhood.lengths[vertex_2] << '\n';
	}

	return;
}



rand_walks::MetricGraph::Edge rand_walks::MetricGraph::getEdge(uint32_t const out_vertex, uint32_t const in_vertex, bool const is_directed, bool const strict_mode) const
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
					return std::make_pair(std::distance(this->edges.begin(), out_lower_bound), std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound));
				else
					return std::make_pair(this->edges.size(), 0);
			}
			else
				return std::make_pair(std::distance(this->edges.begin(), out_lower_bound), std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound));
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
				return std::make_pair(std::distance(this->edges.begin(), out_lower_bound), std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound));
		}
	}

	// 3. Otherwise, such edge doesn't exist
	return std::make_pair(this->edges.size(), 0);
}





// Modifiers





void rand_walks::MetricGraph::updateEdge(uint32_t const out_vertex, uint32_t const in_vertex, long double const length, bool const is_directed)
{
	uint32_t const  out_vertex_new  = (is_directed) ? (out_vertex) : (std::min(out_vertex, in_vertex));
	uint32_t const  in_vertex_new   = (is_directed) ? (in_vertex)  : (std::max(out_vertex, in_vertex));
	auto            out_comparator  = [](VertexNeighbourhood const curr_neighbourhood, uint32_t const value){return curr_neighbourhood.vertex_id < value;};
	Edge            existing_edge   = this->getEdge(out_vertex_new, in_vertex_new, is_directed);

	if (existing_edge.first == this->edges.size())
		existing_edge = this->getEdge(in_vertex_new, out_vertex_new);

	// 1. If there is no edge like this, we just add a new element to the EdgeList
	if (existing_edge.first == this->edges.size())
	{
		auto out_lower_bound = std::lower_bound(this->edges.begin(), this->edges.end(), out_vertex_new, out_comparator);

		// 1.1. If <out_vertex_new> has been added before
		if ((out_lower_bound != this->edges.end()) && (out_lower_bound->vertex_id == out_vertex_new))
		{
			auto in_lower_bound = std::lower_bound(out_lower_bound->connected_vertices.begin(), out_lower_bound->connected_vertices.end(), in_vertex_new);
			out_lower_bound->lengths.insert(out_lower_bound->lengths.begin() + std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound), length);
			out_lower_bound->is_directed.insert(out_lower_bound->is_directed.begin() + std::distance(out_lower_bound->connected_vertices.begin(), in_lower_bound), is_directed);
			out_lower_bound->connected_vertices.insert(in_lower_bound, in_vertex_new);
			return;
		}

		// 1.2. If <out_vertex_new> has never been added before
		this->edges.insert(out_lower_bound, {out_vertex_new, VertexList{in_vertex_new}, LengthList{length}, DirectionList{is_directed}});
		return;
	}

	// 2. If there is an edge like this, we need to check how strict the match is:
	//        - if new edge is <out_vertex_new> ---> <in_vertex_new> but existing is <out_vertex_new> ---> <in_vertex_new>,
	//          we only need to update the length
	//        - if new edge is <out_vertex_new> ---> <in_vertex_new> but existing is <out_vertex_new> <--- <in_vertex_new>,
	//          we need to replace it with a single undirected edge <out_vertex_new> ---- <in_vertex_new>
	//        - if new edge is <out_vertex_new> ---> <in_vertex_new> but existing is <out_vertex_new> ---- <in_vertex_new>,
	//          we only need to update the length
	//        - if new edge is <out_vertex_new> ---- <in_vertex_new> but existing is <out_vertex_new> <--- <in_vertex_new> or <out_vertex_new> ---> <in_vertex_new>,
	//          we need to replace it with a single undirected edge <out_vertex_new> ---- <in_vertex_new>
	//        - if new edge is <out_vertex_new> ---- <in_vertex_new> but existing is <out_vertex_new> ---- <in_vertex_new>,
	//          we only need to update the length
	if (this->edges[existing_edge.first].vertex_id == out_vertex_new)
	{
		this->edges[existing_edge.first].lengths[existing_edge.second] = length;
		if ((this->edges[existing_edge.first].is_directed[existing_edge.second]) && (is_directed))
			return;
		this->edges[existing_edge.first].is_directed[existing_edge.second] = false;
		return;
	}
	if (this->edges[existing_edge.first].vertex_id < out_vertex_new)
	{
		this->edges[existing_edge.first].lengths[existing_edge.second] = length;
		this->edges[existing_edge.first].is_directed[existing_edge.second] = false;
		return;
	}
	this->edges[existing_edge.second].connected_vertices.erase(this->edges[existing_edge.second].connected_vertices.begin() + existing_edge.first);
	this->edges[existing_edge.second].lengths.erase(this->edges[existing_edge.second].lengths.begin() + existing_edge.first);
	this->edges[existing_edge.second].is_directed.erase(this->edges[existing_edge.second].is_directed.begin() + existing_edge.first);
	if (this->edges[existing_edge.second].connected_vertices.size() == 0)
		this->edges.erase(this->edges.begin() + existing_edge.second);
	this->updateEdge(out_vertex_new, in_vertex_new, length, false);
	
	return;
}





// Save/load





void rand_walks::MetricGraph::toFile(std::string const file_name) const
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
	for (uint32_t vertex_1 = 0; vertex_1 < this->edges.size(); ++vertex_1)
	{
		VertexNeighbourhood const &curr_neighbourhood = this->edges[vertex_1];
		for (uint32_t vertex_2 = 0; vertex_2 < curr_neighbourhood.connected_vertices.size(); ++vertex_2)
		{
			bool const is_directed = curr_neighbourhood.is_directed[vertex_2];
			out_file.write(reinterpret_cast<char const *const>(&curr_neighbourhood.vertex_id), sizeof(curr_neighbourhood.vertex_id));
			out_file.write(reinterpret_cast<char const *const>(&curr_neighbourhood.connected_vertices[vertex_2]), sizeof(curr_neighbourhood.connected_vertices[vertex_2]));
			out_file.write(reinterpret_cast<char const *const>(&curr_neighbourhood.lengths[vertex_2]), sizeof(curr_neighbourhood.lengths[vertex_2]));
			out_file.write(reinterpret_cast<char const *const>(&is_directed), sizeof(curr_neighbourhood.is_directed[vertex_2]));
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
		bool            is_directed(false);

		in_file.read(reinterpret_cast<char *>(&out_vertex), sizeof(out_vertex));
		in_file.read(reinterpret_cast<char *>(&in_vertex), sizeof(in_vertex));
		in_file.read(reinterpret_cast<char *>(&length), sizeof(length));
		in_file.read(reinterpret_cast<char *>(&is_directed), sizeof(is_directed));
		while (!in_file.fail())
		{
			this->updateEdge(out_vertex, in_vertex, length, is_directed);
			in_file.read(reinterpret_cast<char *>(&out_vertex), sizeof(out_vertex));
			in_file.read(reinterpret_cast<char *>(&in_vertex), sizeof(in_vertex));
			in_file.read(reinterpret_cast<char *>(&length), sizeof(length));
			in_file.read(reinterpret_cast<char *>(&is_directed), sizeof(is_directed));
		}
	}
	in_file.close();

	return;
}
