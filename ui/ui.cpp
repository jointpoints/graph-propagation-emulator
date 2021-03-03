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
#include <regex>        // needed for "regex" and "regex_match"





// TRANSITION UNIT ENUM
enum LexStates
{
	SPACE,
	TOKEN
};





// TRANSITION UNIT ENUM
enum ScenarioParseStates
{
	TOP,                                // expect a top-level command ("graph")
	GRAPH_FILE,                         // expect a file name with a graph
	GRAPH_BODY_BEGIN,                   // expect a '{' character for a graph block
	GRAPH_BODY,                         // expect a graph command ("epsilon-wander") or a '}' character
	EPSILON_WANDER_BEGIN,               // expect a '{' character for an epsilon-wander block
	EPSILON_WANDER_ARG,                 // expect an epsilon-wander argument ("start-vertex", "epsilon", "time-delta", "use-skip-forward") or a '}' character
	EPSILON_WANDER_INT_VALUES_BEGIN,    // expect a ':' character before integer values inside an epsilon-wander block
	EPSILON_WANDER_REAL_VALUES_BEGIN,   // expect a ':' character before real values inside an epsilon-wander block
	EPSILON_WANDER_BOOL_VALUES_BEGIN,   // expect a ':' character before bool values inside an epsilon-wander block
	EPSILON_WANDER_INT_VALUES,          // expect an integer value, an array of integer values, or generator of integer values
	EPSILON_WANDER_REAL_VALUES,         // expect a real value, an array of real values, or generator of real values
	EPSILON_WANDER_BOOL_VALUES,         // expect a bool value or an array of bool values
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
#define SYNTAX_ERROR(what)      throw std::runtime_error(what);
#define EMULATION_ERROR(what)   throw std::domain_error(what);
#define RUN_EMULATION           if (epsilon_wander_start_vertex.size() == 0) epsilon_wander_start_vertex.push_back(settings.default_epsilon_wander_params.start_vertex); \
                                if (epsilon_wander_epsilon.size() == 0) epsilon_wander_epsilon.push_back(settings.default_epsilon_wander_params.epsilon); \
                                if (epsilon_wander_time_delta.size() == 0) epsilon_wander_time_delta.push_back(settings.default_epsilon_wander_params.time_delta); \
                                if (epsilon_wander_use_skip_forward.size() == 0) epsilon_wander_use_skip_forward.push_back(settings.default_epsilon_wander_params.use_skip_forward); \
                                for (uint32_t start_vertex_i = 0; start_vertex_i < epsilon_wander_start_vertex.size(); ++start_vertex_i) \
                                for (uint32_t epsilon_i = 0; epsilon_i < epsilon_wander_epsilon.size(); ++epsilon_i) \
                                for (uint32_t time_delta_i = 0; time_delta_i < epsilon_wander_time_delta.size(); ++time_delta_i) \
                                for (uint32_t use_skip_forward_i = 0; use_skip_forward_i < epsilon_wander_use_skip_forward.size(); ++use_skip_forward_i) \
                                { \
                                    epsilon_wander.reset(); \
                                    try \
                                    { \
                                        std::cout << epsilon_wander.run(epsilon_wander_start_vertex[start_vertex_i], epsilon_wander_epsilon[epsilon_i], epsilon_wander_time_delta[time_delta_i], epsilon_wander_use_skip_forward[use_skip_forward_i]) << '\n'; \
                                    } \
                                    catch (std::invalid_argument &e) {EMULATION_ERROR("The start vertex does not exist.");} \
                                    catch (std::logic_error &e) {EMULATION_ERROR("Unknown exception.");} \
								} \
                                epsilon_wander_start_vertex.clear(); epsilon_wander_epsilon.clear(); epsilon_wander_time_delta.clear(); epsilon_wander_use_skip_forward.clear();
void cmd_run(AppSettings const & settings, std::string scenario_path)
{
	std::string const           file_format         = ".rwes";
	std::fstream                in_file;
	char                        symbol              = 0;
	bool                        ignore_ws_regime    = false;
	std::string                 token               = "";
	LexStates                   lexer_state         = SPACE;

	std::vector<std::string>    tokens;
	std::string                 curr_argument_string;
	ScenarioParseStates         parser_state = TOP;

	rand_walks::MetricGraph     graph;

	rand_walks::Wander          epsilon_wander(graph);
	std::vector<uint32_t>       epsilon_wander_start_vertex;
	std::vector<long double>    epsilon_wander_epsilon;
	std::vector<long double>    epsilon_wander_time_delta;
	std::vector<bool>           epsilon_wander_use_skip_forward;

	std::vector<uint32_t>      *curr_int_params     = nullptr;
	std::vector<long double>   *curr_real_params    = nullptr;
	std::vector<bool>          *curr_bool_params    = nullptr;

	// 1. Open scenario file
	in_file.open(((scenario_path.size() >= file_format.size()) && (scenario_path.substr(scenario_path.size() - file_format.size()) == file_format)) ? (scenario_path) : (scenario_path + file_format), std::fstream::in);
	if (!in_file.is_open())
		throw std::invalid_argument("Scenario '" + scenario_path + "' was not found.");
	
	// 2. Read and tokenise scenario
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
			if (symbol == '"')
				ignore_ws_regime = !ignore_ws_regime;
			else
				token += symbol;
			break;
		}
	}
	if (token != "")
		tokens.push_back(token);
	in_file.close();
	
	// 3. Parse and execute scenario
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
			graph = rand_walks::MetricGraph();
			graph.fromFile(tokens[token_i]);
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
		
		// expect a graph command ("epsilon-wander") or a '}' character
		case GRAPH_BODY:
			if (tokens[token_i] == "epsilon-wander")
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
		
		// expect a '{' character for an epsilon-wander block
		case EPSILON_WANDER_BEGIN:
			if (tokens[token_i] == "{")
			{
				parser_state = EPSILON_WANDER_ARG;
				break;
			}
			SYNTAX_ERROR("Expected an opening of epsilon-wander block. Found '" + tokens[token_i] + "' instead.");

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
				RUN_EMULATION
				parser_state = GRAPH_BODY;
				break;
			}
			SYNTAX_ERROR("Epsilon wander block contains unknown command '" + tokens[token_i] + "'.");

		// expect a ':' character before integer values inside an epsilon-wander block
		case EPSILON_WANDER_INT_VALUES_BEGIN:
			if (tokens[token_i] == ":")
			{
				parser_state = EPSILON_WANDER_INT_VALUES;
				break;
			}
			SYNTAX_ERROR("Expected a colon after the name of the parameter '" + curr_argument_string + "'. Found '" + tokens[token_i] + "' instead.");
		
		// expect a ':' character before real values inside an epsilon-wander block
		case EPSILON_WANDER_REAL_VALUES_BEGIN:
			if (tokens[token_i] == ":")
			{
				parser_state = EPSILON_WANDER_REAL_VALUES;
				break;
			}
			SYNTAX_ERROR("Expected a colon after the name of the parameter '" + curr_argument_string + "'. Found '" + tokens[token_i] + "' instead.");
		
		// expect a ':' character before bool values inside an epsilon-wander block
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
				RUN_EMULATION
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
				RUN_EMULATION
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
					while (array_elem <= array_stop)
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
				RUN_EMULATION
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





#define PRINT_ERROR(error_type, what) {std::cerr << error_type << ". " << what << '\n'; continue;}
#define PRINT_HELP(file)   {technical.open("Technical files/" + std::string(file), std::fstream::in); \
                            if (!technical.is_open()) \
                            { \
                                technical.close(); \
                                PRINT_ERROR("TECHNICAL FILES ERROR", "Some technical files are missing. Reinstall the program.") \
                            } \
                            std::string line; \
                            while (std::getline(technical, line)) \
                            std::cout << line << '\n'; \
                            technical.close(); \
                            continue;}
void run(void)
{
	std::string                 command = "";
	std::istringstream          command_stream(command);
	LexStates                   lexer_state = SPACE;
	char                        command_symbol = 0;
	bool                        ignore_ws_regime    = false;
	std::string                 token               = "";
	std::vector<std::string>    command_tokens;

	std::fstream technical;

	// 1. Load settings
	AppSettings settings = init();

	std::cout << "Random Walks Emulator v.0.1 by Andrei Eliseev (JointPoints), 2021\nProject web-site: https://jointpoints.github.io/random-walks/\n";
	std::cout << "Type 'help' to get the full list of available commands with their short descriptions.\n\n";

	// 2. Await for the next command
	while (command != "exit")
	{
		ignore_ws_regime = false;
		token = "";
		command_tokens.clear();

		std::cout << ">>> ";

		std::getline(std::cin, command);
		command_stream = std::istringstream(command);

		// 2.1. Tokenise command
		while (command_stream >> std::noskipws >> command_symbol)
		{
			switch (lexer_state)
			{
			// expect whitespaces
			case SPACE:
				if ((command_symbol == ' ') || (command_symbol == '\t') || (command_symbol == '\r'))
					break;
				lexer_state = TOKEN;
			// expect token
			case TOKEN:
				if (!ignore_ws_regime)
				{
					if ((command_symbol == ' ') || (command_symbol == '\t') || (command_symbol == '\r'))
					{
						if (token != "")
						{
							command_tokens.push_back(token);
							token = "";
						}
						lexer_state = SPACE;
						break;
					}
				}
				if (command_symbol == '"')
					ignore_ws_regime = !ignore_ws_regime;
				else
					token += command_symbol;
				break;
			}
		}
		if (token != "")
			command_tokens.push_back(token);

		// 2.1. If nothing was entered
		if (command_tokens.size() == 0)
			continue;
		// 2.2. If user wants to get help
		if (command_tokens[0] == "help")
		{
			if (command_tokens.size() > 1)
				PRINT_ERROR("COMMAND LINE ERROR", "Unexpected command after 'help'.")

			PRINT_HELP("cmdh")

			continue;
		}
		// 2.3. If user wants to run a scenario
		if (command_tokens[0] == "run")
		{
			if (command_tokens.size() < 2)
				PRINT_ERROR("COMMAND LINE ERROR", "Scenario file name was expected after 'run'.")

			if (command_tokens[1] == "?")
				PRINT_HELP("cmdr")

			try
			{
				cmd_run(settings, command_tokens[1]);
			}
			catch (std::invalid_argument &e) PRINT_ERROR("FILE ERROR", e.what())
			catch (std::runtime_error &e) PRINT_ERROR("SYNTAX ERROR", e.what())
			catch (std::domain_error &e) PRINT_ERROR("EMULATION ERROR", e.what())

			continue;
		}
		// 2.n. Unknown command
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
