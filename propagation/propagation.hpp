/**
 * @file propagation.hpp
 * @author JointPoints, 2021, github.com/jointpoints
 */
#ifndef __GPE_PROPAGATION_HPP__
#define __GPE_PROPAGATION_HPP__





#include "../graph/graph.hpp"





namespace gpe
{





/**
 * @struct FirstSaturationInfo
 * @brief Collection of parameters for @c first_saturation procedure
 *
 * Stores functional properties for a procedure to find the first \f$\varepsilon\f$-sauration
 * moment.
 * 
 * @param use_concurrency   Signifies whether to use multiple threads or not.
 * @param use_skip_forward  Signifies whether to use a skip-forward technique or not.
 *                          Skip-forward decreases computation time but is not suitable
 *                          for frame-by-frame animation.
 * @param time_delta        Signifies time step. Less time step increases precision but
 *                          decreases speed of calculations.
 * @param max_time          Signifies maximal reachable time moment. If no saturation
 *                          happens up to this moment, the procedure halts.
 */
using FirstSaturationInfo = struct FirstSaturationInfo
{
	bool        use_concurrency;
	bool        use_skip_forward;
	long double time_delta;
	long double max_time;
};





/**
 * @brief Calculate the first \f$\varepsion\f$-saturation moment
 *
 * Emulates continuous-time point propagation on a given graph until the first
 * \f$\varepsion\f$-saturation moment.
 * 
 * @param graph             An instance of a graph to run emulation on.
 * @param start_vertex_id   An ID of vertex where the process is supposed to start
 *                          (as it is specified in the original GEXF file).
 * @param epsilon           Value of @f$\varepsion@f$.
 * @param info              Parameters of procedure.
 */
void first_saturation(gpe::Graph const &graph, std::string const start_vertex_id, long double const epsilon, gpe::FirstSaturationInfo const &info);





}; // namespace gpe





#endif // __GPE_PROPAGATION_HPP__

