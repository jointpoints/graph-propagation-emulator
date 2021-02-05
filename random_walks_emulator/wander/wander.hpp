/**
 * \file
 *       wander.hpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#ifndef __WANDER_HPP__
#define __WANDER_HPP__





#include "../metric_graph/metric_graph.hpp"     // needed for "MetricGraph"





namespace rand_walks
{





	/**
	 * \class Wander
	 * \brief Emulator of random walks on metric graphs that calculates time of \f$\varepsilon\f$-saturation
	 * 
	 * \f$\varepsilon\f$<b>-saturation</b> is defined as a state of wander when agent instances
	 * form an \f$\varepsilon\f$-net on each edge. Objects of this class are able to find out the
	 * moment of time (with certain precision) when \f$\varepsilon\f$-saturation of graph occurs for
	 * <em>the first time</em>.
	 * 
	 * Each Wander object at each moment of time is in one of the following states:
	 * * \c ready : Wander object is ready to start emulation;
	 * * \c active : Wander object is in process of emulation;
	 * * \c invalid : underlying MetricGraph object has been changed;
	 * * \c dead : underlying MetricGraph object has been deleted.
	 */
	class Wander
	{



	public:



		/**
		 * Concurrency type
		 * 
		 * Defines the mode of emulation:
		 * * \c none : consecutive (edges are updated one by one);
		 * * \c cpu : concurrent, using CPU (multiple edges are updated at the same time utilising multiple cores of CPU).
		 * 
		 * \warning This is a future feature, currently it has no effect.
		 */
		using Concurrency = enum ConcurrencyEnum {none, cpu};



		/// \name Constructors and destructors
		///@{

		/**
		 * Default constructor
		 * 
		 * Constructs an emulator for a specified MetricGraph object.
		 * 
		 * \param   graph   A metric graph to construct an emulator for.
		 * 
		 * \note Right after the end of construction the new Wander object will be in \c ready state.
		 */
		Wander(MetricGraph const &graph);

		/**
		 * Default destructor
		 * 
		 * Destroys the emulator.
		 */
		~Wander(void);

		///@}



		/// \name Modifiers
		///@{

		/**
		 * Reset the emulator
		 * 
		 * Transfers the emulator into the \c ready state.
		 * 
		 * \note The Wander object needs to be in the \c invalid state in order to be reset.
		 */
		void                reset       (void);

		/**
		 * Run the emulation
		 * 
		 * Runs the emulation of random walks on an underlying MetricGraph object. Emulation starts
		 * at the specified vertex of the graph and ends when the first moment of
		 * \f$\varepsilon\f$-saturation occurs.
		 * 
		 * \param   start_vertex        Vertex where the initial agent instance will be spawned.
		 * \param   epsilon             Parameter \f$\varepsilon\f$ of \f$\varepsilon\f$-saturation.
		 * \param   time_delta          Time step of emulation.
		 * \param   concurrency_type    Emulation mode.
		 * 
		 * \return Time of the first \f$\varepsilon\f$-saturation moment; precision of the answer
		 * depends on the time step of emulation.
		 * 
		 * \warning \c concurrency_type is a future feature, currently it has no effect.
		 * 
		 * \note The Wander object needs to be in the \c ready state in order to be run.
		 * 
		 * \note After the emulation halts, the Wander object is transferred into the \c invalid
		 * state.
		 */
		long double const   run         (uint32_t const start_vertex, long double const epsilon, long double const time_delta = 1e-6L, Concurrency concurrency_type = none);

		/**
		 * Invalidates the emulator
		 * 
		 * Transfers the emulator into the \c invalid state.
		 */
		void                invalidate  (void);

		/**
		 * Kills the emulator
		 * 
		 * Transfers the emulator into the \c dead state.
		 */
		void                kill        (void);

		///@}
	private:
		using WanderState           = enum WanderStateEnum {ready, active, invalid, dead};
		using AgentInstance         = struct {long double position; bool direction : 1;};
		using AgentInstanceList     = std::vector<AgentInstance>;
		using EdgeState             = struct {AgentInstanceList agents; bool is_saturated : 1;};
		using NeighbourhoodState    = std::vector<EdgeState>;
		using GraphState            = std::vector<NeighbourhoodState>;
		using AgentCreationRequest  = struct {std::deque<MetricGraph::Edge> target_edges; std::deque<long double> init_positions; std::deque<bool> init_directions;};

		MetricGraph const   &graph;
		GraphState           graph_state;
		WanderState          wander_state;

		// Modifiers
		AgentCreationRequest updateEdgeState(uint32_t vertex_1, uint32_t vertex_2, long double const epsilon, long double const time_delta);
	};





} // rand_walks





#endif
