/**
 * \file
 *       cmd_getconfig.hpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#ifndef RWE__UI_CMD_GETCONFIG_HPP__
#define RWE__UI_CMD_GETCONFIG_HPP__





#include "../ui_common.hpp"
#include <vector>       // needed for "vector"
#include <string>       // needed for "string"
#include <iostream>     // needed for output





void cmd_getconfig(AppSettings const &settings, std::vector<std::string> const &params)
{
	std::cout << "=== RANDOM WALKS EMULATOR CONFIGURATION ===\n\n";
	std::cout << "--- Default epsilon wander parameters ---\n";
	std::cout << "\tStart vertex     [ewsv] :\t" << settings.default_epsilon_wander_params.start_vertex << '\n';
	std::cout << "\tEpsilon          [ewe]  :\t" << settings.default_epsilon_wander_params.epsilon << '\n';
	std::cout << "\tTime delta       [ewtd] :\t" << settings.default_epsilon_wander_params.time_delta << '\n';
	std::cout << "\tUse skip forward [ewsf] :\t" << ((settings.default_epsilon_wander_params.use_skip_forward) ? ("true") : ("false")) << '\n';
	return;
}





#endif
