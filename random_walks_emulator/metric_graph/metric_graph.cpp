/**
 * \file
 *       metric_graph.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "metric_graph.hpp"

#include <algorithm>    // needed for "find_if", "find"
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





// Capacity





uint32_t const rand_walks::MetricGraph::getVertexCount(void)
{
	return this->edges.size();
}





// Modifiers





void rand_walks::MetricGraph::addEdge(uint32_t out_vertex, uint32_t in_vertex, long double length, bool is_directed)
{
	auto out_vertex_neighbourhood = std::find_if(this->edges.begin(), this->edges.end(), [out_vertex](VertexNeighbourhood neighbourhood){return neighbourhood.vertex_id == out_vertex;});
	if (out_vertex_neighbourhood != this->edges.end())
	{
		auto in_vertex_iterator = std::find(out_vertex_neighbourhood->connected_vertices.begin(), out_vertex_neighbourhood->connected_vertices.end(), in_vertex);
		if (in_vertex_iterator != out_vertex_neighbourhood->connected_vertices.end())
			out_vertex_neighbourhood->lengths[std::distance(out_vertex_neighbourhood->connected_vertices.begin(), in_vertex_iterator)] = length;
		else
		{
			out_vertex_neighbourhood->connected_vertices.push_back(in_vertex);
			out_vertex_neighbourhood->lengths.push_back(length);
		}
	}
	else
		this->edges.push_back({out_vertex, VertexList({in_vertex}), LengthList({length})});
	if (!is_directed)
		this->addEdge(in_vertex, out_vertex, length, true);
	
	return;
}





// Save/load





void rand_walks::MetricGraph::toFile(std::string file_name)
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
			VertexNeighbourhood &curr_vertex = this->edges[vertex_i];
			out_file.write(reinterpret_cast<char *>(&curr_vertex.vertex_id), sizeof(curr_vertex.vertex_id));
			out_file.write(reinterpret_cast<char *>(&curr_vertex.connected_vertices[adjacent_vertex_i]), sizeof(curr_vertex.connected_vertices[adjacent_vertex_i]));
			out_file.write(reinterpret_cast<char *>(&curr_vertex.lengths[adjacent_vertex_i]), sizeof(curr_vertex.lengths[adjacent_vertex_i]));
			//out_file << curr_vertex.vertex_id << curr_vertex.connected_vertices[adjacent_vertex_i] << curr_vertex.lengths[adjacent_vertex_i];
		}
	}
	out_file.close();
	
	return;
}



void rand_walks::MetricGraph::fromFile(std::string file_name)
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
