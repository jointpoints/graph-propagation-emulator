/**
 * \file
 *       cmd_setconfig.hpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#ifndef RWE__UI_CMD_SETCONFIG_HPP__
#define RWE__UI_CMD_SETCONFIG_HPP__





#include "../ui_common.hpp"
#include <vector>       // needed for "vector"
#include <map>          // needed for "map"
#include <string>       // needed for "string"
#include <fstream>      // needed for "fstream"
#include <stdexcept>    // needed for exceptions
#include <cstdio>       // needed for "remove"





AppSettings init(void);





enum OptionType
{
	OT_UNKNOWN,
	OT_INT,
	OT_REAL,
	OT_BOOL,
	OT_ALL
};





void cmd_setconfig(AppSettings &settings, std::vector<std::string> const &params)
{
	AppSettings default_settings;

	std::map<std::string, std::pair<uint32_t *const, uint32_t *const> >         int_options
	{
		{"ewsv", {&settings.default_epsilon_wander_params.start_vertex, &default_settings.default_epsilon_wander_params.start_vertex}}
	};
	std::map<std::string, std::pair<long double *const, long double *const> >   real_options
	{
		{"ewe",  {&settings.default_epsilon_wander_params.epsilon, &default_settings.default_epsilon_wander_params.epsilon}},
		{"ewtd", {&settings.default_epsilon_wander_params.time_delta, &default_settings.default_epsilon_wander_params.time_delta}}
	};
	std::map<std::string, std::pair<bool *const, bool *const> >                 bool_options
	{
		{"ewsf", {&settings.default_epsilon_wander_params.use_skip_forward, &default_settings.default_epsilon_wander_params.use_skip_forward}}
	};

	OptionType      curr_option_type    = OT_UNKNOWN;
	std::fstream    config_file;

	// 1. Process params
	if (params.size() == 1)
		throw std::invalid_argument("Expected optname.");
	curr_option_type =   (int_options.find(params[1]) != int_options.end())     ? (OT_INT)      :
	                   ( (real_options.find(params[1]) != real_options.end())   ? (OT_REAL)     :
	                   ( (bool_options.find(params[1]) != bool_options.end())   ? (OT_BOOL)     :
					   ( (params[1] == "*")                                     ? (OT_ALL)      : (OT_UNKNOWN) ) ) );
	if (curr_option_type == OT_UNKNOWN)
		throw std::invalid_argument("Unknown optname '" + params[1] + "'.");

	// 2. If default values should be restored for everything
	if (curr_option_type == OT_ALL)
	{
		if (remove("Technical files/gc"))
			throw std::invalid_argument("Operation cannot be completed. Make sure all files in 'Technical files' directory are closed and untouched.");
		
		settings.default_epsilon_wander_params = init().default_epsilon_wander_params;

		return;
	}

	// 3. If default value should only be changed for a certain option
	if (params.size() == 2)
	{
		config_file.open("Technical files/dc", std::fstream::in | std::fstream::binary);
		if (!config_file.is_open())
			throw std::invalid_argument("Operation cannot be completed. Make sure all files in 'Technical files' directory are closed and untouched.");
		config_file.read(reinterpret_cast<char *>(&default_settings), sizeof(default_settings));
		config_file.close();

		switch (curr_option_type)
		{
		case OT_INT:
			*int_options[params[1]].first = *int_options[params[1]].second;
			break;
		case OT_REAL:
			*real_options[params[1]].first = *real_options[params[1]].second;
			break;
		case OT_BOOL:
			*bool_options[params[1]].first = *bool_options[params[1]].second;
			break;
		case OT_UNKNOWN:
		case OT_ALL:
			break;
		}

		config_file.open("Technical files/gc", std::fstream::out | std::fstream::binary);
		config_file.write(reinterpret_cast<char *const>(&settings), sizeof(settings));
		config_file.close();

		return;
	}

	// 4. If a new value should be set for a certain option
	try
	{
		switch (curr_option_type)
		{
		case OT_INT:
			*int_options[params[1]].first = std::stoi(params[2]);
			break;
		case OT_REAL:
			*real_options[params[1]].first = std::stold(params[2]);
			break;
		case OT_BOOL:
			if ((params[2] != "true") && (params[2] != "false"))
				throw 1;
			*bool_options[params[1]].first = (params[2] == "true");
		case OT_UNKNOWN:
		case OT_ALL:
			break;
		}
	}
	catch (...)
	{
		throw std::invalid_argument("Cannot interpret value '" + params[2] + "' as " + ( (curr_option_type == OT_INT) ? ("integer.") : ( (curr_option_type == OT_REAL) ? ("real.") : ("boolean.") ) ));
	};

	config_file.open("Technical files/gc", std::fstream::out | std::fstream::binary);
	config_file.write(reinterpret_cast<char *const>(&settings), sizeof(settings));
	config_file.close();

	return;
}





#endif
