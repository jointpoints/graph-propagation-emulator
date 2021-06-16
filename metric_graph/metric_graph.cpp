/**
 * \file
 *       metric_graph.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "../rw_space/rw_space.hpp"

#include <stdexcept>    // needed for exceptions
#include <algorithm>    // needed for "find_if", "find", "min", "max", "lower_bound"
#include <fstream>      // needed for "fstream"
#include <set>          // needed for "set"





// Constructors and destructors





rwe::MetricGraph::MetricGraph(void) :
	edges(), associated_wanders()
{
	// Intended to be empty
}



rwe::MetricGraph::~MetricGraph(void)
{
	// 1. Kill all associated wanders
	for (uint32_t wander_i = 0; wander_i < this->associated_wanders.size(); ++wander_i)
		this->associated_wanders[wander_i]->kill();
}





// Operators





rwe::MetricGraph & rwe::MetricGraph::operator=(rwe::MetricGraph &&other)
{
	// 1. Invalidate associated wanders
	for (uint32_t wander_i = 0; wander_i < this->associated_wanders.size(); ++wander_i)
		this->associated_wanders[wander_i]->invalidate();
	
	// 2. Copy data from <other>
	this->edges = std::move(other.edges);
	this->associated_wanders.insert(this->associated_wanders.end(), other.associated_wanders.begin(), other.associated_wanders.end());

	return *this;
}





// Access





bool const rwe::MetricGraph::checkVertex(uint32_t const vertex) const
{
	// 1. Check if <vertex> is present in the <id>'s of neighbourhoods
	auto    comparator          = [](VertexView const curr_vertex, uint32_t const value){return curr_vertex.id < value;};
	auto    vertex_lower_bound  = std::lower_bound(this->edges.begin(), this->edges.end(), vertex, comparator);

	if ((vertex_lower_bound != this->edges.end()) && (vertex_lower_bound->id == vertex))
		return true;

	// 2. Try looking for <vertex> among adjacent vertices
	for (uint32_t neighbourhood_i = 0; neighbourhood_i < this->edges.size(); ++neighbourhood_i)
	{
		VertexList const    &neighbourhood          = this->edges[neighbourhood_i].adjacents;
		auto                 adjacent_lower_bound   = std::lower_bound(neighbourhood.begin(), neighbourhood.end(), vertex);

		if ((adjacent_lower_bound != neighbourhood.end()) && (*adjacent_lower_bound == vertex))
			return true;
	}

	return false;
}



std::vector<uint32_t> const rwe::MetricGraph::getVertexList(void) const
{
	std::set<uint32_t> vertices;

	for (uint32_t vertex_i = 0; vertex_i < this->edges.size(); ++vertex_i)
	{
		vertices.insert(vertex_i);
		vertices.insert(this->edges[vertex_i].adjacents.begin(), this->edges[vertex_i].adjacents.end());
	}

	return std::vector<uint32_t>(vertices.begin(), vertices.end());
}



long double const rwe::MetricGraph::getEdgeLength(uint32_t const out_vertex, uint32_t in_vertex) const
{
	// 1. Try to find corresponding edge
	Edge edge = this->getEdge(out_vertex, in_vertex);

	if (edge.first != this->edges.size())
		return this->edges[edge.first].lengths[edge.second];

	// 2. If there is no edge between <out_vertex> and <in_vertex>, return infinity
	return std::numeric_limits<long double const>::infinity();
}



void rwe::MetricGraph::outputEdgeList(std::ostream &output_stream) const
{
	for (uint32_t vertex_1 = 0; vertex_1 < this->edges.size(); ++vertex_1)
	{
		VertexView const &curr_vertex = this->edges[vertex_1];
		for (uint32_t vertex_2 = 0; vertex_2 < curr_vertex.adjacents.size(); ++vertex_2)
			output_stream << curr_vertex.id << ((curr_vertex.is_directed[vertex_2]) ? (" ---> ") : (" ---- ")) << curr_vertex.adjacents[vertex_2] << '\t' << curr_vertex.lengths[vertex_2] << '\n';
	}

	return;
}



rwe::MetricGraph::Edge rwe::MetricGraph::getEdge(uint32_t const out_vertex, uint32_t const in_vertex, bool const is_directed, bool const strict_mode) const
{
	uint32_t const  out_vertex_new      = (is_directed) ? (out_vertex) : (std::min(out_vertex, in_vertex));
	uint32_t const  in_vertex_new       = (is_directed) ? (in_vertex)  : (std::max(out_vertex, in_vertex));
	auto            out_comparator      = [](VertexView const curr_vertex, uint32_t const value){return curr_vertex.id < value;};
	auto            out_lower_bound     = std::lower_bound(this->edges.begin(), this->edges.end(), out_vertex_new, out_comparator);

	// 1. Try to find a direct match: <out_vertex_new> ---> <in_vertex_new>
	if ((out_lower_bound != this->edges.end()) && (out_lower_bound->id == out_vertex_new))
	{
		auto in_lower_bound = std::lower_bound(out_lower_bound->adjacents.begin(), out_lower_bound->adjacents.end(), in_vertex_new);

		if ((in_lower_bound != out_lower_bound->adjacents.end()) && (*in_lower_bound == in_vertex_new))
		{
			if (strict_mode)
			{
				if (out_lower_bound->is_directed[std::distance(out_lower_bound->adjacents.begin(), in_lower_bound)] == is_directed)
					return std::make_pair(std::distance(this->edges.begin(), out_lower_bound), std::distance(out_lower_bound->adjacents.begin(), in_lower_bound));
				else
					return std::make_pair(this->edges.size(), 0);
			}
			else
				return std::make_pair(std::distance(this->edges.begin(), out_lower_bound), std::distance(out_lower_bound->adjacents.begin(), in_lower_bound));
		}
	}

	// 2. If this is NOT a strict mode and the edge is directed, we may try to find <in_vertex_new> ---- <out_vertex_new>
	if ((!strict_mode) && (is_directed) && (in_vertex_new > out_vertex_new))
	{
		out_lower_bound = std::lower_bound(this->edges.begin(), this->edges.end(), in_vertex_new, out_comparator);

		if ((out_lower_bound != this->edges.end()) && (out_lower_bound->id == in_vertex_new))
		{
			auto in_lower_bound = std::lower_bound(out_lower_bound->adjacents.begin(), out_lower_bound->adjacents.end(), out_vertex_new);

			if ((in_lower_bound != out_lower_bound->adjacents.end()) && (*in_lower_bound == in_vertex_new) && (!out_lower_bound->is_directed[std::distance(out_lower_bound->adjacents.begin(), in_lower_bound)]))
				return std::make_pair(std::distance(this->edges.begin(), out_lower_bound), std::distance(out_lower_bound->adjacents.begin(), in_lower_bound));
		}
	}

	// 3. Otherwise, such edge doesn't exist
	return std::make_pair(this->edges.size(), 0);
}



std::deque<rwe::MetricGraph::Edge> rwe::MetricGraph::getDepartingEdges(uint32_t const out_vertex) const
{
	auto                out_comparator  = [](VertexView const curr_vertex, uint32_t const value){return curr_vertex.id < value;};
	uint32_t const      max_index       = std::distance(this->edges.begin(), std::lower_bound(this->edges.begin(), this->edges.end(), out_vertex, out_comparator));
	std::deque<Edge>    answer;

	// 1. Traverse edges and find all indices that lead to edges of <out_vertex> ---- <x> or <out_vertex> ---> <x> types
	for (uint32_t vertex_1 = 0; vertex_1 < (((max_index < this->edges.size()) && (this->edges[max_index].id == out_vertex)) ? (max_index + 1) : (max_index)); ++vertex_1)
	{
		VertexView const &curr_vertex = this->edges[vertex_1];
		
		if (curr_vertex.id == out_vertex)
			for (uint32_t vertex_2 = 0; vertex_2 < curr_vertex.adjacents.size(); ++vertex_2)
				answer.push_back(std::make_pair(vertex_1, vertex_2));
		else
		{
			auto in_lower_bound = std::lower_bound(curr_vertex.adjacents.begin(), curr_vertex.adjacents.end(), out_vertex);

			if ((in_lower_bound != curr_vertex.adjacents.end()) && (*in_lower_bound == out_vertex) && (!curr_vertex.is_directed[std::distance(curr_vertex.adjacents.begin(), in_lower_bound)]))
				answer.push_back(std::make_pair(vertex_1, std::distance(curr_vertex.adjacents.begin(), in_lower_bound)));
		}
	}

	return answer;
}





// Modifiers





void rwe::MetricGraph::updateEdge(uint32_t const out_vertex, uint32_t const in_vertex, long double const length, bool const is_directed)
{
	// 1. <length> must be positive
	if (length <= 0)
		throw std::invalid_argument("Desired length of an edge must be a positive number.");

	uint32_t const  out_vertex_new  = (is_directed) ? (out_vertex) : (std::min(out_vertex, in_vertex));
	uint32_t const  in_vertex_new   = (is_directed) ? (in_vertex)  : (std::max(out_vertex, in_vertex));
	auto            out_comparator  = [](VertexView const curr_vertex, uint32_t const value){return curr_vertex.id < value;};
	Edge            existing_edge   = this->getEdge(out_vertex_new, in_vertex_new, is_directed);

	if (existing_edge.first == this->edges.size())
		existing_edge = this->getEdge(in_vertex_new, out_vertex_new);

	// 1. If there is no edge like this, we just add a new element to the EdgeList
	if (existing_edge.first == this->edges.size())
	{
		auto out_lower_bound = std::lower_bound(this->edges.begin(), this->edges.end(), out_vertex_new, out_comparator);

		// 1.1. If <out_vertex_new> has been added before
		if ((out_lower_bound != this->edges.end()) && (out_lower_bound->id == out_vertex_new))
		{
			auto in_lower_bound = std::lower_bound(out_lower_bound->adjacents.begin(), out_lower_bound->adjacents.end(), in_vertex_new);
			out_lower_bound->lengths.insert(out_lower_bound->lengths.begin() + std::distance(out_lower_bound->adjacents.begin(), in_lower_bound), length);
			out_lower_bound->is_directed.insert(out_lower_bound->is_directed.begin() + std::distance(out_lower_bound->adjacents.begin(), in_lower_bound), is_directed);
			out_lower_bound->adjacents.insert(in_lower_bound, in_vertex_new);
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
	if (this->edges[existing_edge.first].id == out_vertex_new)
	{
		this->edges[existing_edge.first].lengths[existing_edge.second] = length;
		if ((this->edges[existing_edge.first].is_directed[existing_edge.second]) && (is_directed))
			return;
		this->edges[existing_edge.first].is_directed[existing_edge.second] = false;
		return;
	}
	if (this->edges[existing_edge.first].id < out_vertex_new)
	{
		this->edges[existing_edge.first].lengths[existing_edge.second] = length;
		this->edges[existing_edge.first].is_directed[existing_edge.second] = false;
		return;
	}
	this->edges[existing_edge.second].adjacents.erase(this->edges[existing_edge.second].adjacents.begin() + existing_edge.first);
	this->edges[existing_edge.second].lengths.erase(this->edges[existing_edge.second].lengths.begin() + existing_edge.first);
	this->edges[existing_edge.second].is_directed.erase(this->edges[existing_edge.second].is_directed.begin() + existing_edge.first);
	if (this->edges[existing_edge.second].adjacents.size() == 0)
		this->edges.erase(this->edges.begin() + existing_edge.second);
	this->updateEdge(out_vertex_new, in_vertex_new, length, false);

	// 3. Invalidate associated wanders
	for (uint32_t wander_i = 0; wander_i < this->associated_wanders.size(); ++wander_i)
		this->associated_wanders[wander_i]->invalidate();
	
	return;
}





// Save/load





void rwe::MetricGraph::toGEXF(std::string const file_name, bool const rewrite) const
{
	std::string const   file_format     = ".gexf";
	std::string         file_name_new   = ((file_name.size() >= file_format.size()) && (file_name.substr(file_name.size() - file_format.size()) == file_format)) ? (file_name.substr(0, file_name.size() - file_format.size())) : (file_name);
	std::fstream        out_file;

	std::vector<uint32_t>   vertex_list;
	uint64_t                edge_id         = 0;

	// 1. Check if specified file already exists
	if (!rewrite)
	{
		out_file.open(file_name_new + file_format, std::fstream::in);
		if (out_file.is_open())
		{
			uint8_t file_number = 1;

			out_file.close();
			file_name_new += " (1)";
			out_file.open(file_name_new + file_format, std::fstream::in);
			while (out_file.is_open())
			{
				out_file.close();
				file_name_new = file_name + " (" + std::to_string(++file_number) + ")";
				out_file.open(file_name_new + file_format, std::fstream::in);
			}
		}
		out_file.close();
	}

	// 2. Dump information about graph into this file
	out_file.open(file_name_new + file_format, std::fstream::out);
	out_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	out_file << "<gexf xmlns=\"http://www.gexf.net/1.2draft\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.gexf.net/1.2draft http://www.gexf.net/1.2draft/gexf.xsd\" version=\"1.2\">\n";
	out_file << "\t<meta>\n";
	out_file << "\t\t<creator>Random Walks Emulator v.0.2 by Andrei Eliseev (JointPoints, https://jointpoints.github.io/random-walks/)</creator>\n";
	out_file << "\t</meta>\n";
	out_file << "\t<graph>\n";
	out_file << "\t\t<nodes>\n";
	vertex_list = this->getVertexList();
	for (uint32_t vertex_i = 0; vertex_i < vertex_list.size(); ++vertex_i)
		out_file << "\t\t\t<node id=\"" + std::to_string(vertex_list[vertex_i]) + "\"/>\n";
	out_file << "\t\t</nodes>\n";
	out_file << "\t\t<edges>\n";
	for (uint32_t vertex_1 = 0; vertex_1 < this->edges.size(); ++vertex_1)
	{
		VertexView const &curr_vertex = this->edges[vertex_1];
		for (uint32_t vertex_2 = 0; vertex_2 < curr_vertex.adjacents.size(); ++vertex_2)
			out_file << "\t\t\t<edge id=\"" + std::to_string(edge_id++) + "\" source=\"" + std::to_string(curr_vertex.id) + "\" target=\"" + \
			            std::to_string(curr_vertex.adjacents[vertex_2]) + "\" type=\"" + ((curr_vertex.is_directed[vertex_2]) ? ("directed") : ("undirected")) + \
			            "\" weight=\"" + std::to_string(curr_vertex.lengths[vertex_2]) + "\" />\n";
	}
	out_file << "\t\t</edges>\n";
	out_file << "\t</graph>\n";
	out_file << "</gexf>\n";
	out_file.close();
	
	return;
}



void rwe::MetricGraph::fromGEXF(std::string const file_name)
{
	enum GEXFLexStates{SPACE, TOKEN};
	enum GEXFParserStates{EDGE_BEGIN, ATTR_BEGIN, SOURCE_VALUE, TARGET_VALUE, TYPE_VALUE, WEIGHT_VALUE, SKIP_VALUE};

	std::string const   file_format     = ".gexf";
	std::fstream        in_file;

	// 1. Open file and read data
	in_file.open(((file_name.size() >= file_format.size()) && (file_name.substr(file_name.size() - file_format.size()) == file_format)) ? (file_name) : (file_name + file_format), std::fstream::in);
	if (in_file.is_open())
	{
		GEXFLexStates                       lexer_state         = SPACE;
		bool                                ignore_ws_regime    = false;
		char                                symbol;
		std::string                         token               = "";
		std::vector<std::string>            tokens;
		std::vector<std::string>::iterator  default_type_begin;
		std::vector<std::string>::iterator  edges_begin;
		std::vector<std::string>::iterator  edges_end;

		GEXFParserStates                    parser_state        = EDGE_BEGIN;
		uint32_t                            out_vertex(0), in_vertex(0);
		long double                         length(0.0);
		bool                                is_directed(false);
		bool                                default_is_directed(false);
		bool                                source_specified    = false;
		bool                                target_specified    = false;
		bool                                weight_specified    = false;

		// 1.1. Tokenise the text from file
		while (in_file >> std::noskipws >> symbol)
		{
			switch (lexer_state)
			{
			// expect space
			case SPACE:
				if ((symbol == ' ') || (symbol == '\t') || (symbol == '\n') || (symbol == '\r'))
					break;
				lexer_state = TOKEN;
			// expect token
			case TOKEN:
				if (!ignore_ws_regime)
				{
					if ((symbol == '>') || (symbol == '='))
					{
						if (token != "")
						{
							tokens.push_back(token);
							token = "";
						}
						lexer_state = SPACE;
						break;
					}
					if ((symbol == ' ') || (symbol == '\t') || (symbol == '\n') || (symbol == '\r'))
					{
						if (token != "")
						{
							tokens.push_back(token);
							token = "";
						}
						lexer_state = SPACE;
						break;
					}
				}
				if (symbol == '"')
					ignore_ws_regime = !ignore_ws_regime;
				else
					token += symbol;
				break;
			}
		}

		// 1.2. Find out the default edge type
		default_type_begin = std::find(tokens.begin(), tokens.end(), "defaultedgetype");
		if (default_type_begin == tokens.end() - 1)
			throw std::runtime_error("Unable to find out the default edge type.");
		if (default_type_begin != tokens.end())
		{
			if ((*(default_type_begin + 1) == "undirected") || (*(default_type_begin + 1) == "mutual"))
				default_is_directed = false;
			else
				if (*(default_type_begin + 1) == "directed")
					default_is_directed = true;
				else
					throw std::runtime_error("Unknown default type of the edge '" + *(default_type_begin + 1) + "'.");
		}

		// 1.3. We don't really care about anything but the "edges" section
		edges_begin = std::find(tokens.begin(), tokens.end(), "<edges");
		edges_end = std::find(tokens.begin(), tokens.end(), "</edges");
		if ((edges_begin == tokens.end()) || (edges_end == tokens.end()))
		{
			in_file.close();
			return;
		}
		for (auto token_i = edges_begin + 1; token_i < edges_end; ++token_i)
		{
			switch (parser_state)
			{
			// expect beginning of a new edge
			case EDGE_BEGIN:
				if (*token_i != "<edge")
					throw std::runtime_error("Unexpected token '" + *token_i + "' in the 'edges' section of the gexf file.");
				source_specified = target_specified = weight_specified = false;
				out_vertex = in_vertex = 0;
				length = 0.0;
				is_directed = default_is_directed;
				parser_state = ATTR_BEGIN;
				break;
			// expect beginning of an attribute
			case ATTR_BEGIN:
				if (*token_i == "source")
				{
					parser_state = SOURCE_VALUE;
					break;
				}
				if (*token_i == "target")
				{
					parser_state = TARGET_VALUE;
					break;
				}
				if (*token_i == "type")
				{
					parser_state = TYPE_VALUE;
					break;
				}
				if (*token_i == "weight")
				{
					parser_state = WEIGHT_VALUE;
					break;
				}
				if ((*token_i == "<edge") || (*token_i == "/"))
				{
					if (source_specified && target_specified && weight_specified)
					{
						this->updateEdge(out_vertex, in_vertex, length, is_directed);
						if (*token_i == "<edge")
						{
							source_specified = target_specified = weight_specified = false;
							out_vertex = in_vertex = 0;
							length = 0.0;
							is_directed = default_is_directed;
							break;
						}
						parser_state = EDGE_BEGIN;
						break;
					}
					throw std::runtime_error("Each edge must contain 'source', 'target' and 'weight' attributes.");
				}
				if ((*token_i)[0] == '<')
					throw std::runtime_error("Unexpected token '" + *token_i + "' in the 'edges' section of the gexf file.");
				parser_state = SKIP_VALUE;
				break;
			// expect the source value
			case SOURCE_VALUE:
				try
				{
					out_vertex = std::stoi(*token_i);
					source_specified = true;
					parser_state = ATTR_BEGIN;
					break;
				}
				catch(...){}
				throw std::runtime_error("Vertex ID '" + *token_i + "' do not comply with the requirements of emulator.");
			// expect the target value
			case TARGET_VALUE:
				try
				{
					in_vertex = std::stoi(*token_i);
					target_specified = true;
					parser_state = ATTR_BEGIN;
					break;
				}
				catch(...){}
				throw std::runtime_error("Vertex ID '" + *token_i + "' do not comply with the requirements of emulator.");
			// expect the type value
			case TYPE_VALUE:
				if ((*token_i == "undirected") || (*token_i == "mutual"))
				{
					is_directed = false;
					parser_state = ATTR_BEGIN;
					break;
				}
				if (*token_i == "directed")
				{
					is_directed = true;
					parser_state = ATTR_BEGIN;
					break;
				}
				throw std::runtime_error("Unknown type of the edge '" + *token_i + "'.");
			// expect the weight value
			case WEIGHT_VALUE:
				try
				{
					length = std::stold(*token_i);
					weight_specified = true;
					parser_state = ATTR_BEGIN;
					break;
				}
				catch(...){}
				throw std::runtime_error("Unable to interpret weight value '" + *token_i + "'.");
			// skip the value
			case SKIP_VALUE:
				parser_state = ATTR_BEGIN;
				break;
			}
		}
	}
	in_file.close();

	return;
}



void rwe::MetricGraph::toRWEG(std::string const file_name, bool const rewrite) const
{
	std::string const   file_format     = ".rweg";
	std::string         file_name_new   = ((file_name.size() >= file_format.size()) && (file_name.substr(file_name.size() - file_format.size()) == file_format)) ? (file_name.substr(0, file_name.size() - file_format.size())) : (file_name);;
	std::fstream        out_file;

	// 1. Check if specified file already exists
	if (!rewrite)
	{
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
	}

	// 2. Dump information about graph into this file
	out_file.open(file_name_new + file_format, std::fstream::out | std::fstream::binary);
	for (uint32_t vertex_1 = 0; vertex_1 < this->edges.size(); ++vertex_1)
	{
		VertexView const &curr_vertex = this->edges[vertex_1];
		for (uint32_t vertex_2 = 0; vertex_2 < curr_vertex.adjacents.size(); ++vertex_2)
		{
			bool const is_directed = curr_vertex.is_directed[vertex_2];
			out_file.write(reinterpret_cast<char const *const>(&curr_vertex.id), sizeof(curr_vertex.id));
			out_file.write(reinterpret_cast<char const *const>(&curr_vertex.adjacents[vertex_2]), sizeof(curr_vertex.adjacents[vertex_2]));
			out_file.write(reinterpret_cast<char const *const>(&curr_vertex.lengths[vertex_2]), sizeof(curr_vertex.lengths[vertex_2]));
			out_file.write(reinterpret_cast<char const *const>(&is_directed), sizeof(curr_vertex.is_directed[vertex_2]));
		}
	}
	out_file.close();
	
	return;
}



void rwe::MetricGraph::fromRWEG(std::string const file_name)
{
	std::string const   file_format     = ".rweg";
	std::fstream        in_file;

	// 1. Open file and read data
	in_file.open(((file_name.size() >= file_format.size()) && (file_name.substr(file_name.size() - file_format.size()) == file_format)) ? (file_name) : (file_name + file_format), std::fstream::in | std::fstream::binary);
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
