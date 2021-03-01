/**
 * \file
 *       ui.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "ui.hpp"

#include <fstream>      // needed for "fstream"
#include <stdexcept>    // needed for exceptions
#include <string>       // needed for strings and "getline"
#include <sstream>      // needed for "istringstream"
#include <iterator>     // needed for "istream_iterator"





// TRANSITION UNIT ENUM
enum ScenarioParseStates
{
	TOP,        // expect a top-level command: "graph"
	GRAPH_FILE  // expect a file name with a graph
};





// TRANSITION UNIT FUNCTION
#define SYNTAX_ERROR(what) do {in_file.close(); throw std::runtime_error(what);} while (false)
void executeScenario(std::string scenario_path)
{
	std::string const       file_format     = ".rwes";
	std::fstream            in_file;
	std::string             token;
	ScenarioParseStates     curr_state = TOP;

	std::vector<rand_walks::MetricGraph>    graphs;
	std::vector<rand_walks::Wander>         wanders;

	// 1. Open scenario file
	in_file.open(((scenario_path.size() >= file_format.size()) && (scenario_path.substr(scenario_path.size() - file_format.size()) == file_format)) ? (scenario_path) : (scenario_path + file_format), std::fstream::in);
	if (!in_file.is_open())
		throw std::invalid_argument("Scenario was not found.");
	
	// 2. Read scenario
	while (in_file >> token)
	{
		switch (curr_state)
		{
		case TOP:
			if (token == "graph")
			{
				curr_state = GRAPH_FILE;
				break;
			}
			else SYNTAX_ERROR("Scenario contains unknown command '" + token +"'.");
		case GRAPH_FILE:
			break;
		}
	}
	
	in_file.close();
	return;
}
#undef SYNTAX_ERROR





#define PRINT_ERROR(error_type, what) {std::cerr << error_type << ". " << what << '\n'; continue;}
void run(void)
{
	std::string                 command = "";
	std::istringstream          command_stream(command);
	std::vector<std::string>    command_tokens;

	// 1. Await for the next command
	while (command != "exit")
	{
		std::cout << ">>> ";
		std::getline(std::cin, command);
		command_stream = std::istringstream(command);
		command_tokens = std::vector<std::string>(std::istream_iterator<std::string>(command_stream), std::istream_iterator<std::string>());

		// 1.1. If nothing was entered
		if (command_tokens.size() == 0)
			continue;
		// 1.2. If user wants to run a scenario
		if (command_tokens[0] == "run")
		{
			if (command_tokens.size() < 2)
				PRINT_ERROR("COMMAND LINE ERROR", "Scenario file name was expected after 'run'.")

			try
			{
				executeScenario(command_tokens[1]);
			}
			catch (std::invalid_argument &e) PRINT_ERROR("FILE ERROR", e.what())
			catch (std::runtime_error &e) PRINT_ERROR("SYNTAX ERROR", e.what())
		}
		// 1.n. Unknown command
		if (command != "exit")
			PRINT_ERROR("COMMAND LINE ERROR", "Unknown command.")
	}

	return;
}
#undef PRINT_ERROR





void runScenario(std::string scenario_path)
{
	return;
}
