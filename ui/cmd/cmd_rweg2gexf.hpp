/**
 * \file
 *       cmd_rweg2gexf.hpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#ifndef RWE__UI_CMD_RWEG2GEXF_HPP__
#define RWE__UI_CMD_RWEG2GEXF_HPP__





#include "../ui_common.hpp"
#include "../../metric_graph/metric_graph.hpp"
#include <vector>       // needed for "vector"
#include <string>       // needed for "string"
#include <iostream>     // needed for output





void cmd_rweg2gexf(AppSettings &settings, std::vector<std::string> const &params)
{
	rwe::MetricGraph    graph;
	std::string         in_file;

	// 1. Process params
	if (params.size() == 1)
		throw std::invalid_argument("Expected infile.");
	in_file = (params[1].substr(params[1].size() - 5) == ".rweg") ? (params[1].substr(0, params[1].size() - 5) ) : (params[1]);

	// 2. If there is no out file name, save with the same name but different format
	if (params.size() == 2)
	{
		graph.fromRWEG(in_file);
		graph.toGEXF(in_file);
		return;
	}

	// 3. If there is an out file name, save with the specified name
	graph.fromRWEG(in_file);
	graph.toGEXF(params[2]);

	return;
}





#endif
