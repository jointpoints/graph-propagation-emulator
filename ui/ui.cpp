/**
 * \file
 *       ui.cpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#include "ui.hpp"

#include "cmd/cmd_getconfig.hpp"
#include "cmd/cmd_run.hpp"
#include "cmd/cmd_rweg2gexf.hpp"
#include "cmd/cmd_setconfig.hpp"

#include <fstream>      // needed for "fstream"
#include <stdexcept>    // needed for exceptions
#include <string>       // needed for strings and "getline"
#include <sstream>      // needed for "istringstream"
#include <iterator>     // needed for "istream_iterator"
#ifdef _WIN32
#	include <windows.h>
#	ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#		define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#	endif
#endif





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





// Public functions





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

	std::vector<std::string> const              cmd_names{"getconfig", "run", "rweg2gexf", "setconfig"};
	void                                      (*cmd_functions[])(AppSettings &, std::vector<std::string> const &) = {cmd_getconfig, cmd_run, cmd_rweg2gexf, cmd_setconfig};
	std::string const                           cmd_help[] = {"cmdgc", "cmdr", "cmdrtg", "cmdsc"};
	auto                                        cmd_position = cmd_names.end();

	std::fstream technical;

	// 1. Load settings
	AppSettings settings = init();

#	ifdef _WIN32
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#	endif

	std::cout << "Random Walks Emulator v.0.2 by Andrei Eliseev (JointPoints), 2021\nProject web-site: https://jointpoints.github.io/random-walks/\n";
	std::cout << "Type 'help' to get the full list of available commands with their short descriptions.\n\n";

	// 2. Await for the next command
	while (command != "exit")
	{
		ignore_ws_regime = false;
		token = "";
		command_tokens.clear();

		std::cout << "\033[0;92m>>>\033[0m ";

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
		// 2.3. If user wants to run a command
		if ((cmd_position = std::find(cmd_names.begin(), cmd_names.end(), command_tokens[0])) != cmd_names.end())
		{
			uint8_t cmd_index = std::distance(cmd_names.begin(), cmd_position);

			if ((command_tokens.size() > 1) && (command_tokens[1] == "?"))
				PRINT_HELP(cmd_help[cmd_index])

			try
			{
				cmd_functions[cmd_index](settings, command_tokens);
			}
			catch (std::invalid_argument &e) PRINT_ERROR("EXECUTION ERROR", e.what())
			catch (std::runtime_error &e) PRINT_ERROR("SYNTAX ERROR", e.what())
			catch (std::domain_error &e) PRINT_ERROR("EMULATION ERROR", e.what())

			continue;
		}
		// 2.4. Unknown command
		if (command != "exit")
			PRINT_ERROR("COMMAND LINE ERROR", "Unknown command.")
	}

	return;
}
#undef PRINT_ERROR
