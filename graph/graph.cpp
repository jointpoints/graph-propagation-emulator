/**
 * @file graph.cpp
 * @author JointPoints, 2021, github.com/jointpoints
 */
#include "graph.hpp"
#include "../exceptions/exceptions.hpp"
#include "../_thirdparty/rapidxml-1.13/rapidxml.hpp"
#include "../_thirdparty/rapidxml-1.13/rapidxml_utils.hpp"
#include <algorithm>





// Constructors & destructors





gpe::Graph::Graph(std::string const gexf_path)
{
#define CHECK_NULL(pointer) if (pointer == nullptr) throw gpe::error::WrongFileFormat(gexf_path)
	try
	{
		// 1. Read GEXF as XML
		rapidxml::file<>            gexf_file       (gexf_path.c_str());
		rapidxml::xml_document<>    gexf_data;
		gexf_data.parse<0>(gexf_file.data());

		// 2. Remember default type of edges
		rapidxml::xml_node<>       *curr_node       = gexf_data.first_node("gexf")->first_node("graph");
		CHECK_NULL(curr_node);
		rapidxml::xml_attribute<>  *curr_attribute  = curr_node->first_attribute("defaultedgetype");
		bool const default_edge_directed = (curr_attribute != nullptr) && (curr_attribute->value() == "directed");

		// 3. Read nodes
		curr_node = curr_node->first_node("nodes");
		CHECK_NULL(curr_node);
		size_t                          vertex_count    = 0;
		std::vector<Vertex>::iterator   vertex_iter;
		// RapidXML cannot count children, so we'll have to do it manually
		for (curr_node = curr_node->first_node("node"); curr_node != nullptr; curr_node = curr_node->next_sibling("node"), ++vertex_count);
		curr_node = gexf_data.first_node("gexf")->first_node("graph")->first_node("nodes");
		this->self.resize(vertex_count);
		vertex_iter = this->self.begin();
		for (curr_node = curr_node->first_node("node"); curr_node != nullptr; curr_node = curr_node->next_sibling("node"), ++vertex_iter)
		{
			curr_attribute = curr_node->first_attribute("id");
			CHECK_NULL(curr_attribute);
			vertex_iter->id = curr_attribute->value();
			curr_attribute = curr_node->first_attribute("label");
			vertex_iter->label = (curr_attribute == nullptr) ? ("") : (curr_attribute->value());
		}

		// 4. Read edges
		curr_node = gexf_data.first_node("gexf")->first_node("graph")->first_node("edges");
		CHECK_NULL(curr_node);
		for (curr_node = curr_node->first_node("edge"); curr_node != nullptr; curr_node = curr_node->next_sibling("edge"))
		{
			std::string     source, target;
			long double     length;
			bool            edge_directed;
			// Extract source and target
			curr_attribute = curr_node->first_attribute("source");
			CHECK_NULL(curr_attribute);
			source = curr_attribute->value();
			curr_attribute = curr_node->first_attribute("target");
			CHECK_NULL(curr_attribute);
			target = curr_attribute->value();
			// Extract weight
			curr_attribute = curr_node->first_attribute("weight");
			length = (curr_attribute == nullptr) ? (1.0) : (std::atof(curr_attribute->value()));
			if (length <= 0.0)
				throw gpe::error::WrongFileFormat(gexf_path);
			// Look up indices of source and target by their GEXF ids
			auto        source_vertex   = std::find(this->self.begin(), this->self.end(), source);
			size_t      source_index    = source_vertex - this->self.begin();
			auto        target_vertex   = std::find(this->self.begin(), this->self.end(), target);
			size_t      target_index    = target_vertex - this->self.begin();
			if ((source_vertex == this->self.end()) || (target_index >= this->self.size()))
				throw gpe::error::WrongFileFormat(gexf_path);
			// Check if edge is directed
			curr_attribute = curr_node->first_attribute("type");
			edge_directed = (curr_attribute == nullptr) ? (default_edge_directed) : (curr_attribute->value() == "directed");
			// Check if edge between source and target or target and source already exists
			if ((std::find(source_vertex->neighbours.begin(), source_vertex->neighbours.end(), target_index) != source_vertex->neighbours.end()) ||
			    (std::find(target_vertex->neighbours.begin(), target_vertex->neighbours.end(), source_index) != target_vertex->neighbours.end()))
				throw gpe::error::WrongFileFormat(gexf_path);
			// Insert edge
			source_vertex->neighbours.emplace(target_index, length);
			// If edge is undirected, insert its reverse copy
			if (!edge_directed)
				target_vertex->neighbours.emplace(source_index, length);
		}
	}
	// runtime_error is thrown by RapidXML if file does not exist
	catch(std::runtime_error){throw gpe::error::FileDoesNotExist(gexf_path);}
	catch(rapidxml::parse_error){throw gpe::error::WrongFileFormat(gexf_path);};
#undef CHECK_NULL
}





// Access





size_t const gpe::Graph::get_edge_count(void) const
{
	size_t answer = 0;

	for (size_t vertex_i = 0; vertex_i < this->self.size(); ++vertex_i)
		for (auto edge_iter = this->self[vertex_i].neighbours.begin(); std::distance(edge_iter, this->self[vertex_i].neighbours.end()); ++edge_iter)
		{
			// Additional checks if edge is undirected
			if ((vertex_i > edge_iter->target) && (this->self[edge_iter->target].neighbours.find({vertex_i, edge_iter->length}) != this->self[edge_iter->target].neighbours.end()))
				continue;
			++answer;
		}
	
	return answer;
}





std::vector<gpe::Graph::Edge> gpe::Graph::get_out_edges(size_t const vertex_i) const
{
	return std::vector<gpe::Graph::Edge>(this->self[vertex_i].neighbours.begin(), this->self[vertex_i].neighbours.end());
}





size_t const gpe::Graph::get_vertex_count(void) const
{
	return this->self.size();
}
