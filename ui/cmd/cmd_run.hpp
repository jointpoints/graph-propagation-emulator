/**
 * \file
 *       cmd_run.hpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#ifndef RWE__UI_CMD_RUN_HPP__
#define RWE__UI_CMD_RUN_HPP__





#include "../../rw_space/rw_space.hpp"
#include "../ui_common.hpp"
#include <chrono>   // needed for "chrono" and "duration_cast"
#include <fstream>  // needed for "fstream" and "regex_match"
#include <regex>    // needed for "regex"





enum ScenarioParseStates
{
	TOP,                                // expect a top-level command ("graph")
	GRAPH_FILE,                         // expect a file name with a graph
	GRAPH_BODY_BEGIN,                   // expect a '{' character for a graph block
	GRAPH_BODY,                         // expect a graph command ("epsilon-saturation") or a '}' character
	EPSILON_WANDER_BEGIN,               // expect a '{' character for an epsilon-saturation block
	EPSILON_WANDER_ARG,                 // expect an epsilon-saturation argument ("start-vertex", "epsilon", "time-delta", "use-skip-forward") or a '}' character
	EPSILON_WANDER_INT_VALUES_BEGIN,    // expect a ':' character before integer values inside an epsilon-saturation block
	EPSILON_WANDER_REAL_VALUES_BEGIN,   // expect a ':' character before real values inside an epsilon-saturation block
	EPSILON_WANDER_BOOL_VALUES_BEGIN,   // expect a ':' character before bool values inside an epsilon-saturation block
	EPSILON_WANDER_INT_VALUES,          // expect an integer value, an array of integer values, or generator of integer values
	EPSILON_WANDER_REAL_VALUES,         // expect a real value, an array of real values, or generator of real values
	EPSILON_WANDER_BOOL_VALUES,         // expect a bool value or an array of bool values
};





// Run epsilon wander emulation
#define EMULATION_ERROR(what)   throw std::domain_error(what);
void runEpsilonWander(AppSettings const &settings, rwe::RWSpace &rw_space,
                      std::vector<uint32_t> &epsilon_wander_start_vertex, std::vector<long double> &epsilon_wander_epsilon,
                      std::vector<long double> &epsilon_wander_time_delta, std::vector<bool> &epsilon_wander_use_skip_forward,
                      uint8_t const verbosity_level)
{
	// 1. If some parameters were not set by user, set them by default values
	if (epsilon_wander_start_vertex.size() == 0) epsilon_wander_start_vertex.push_back(settings.default_epsilon_wander_params.start_vertex);
	if (epsilon_wander_epsilon.size() == 0) epsilon_wander_epsilon.push_back(settings.default_epsilon_wander_params.epsilon);
	if (epsilon_wander_time_delta.size() == 0) epsilon_wander_time_delta.push_back(settings.default_epsilon_wander_params.time_delta);
	if (epsilon_wander_use_skip_forward.size() == 0) epsilon_wander_use_skip_forward.push_back(settings.default_epsilon_wander_params.use_skip_forward);

	// 2. Print header
	switch (verbosity_level)
	{
	// raw output
	case 0:
		break;
	// default output
	case 1:
		std::cout << "Epsilon saturation experiment:\n";
		break;
	// MarkDown output
	case 2:
		std::cout << "\n### Epsilon saturation experiment\n";
		break;
	}

	// 3. Run emulations
	auto time_start = std::chrono::high_resolution_clock::now();
	for (uint32_t start_vertex_i = 0; start_vertex_i < epsilon_wander_start_vertex.size(); ++start_vertex_i)
	for (uint32_t epsilon_i = 0; epsilon_i < epsilon_wander_epsilon.size(); ++epsilon_i)
	for (uint32_t time_delta_i = 0; time_delta_i < epsilon_wander_time_delta.size(); ++time_delta_i)
	for (uint32_t use_skip_forward_i = 0; use_skip_forward_i < epsilon_wander_use_skip_forward.size(); ++use_skip_forward_i)
	{
		rw_space.reset();
		try
		{
			long double saturation_time = rw_space.run_saturation(epsilon_wander_start_vertex[start_vertex_i], epsilon_wander_epsilon[epsilon_i], epsilon_wander_time_delta[time_delta_i], epsilon_wander_use_skip_forward[use_skip_forward_i]); \
			switch (verbosity_level)
			{
			// raw output
			case 0:
				std::cout << (((epsilon_i | time_delta_i | use_skip_forward_i) > 0) ? (",") : (  (start_vertex_i > 0) ? ("\n") : ("")  )) << saturation_time;
				break;
			// default output
			case 1:
				std::cout << (((epsilon_i | time_delta_i | use_skip_forward_i) > 0) ? ("") : ("\tStart vertex : " + std::to_string(epsilon_wander_start_vertex[start_vertex_i]) + "\n"));
				std::cout << "\t\tEpsilon = " << epsilon_wander_epsilon[epsilon_i] << "(time delta = " << epsilon_wander_time_delta[time_delta_i] << ")\t: " << saturation_time << '\n';
				break;
			// MarkDown output
			case 2:
				std::cout << (((epsilon_i | time_delta_i | use_skip_forward_i) > 0) ? ("") : ("\n#### Start vertex : " + std::to_string(epsilon_wander_start_vertex[start_vertex_i]) + "\n\n"));
				std::cout << (((epsilon_i | time_delta_i | use_skip_forward_i) > 0) ? ("") : ("| Epsilon | Time delta | Saturation time |\n|:-------:|:----------:|:---------------:|\n"));
				std::cout << "| " << epsilon_wander_epsilon[epsilon_i] << " | " << epsilon_wander_time_delta[time_delta_i] << " | " << saturation_time << " |\n";
				break;
			}
		}
		catch (std::invalid_argument &e) {if (verbosity_level == 0) std::cout << '\n'; EMULATION_ERROR("The start vertex does not exist.");}
		catch (std::logic_error &e) {if (verbosity_level == 0) std::cout << '\n'; EMULATION_ERROR("Unknown exception.");}
	}
	auto time_stop = std::chrono::high_resolution_clock::now();

	// 4. Print footer
	switch (verbosity_level)
	{
	// raw output
	case 0:
		std::cout << '\n';
		break;
	// default output
	case 1:
		std::cout << "Completed in " << std::chrono::duration_cast<std::chrono::seconds>(time_stop - time_start).count() << " seconds.\n";
		break;
	// MarkDown output
	case 2:
		std::cout << "\nCompleted in " << std::chrono::duration_cast<std::chrono::seconds>(time_stop - time_start).count() << " seconds.\n";
		break;
	}

	epsilon_wander_start_vertex.clear(); epsilon_wander_epsilon.clear(); epsilon_wander_time_delta.clear(); epsilon_wander_use_skip_forward.clear();

	return;
}
#undef EMULATION_ERROR





#define SYNTAX_ERROR(what)      throw std::runtime_error(what);
#define EMULATION_ERROR(what)   throw std::domain_error(what);
void cmd_run(AppSettings &settings, std::vector<std::string> const &params)
{
	std::string                 scenario_path;
	std::string const           file_format         = ".rwes";
	std::fstream                in_file;
	char                        symbol              = 0;
	bool                        ignore_ws_regime    = false;
	std::string                 token               = "";
	LexStates                   lexer_state         = SPACE;

	std::vector<std::string>    tokens;
	std::string                 curr_argument_string;
	ScenarioParseStates         parser_state = TOP;

	rwe::MetricGraph     graph;

	rwe::RWSpace         rw_space(graph);
	std::vector<uint32_t>       epsilon_wander_start_vertex;
	std::vector<long double>    epsilon_wander_epsilon;
	std::vector<long double>    epsilon_wander_time_delta;
	std::vector<bool>           epsilon_wander_use_skip_forward;

	std::vector<uint32_t>      *curr_int_params     = nullptr;
	std::vector<long double>   *curr_real_params    = nullptr;
	std::vector<bool>          *curr_bool_params    = nullptr;

	uint8_t                     verbosity_level     = 1;

	// 1. Process params
	for (uint32_t param_i = 1; param_i < params.size(); ++param_i)
	{
		if (params[param_i] == "-d")
		{
			verbosity_level = 1;
			continue;
		}
		if (params[param_i] == "-m")
		{
			verbosity_level = 2;
			continue;
		}
		if (params[param_i] == "-r")
		{
			verbosity_level = 0;
			continue;
		}
		scenario_path = params[param_i];
	}

	// 2. Open scenario file
	in_file.open(((scenario_path.size() >= file_format.size()) && (scenario_path.substr(scenario_path.size() - file_format.size()) == file_format)) ? (scenario_path) : (scenario_path + file_format), std::fstream::in);
	if (!in_file.is_open())
		throw std::invalid_argument("Scenario '" + scenario_path + "' was not found.");
	
	// 3. Read and tokenise scenario
	while (in_file >> std::noskipws >> symbol)
	{
		switch (lexer_state)
		{
		// expect whitespaces
		case SPACE:
			if ((symbol == ' ') || (symbol == '\t') || (symbol == '\n') || (symbol == '\r'))
				break;
			lexer_state = TOKEN;
		// expect token
		case TOKEN:
			if (!ignore_ws_regime)
			{
				if ((symbol == '{') || (symbol == '}') || (symbol == ',') || (symbol == ':'))
				{
					if (token != "")
					{
						tokens.push_back(token);
						token = "";
					}
					tokens.emplace_back(1, symbol);
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
			if (symbol == '#')
			{
				if (token != "")
				{
					tokens.push_back(token);
					token = "";
				}
				lexer_state = COMMENT;
				break;
			}
			if (symbol == '"')
				ignore_ws_regime = !ignore_ws_regime;
			else
				token += symbol;
			break;
		// expect comment
		case COMMENT:
			if ((symbol == '\n') || (symbol == '\r'))
				lexer_state = SPACE;
			break;
		}
	}
	if (token != "")
		tokens.push_back(token);
	in_file.close();

	switch(verbosity_level)
	{
	// raw output
	case 0:
		break;
	// default output
	case 1:
		std::cout << "=== SCENARIO EXECUTION REPORT ===\n";
		break;
	// MarkDown output
	case 2:
		std::cout << "# SCENARIO EXECUTION REPORT\n";
		break;
	}
	
	// 4. Parse and execute scenario
	for (uint32_t token_i = 0; token_i < tokens.size(); ++token_i)
	{
		switch (parser_state)
		{
		// expect a top-level command ("graph")
		case TOP:
			if (tokens[token_i] == "graph")
			{
				parser_state = GRAPH_FILE;
				break;
			}
			SYNTAX_ERROR("Scenario contains unknown command '" + tokens[token_i] +"'.");

		// expect a file name with a graph
		case GRAPH_FILE:
			in_file.open(tokens[token_i], std::fstream::in);
			if (!in_file.is_open())
			{
				in_file.close();
				EMULATION_ERROR("Graph '" + tokens[token_i] + "' does not exist.");
			}
			in_file.close();
			graph = rwe::MetricGraph();
			graph.fromRWEG(tokens[token_i]);
			switch (verbosity_level)
			{
			// raw output
			case 0:
				break;
			// default output
			case 1:
				std::cout << "\n--- Graph " << tokens[token_i] << " ---\n";
				break;
			// MarkDown output
			case 2:
				std::cout << "\n## Graph " << tokens[token_i] << '\n';
				break;
			}
			parser_state = GRAPH_BODY_BEGIN;
			break;

		// expect a '{' character for a graph block
		case GRAPH_BODY_BEGIN:
			if (tokens[token_i] == "{")
			{
				parser_state = GRAPH_BODY;
				break;
			}
			SYNTAX_ERROR("Expected an opening of graph block. Found '" + tokens[token_i] + "' instead.");
		
		// expect a graph command ("epsilon-saturation") or a '}' character
		case GRAPH_BODY:
			if (tokens[token_i] == "epsilon-saturation")
			{
				parser_state = EPSILON_WANDER_BEGIN;
				break;
			}
			if (tokens[token_i] == "}")
			{
				parser_state = TOP;
				break;
			}
			SYNTAX_ERROR("Graph block contains unknown command '" + tokens[token_i] + "'.");
		
		// expect a '{' character for an epsilon-saturation block
		case EPSILON_WANDER_BEGIN:
			if (tokens[token_i] == "{")
			{
				parser_state = EPSILON_WANDER_ARG;
				break;
			}
			SYNTAX_ERROR("Expected an opening of epsilon-saturation block. Found '" + tokens[token_i] + "' instead.");

		// expect an epsilon-wander argument ("start-vertex", "epsilon", "time-delta", "use-skip-forward") or a '}' character
		case EPSILON_WANDER_ARG:
			curr_argument_string = tokens[token_i];
			if (tokens[token_i] == "start-vertex")
			{
				curr_int_params = &epsilon_wander_start_vertex;
				parser_state = EPSILON_WANDER_INT_VALUES_BEGIN;
				break;
			}
			if ((tokens[token_i] == "epsilon") || (tokens[token_i] == "time-delta"))
			{
				curr_real_params = (tokens[token_i] == "epsilon") ? (&epsilon_wander_epsilon) : (&epsilon_wander_time_delta);
				parser_state = EPSILON_WANDER_REAL_VALUES_BEGIN;
				break;
			}
			if (tokens[token_i] == "use-skip-forward")
			{
				curr_bool_params = &epsilon_wander_use_skip_forward;
				parser_state = EPSILON_WANDER_BOOL_VALUES_BEGIN;
				break;
			}
			if (tokens[token_i] == "}")
			{
				runEpsilonWander(settings, rw_space, epsilon_wander_start_vertex, epsilon_wander_epsilon, epsilon_wander_time_delta, epsilon_wander_use_skip_forward, verbosity_level);
				parser_state = GRAPH_BODY;
				break;
			}
			SYNTAX_ERROR("Epsilon saturation block contains unknown command '" + tokens[token_i] + "'.");

		// expect a ':' character before integer values inside an epsilon-saturation block
		case EPSILON_WANDER_INT_VALUES_BEGIN:
			if (tokens[token_i] == ":")
			{
				parser_state = EPSILON_WANDER_INT_VALUES;
				break;
			}
			SYNTAX_ERROR("Expected a colon after the name of the parameter '" + curr_argument_string + "'. Found '" + tokens[token_i] + "' instead.");
		
		// expect a ':' character before real values inside an epsilon-saturation block
		case EPSILON_WANDER_REAL_VALUES_BEGIN:
			if (tokens[token_i] == ":")
			{
				parser_state = EPSILON_WANDER_REAL_VALUES;
				break;
			}
			SYNTAX_ERROR("Expected a colon after the name of the parameter '" + curr_argument_string + "'. Found '" + tokens[token_i] + "' instead.");
		
		// expect a ':' character before bool values inside an epsilon-saturation block
		case EPSILON_WANDER_BOOL_VALUES_BEGIN:
			if (tokens[token_i] == ":")
			{
				parser_state = EPSILON_WANDER_BOOL_VALUES;
				break;
			}
			SYNTAX_ERROR("Expected a colon after the name of the parameter '" + curr_argument_string + "'. Found '" + tokens[token_i] + "' instead.");
		
		// expect an integer value, an array of integer values, or generator of integer values
		case EPSILON_WANDER_INT_VALUES:
			if (tokens[token_i] == ",")
			{
				parser_state = EPSILON_WANDER_ARG;
				break;
			}
			if (tokens[token_i] == "}")
			{
				runEpsilonWander(settings, rw_space, epsilon_wander_start_vertex, epsilon_wander_epsilon, epsilon_wander_time_delta, epsilon_wander_use_skip_forward, verbosity_level);
				parser_state = GRAPH_BODY;
				break;
			}
			if (std::regex_match(tokens[token_i], std::regex("(.+)\\.\\.(.+)\\.\\.(.+)")))
			{
				try
				{
					token = tokens[token_i];
					uint32_t array_elem = std::stoi(token.substr(0, token.find("..")));
					token.erase(0, token.find("..") + 2);
					uint32_t array_step = std::stoi(token.substr(0, token.find("..")));
					token.erase(0, token.find("..") + 2);
					uint32_t array_stop = std::stoi(token.substr(0, token.find("..")));
					while (array_elem <= array_stop)
					{
						curr_int_params->push_back(array_elem);
						array_elem += array_step;
					}
					break;
				}
				catch (...) {}
				SYNTAX_ERROR("Cannot interpret value '" + tokens[token_i] + "' for parameter '" + curr_argument_string + "' as integer of integer generator.");
			}
			try
			{
				curr_int_params->push_back(std::stoi(tokens[token_i]));
				break;
			}
			catch (...) {}
			SYNTAX_ERROR("Cannot interpret value '" + tokens[token_i] + "' for parameter '" + curr_argument_string + "' as integer of integer generator.");
		
		// expect a real value, an array of real values, or generator of real values
		case EPSILON_WANDER_REAL_VALUES:
			if (tokens[token_i] == ",")
			{
				parser_state = EPSILON_WANDER_ARG;
				break;
			}
			if (tokens[token_i] == "}")
			{
				runEpsilonWander(settings, rw_space, epsilon_wander_start_vertex, epsilon_wander_epsilon, epsilon_wander_time_delta, epsilon_wander_use_skip_forward, verbosity_level);
				parser_state = GRAPH_BODY;
				break;
			}
			if (std::regex_match(tokens[token_i], std::regex("(.+)\\.\\.(.+)\\.\\.(.+)")))
			{
				try
				{
					token = tokens[token_i];
					long double array_elem = std::stold(token.substr(0, token.find("..")));
					token.erase(0, token.find("..") + 2);
					long double array_step = std::stold(token.substr(0, token.find("..")));
					token.erase(0, token.find("..") + 2);
					long double array_stop = std::stold(token.substr(0, token.find("..")));
					while (array_elem <= array_stop + array_step / 100)
					{
						curr_real_params->push_back(array_elem);
						array_elem += array_step;
					}
					break;
				}
				catch (...) {}
				SYNTAX_ERROR("Cannot interpret value '" + tokens[token_i] + "' for parameter '" + curr_argument_string + "' as real of real generator.");
			}
			try
			{
				curr_real_params->push_back(std::stold(tokens[token_i]));
				break;
			}
			catch (...) {}
			SYNTAX_ERROR("Cannot interpret value '" + tokens[token_i] + "' for parameter '" + curr_argument_string + "' as real of real generator.");
		
		// expect a bool value or an array of bool values
		case EPSILON_WANDER_BOOL_VALUES:
			if (tokens[token_i] == ",")
			{
				parser_state = EPSILON_WANDER_ARG;
				break;
			}
			if (tokens[token_i] == "}")
			{
				runEpsilonWander(settings, rw_space, epsilon_wander_start_vertex, epsilon_wander_epsilon, epsilon_wander_time_delta, epsilon_wander_use_skip_forward, verbosity_level);
				parser_state = GRAPH_BODY;
				break;
			}
			if (tokens[token_i] == "true")
			{
				curr_bool_params->push_back(true);
				break;
			}
			if (tokens[token_i] == "false")
			{
				curr_bool_params->push_back(false);
				break;
			}
			SYNTAX_ERROR("Cannot interpret value '" + tokens[token_i] + "' for parameter '" + curr_argument_string + "' as boolean.");
		}
	}

	return;
}
#undef SYNTAX_ERROR
#undef EMULATION_ERROR





#endif
