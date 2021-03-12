/**
 * \file
 *       ui_common.hpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#ifndef RWE__UI_COMMON_HPP__
#define RWE__UI_COMMON_HPP__





#include <cstdint>





// TRANSITION UNIT ENUM
enum LexStates
{
	SPACE,
	TOKEN
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





#endif
