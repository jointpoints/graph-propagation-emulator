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
	TOP,                                // expect a top-level command ("graph")
	GRAPH_FILE,                         // expect a file name with a graph
	GRAPH_FILE_LONG,                    // expect a file name with a graph that consists of multiple words
	GRAPH_BODY_BEGIN,                   // expect a '{' character for a graph block
	GRAPH_BODY,                         // expect a graph command ("epsilon-wander") or a '}' character
	EPSILON_WANDER_BEGIN,               // expect a '{' character for an epsilon-wander block
	EPSILON_WANDER_ARG,                 // expect an epsilon-wander argument ("start-vertex", "epsilon", "time-delta", "use-skip-forward") or a '}' character
	EPSILON_WANDER_INT_VALUES_BEGIN,    // expect a ':' character before integer values inside an epsilon-wander block
	EPSILON_WANDER_REAL_VALUES_BEGIN,   // expect a ':' character before real values inside an epsilon-wander block
	EPSILON_WANDER_BOOL_VALUES_BEGIN,   // expect a ':' character before bool values inside an epsilon-wander block
	EPSILON_WANDER_INT_VALUES,          // expect an integer value, an array of integer values, or geenrator of integer values
};





// TRANSITION UNIT STRUCT
struct EpsilonWanderParams
{
	uint32_t        start_vertex;
	long double     epsilon;
	long double     time_delta;
	bool            use_skip_forward;
};





// TRANSITION UNIT STRUCT
struct AppSettings
{
	EpsilonWanderParams default_epsilon_wander_params;
};





// TRANSITION UNIT FUNCTION
// Check the presence of all technical files and create them, if needed; load the configuration
AppSettings init(void)
{
	std::fstream        config_file;

	// 1. If default configuration file does not exist, create it
	config_file.open("Technical files/dc", std::fstream::in | std::fstream::binary);
	if (!config_file.is_open())
	{
		config_file.close();
		config_file.open("Technical files/dc", std::fstream::out | std::fstream::binary);

		AppSettings default_settings;
		default_settings.default_epsilon_wander_params.epsilon              = 0.5L;
		default_settings.default_epsilon_wander_params.start_vertex         = 0UL;
		default_settings.default_epsilon_wander_params.time_delta           = 1e-6L;
		default_settings.default_epsilon_wander_params.use_skip_forward     = true;

		config_file.write(reinterpret_cast<char const *const>(&default_settings), sizeof(default_settings));
	}
	config_file.close();

	// 2. If global configuration file does not exist, create it
	config_file.open("Technical files/gc", std::fstream::in | std::fstream::binary);
	if (!config_file.is_open())
	{
		config_file.close();
		config_file.open("Technical files/gc", std::fstream::out | std::fstream::binary);

		AppSettings default_settings;
		default_settings.default_epsilon_wander_params.epsilon              = 0.5L;
		default_settings.default_epsilon_wander_params.start_vertex         = 0UL;
		default_settings.default_epsilon_wander_params.time_delta           = 1e-6L;
		default_settings.default_epsilon_wander_params.use_skip_forward     = true;

		config_file.write(reinterpret_cast<char const *const>(&default_settings), sizeof(default_settings));
		config_file.close();

		return default_settings;
	}

	// 3. Read information from global configuration
	AppSettings settings;
	config_file.read(reinterpret_cast<char *>(&settings), sizeof(settings));

	return settings;
}





// Command line functions





// TRANSITION UNIT FUNCTION
#define SYNTAX_ERROR(what)      do {in_file.close(); throw std::runtime_error(what);} while (false)
#define EMULATION_ERROR(what)   do {in_file.close(); throw std::domain_error(what);} while (false)
#define RUN_EMULATION           wander.reset(); \
                                try \
                                { \
                                    std::cout << wander.run(curr_wander_params.start_vertex, curr_wander_params.epsilon, curr_wander_params.time_delta, curr_wander_params.use_skip_forward) << '\n'; \
                                } \
                                catch (std::invalid_argument &e) {EMULATION_ERROR("The start vertex does not exist.");} \
                                catch (std::logic_error &e) {EMULATION_ERROR("Unknown exception.");}
void cmd_run(AppSettings const & settings, std::string scenario_path)
{
	std::string const       file_format     = ".rwes";
	std::fstream            in_file;
	std::string             token;
	std::string             curr_argument;
	std::vector<uint32_t>   curr_arguments_uint32;
	ScenarioParseStates     curr_state = TOP;

	rand_walks::MetricGraph     graph;
	rand_walks::Wander          wander(graph);
	EpsilonWanderParams         curr_wander_params = settings.default_epsilon_wander_params;

	// 1. Open scenario file
	in_file.open(((scenario_path.size() >= file_format.size()) && (scenario_path.substr(scenario_path.size() - file_format.size()) == file_format)) ? (scenario_path) : (scenario_path + file_format), std::fstream::in);
	if (!in_file.is_open())
		throw std::invalid_argument("Scenario was not found.");
	
	// 2. Read scenario
	while (in_file >> token)
	{
		switch (curr_state)
		{
		// expect a top-level command ("graph")
		case TOP:
			if (token == "graph")
			{
				curr_state = GRAPH_FILE;
				break;
			}
			SYNTAX_ERROR("Scenario contains unknown command '" + token +"'.");

		// expect a file name with a graph
		case GRAPH_FILE:
			if (token[0] == '"')
			{
				if (token[token.size() - 1] == '"')
				{
					graph = rand_walks::MetricGraph();
					graph.fromFile(token.substr(1, token.size() - 2));
					curr_state = GRAPH_BODY_BEGIN;
					break;
				}
				curr_argument = token.substr(1);
				curr_state = GRAPH_FILE_LONG;
				break;
			}
			graph = rand_walks::MetricGraph();
			graph.fromFile(token);
			curr_state = GRAPH_BODY_BEGIN;
			break;

		// expect a file name with a graph that consists of multiple words
		case GRAPH_FILE_LONG:
			if (token[token.size() - 1] != '"')
			{
				curr_argument += ' ' + token;
				break;
			}
			curr_argument += ' ' + token.substr(0, token.size() - 1);
			graph = rand_walks::MetricGraph();
			graph.fromFile(curr_argument);
			curr_argument = "";
			curr_state = GRAPH_BODY_BEGIN;
			break;

		// expect a '{' character for a graph block
		case GRAPH_BODY_BEGIN:
			if (token == "{")
			{
				curr_state = GRAPH_BODY;
				break;
			}
			SYNTAX_ERROR("Expected an opening of graph block. Found '" + token + "' instead.");
		
		// expect a graph command ("epsilon-wander") or a '}' character
		case GRAPH_BODY:
			if (token == "epsilon-wander")
			{
				curr_state = EPSILON_WANDER_BEGIN;
				break;
			}
			if (token == "}")
			{
				curr_state = TOP;
				break;
			}
			SYNTAX_ERROR("Graph block contains unknown command '" + token + "'.");
		
		// expect a '{' character for an epsilon-wander block
		case EPSILON_WANDER_BEGIN:
			if (token == "{")
			{
				curr_wander_params = settings.default_epsilon_wander_params;
				curr_state = EPSILON_WANDER_ARG;
				break;
			}
			SYNTAX_ERROR("Expected an opening of epsilon-wander block. Found '" + token + "' instead.");

		// expect an epsilon-wander argument ("start-vertex", "epsilon", "time-delta", "use-skip-forward") or a '}' character
		case EPSILON_WANDER_ARG:
			if ((token == "start-vertex") || (token == "epsilon") || (token == "time-delta") || (token == "use-skip-forward"))
			{
				curr_argument = token;
				curr_state = (  token == "start-vertex"  ) ? (EPSILON_WANDER_INT_VALUES_BEGIN) : (    (  (token == "epsilon") || (token == "time-delta")  ) ? (EPSILON_WANDER_REAL_VALUES_BEGIN) : (EPSILON_WANDER_BOOL_VALUES_BEGIN)    );
				break;
			}
			if (token == "}")
			{
				RUN_EMULATION
				curr_state = GRAPH_BODY;
				break;
			}
			SYNTAX_ERROR("Epsilon wander block contains unknown command '" + token + "'.");

		// expect a ':' character before integer values inside an epsilon-wander block
		case EPSILON_WANDER_INT_VALUES_BEGIN:
			if (token == ":")
			{
				curr_state = EPSILON_WANDER_INT_VALUES;
				break;
			}
			SYNTAX_ERROR("Expected a colon after the name of the parameter '" + curr_argument + "'. Found '" + token + "' instead.");
		
		// expect a ':' character before real values inside an epsilon-wander block
		case EPSILON_WANDER_REAL_VALUES_BEGIN:
			break;
		
		// expect a ':' character before bool values inside an epsilon-wander block
		case EPSILON_WANDER_BOOL_VALUES_BEGIN:
			break;
		
		// expect an integer value, an array of integer values, or geenrator of integer values
		case EPSILON_WANDER_INT_VALUES:
			try
			{
				curr_arguments_uint32.push_back(std::stoi(token));
				break;
			}
			catch (...)
			{
				//
			}
		}
	}
	
	in_file.close();
	return;
}
#undef SYNTAX_ERROR
#undef EMULATION_ERROR





#define PRINT_ERROR(error_type, what) {std::cerr << error_type << ". " << what << '\n'; continue;}
void run(void)
{
	std::string                 command = "";
	std::istringstream          command_stream(command);
	std::vector<std::string>    command_tokens;

	// 1. Load settings
	AppSettings settings = init();

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
				cmd_run(settings, command_tokens[1]);
			}
			catch (std::invalid_argument &e) PRINT_ERROR("FILE ERROR", e.what())
			catch (std::runtime_error &e) PRINT_ERROR("SYNTAX ERROR", e.what())
			catch (std::domain_error &e) PRINT_ERROR("EMULATION ERROR", e.what())

			continue;
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
