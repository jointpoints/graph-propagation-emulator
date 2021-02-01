/**
 * \file
 *       metric_graph.hpp
 *
 * \author
 *       Andrei Eliseev (JointPoints), 2021
 */
#ifndef __METRIC_GRAPH_HPP__
#define __METRIC_GRAPH_HPP__





#include <vector>   // needed for "vector"
#include <cstdint>  // needed for "int*_t" and "uint*_t" types
#include <string>   // needed for "string"
#include <iostream> // needed for "iostream"





namespace rand_walks
{





	// Forward declaration of Wander class
	class Wander;





	/**
	 * \class MetricGraph
	 * \brief A metric graph class
	 * 
	 * Metric graph is defined as a graph \f$(V,E)\f$ with each edge \f$e \in E\f$ being associated with
	 * a certain interval \f$[0,l(e)]\f$, where \f$l(e) > 0\f$.
	 */
	class MetricGraph
	{
	public:
		/// \name Constructors and destructors
		///@{
		
		/**
		 * Default constructor
		 * 
		 * Constructs an empty metric graph.
		 */
		MetricGraph     (void);

		/**
		 * Default destructor
		 * 
		 * Destroys the metric graph.
		 * 
		 * \warning All \c Wander objects associated with the respective metric graph will be
		 * automatically transferred to the \c dead state.
		 */
		~MetricGraph    (void);

		///@}



		/// \name Accessors
		///@{

		/**
		 * Checks whether vertex is present in the graph
		 * 
		 * This function helps to find out whether a vertex with specified ID is present in the
		 * corresponding graph or not.
		 * 
		 * \param   vertex  Vertex ID.
		 * 
		 * \return
		 * * \c true if vertex is found in graph;
		 * * \c false otherwise.
		 */
		bool const          checkVertex     (uint32_t const vertex)                             const;

		/**
		 * Gets number of vertices in the graph
		 * 
		 * This function helps to find out how many vertices there are in the corresponding graph.
		 * 
		 * \return Number of vertices, i.e. \f$|V|\f$.
		 * 
		 * \warning Yet to be implemented.
		 */
		uint32_t const      getVertexCount  (void)                                              const;

		/**
		 * Gets length of the edge
		 * 
		 * This functions helps to find out the length of the edge connecting vertex \f$v\f$ with
		 * vertex \f$w\f$.
		 * 
		 * \param   out_vertex  ID of vertex \f$v\f$.
		 * \param   in_vertex   ID of vertex \f$w\f$.
		 * 
		 * \return
		 * * Length of the edge, if both \f$v \in V\f$ and \f$w \in V\f$, and either \f$v-w\f$ edge or
		 * \f$v \rightarrow w\f$ edge exists;
		 * * Positive infinity, otherwise.
		 */
		long double const   getEdgeLength   (uint32_t const out_vertex, uint32_t in_vertex)     const;

		/**
		 * Prints edge list into the stream
		 * 
		 * Sends human-readable list of graph edges into the specified output stream.
		 * 
		 * \param   output_stream   Target output stream.
		 * 
		 * \paragraph example Example
		 * Source snippet:
		 * \include{lineno} MetricGraph_outputEdgeList_example.txt
		 * Expected output:
		 * \include MetricGraph_outputEdgeList_output.txt
		 */
		void                outputEdgeList  (std::ostream &output_stream)                       const;

		///@}



		/// \name Modifiers
		///@{
		
		/**
		 * Updates edge characteristics
		 * 
		 * This function adds a new edge or updates already existing one. The exact behaviour depends
		 * on the parameters and prior configuration of graph. Say, we want to add an edge of length \f$l\f$
		 * from vertex \f$v\f$ to vertex \f$w\f$. Possible cases are:
		 * <table>
		 * <tr align="center">
		 *     <th>Is \f$v \in V\f$?</th>
		 *     <th>Is \f$w \in V\f$?</th>
		 *     <th>Type of existing edge</th>
		 *     <th>Desired type of edge</th>
		 *     <th>Result</th>
		 * </tr>
		 * <tr align="center">
		 *     <td>No</td>
		 *     <td>Yes</td>
		 *     <td rowspan="3">None</td>
		 *     <td rowspan="3">Any</td>
		 *     <td rowspan="3" align="left">All non-existing vertices are added to the graph (i.e., \f$V = V \cup \{v,w\}\f$)
		 *         together with desired edge of length \f$l\f$.</td>
		 * </tr>
		 * <tr align="center">
		 *     <td>Yes</td>
		 *     <td>No</td>
		 * </tr>
		 * <tr align="center">
		 *     <td>No</td>
		 *     <td>No</td>
		 * </tr>
		 * <tr align="center">
		 *     <td rowspan="7">Yes</td>
		 *     <td rowspan="7">Yes</td>
		 *     <td>None</td>
		 *     <td>Any</td>
		 *     <td align="left">Desired edge from \f$v\f$ to \f$w\f$ of length \f$l\f$ is added to the graph.</td>
		 * </tr>
		 * <tr align="center">
		 *     <td rowspan="2">\f$v \rightarrow w\f$</td>
		 *     <td>\f$v \rightarrow w\f$</td>
		 *     <td align="left">The length of existing edge \f$v \rightarrow w\f$ is updated to be \f$l\f$.</td>
		 * </tr>
		 * <tr align="center">
		 *     <td>\f$v-w\f$</td>
		 *     <td align="left">Existing edge \f$v \rightarrow w\f$ is replaced with \f$v-w\f$ of length \f$l\f$.</td>
		 * </tr>
		 * <tr align="center">
		 *     <td rowspan="2">\f$v \leftarrow w\f$</td>
		 *     <td>\f$v \rightarrow w\f$</td>
		 *     <td align="left">Existing edge \f$v \leftarrow w\f$ is replaced with \f$v-w\f$ of length \f$l\f$.</td>
		 * </tr>
		 * <tr align="center">
		 *     <td>\f$v-w\f$</td>
		 *     <td align="left">Existing edge \f$v \leftarrow w\f$ is replaced with \f$v-w\f$ of length \f$l\f$.</td>
		 * </tr>
		 * <tr align="center">
		 *     <td rowspan="2">\f$v-w\f$</td>
		 *     <td>\f$v \rightarrow w\f$</td>
		 *     <td align="left">The length of existing edge \f$v-w\f$ is updated to be \f$l\f$.</td>
		 * </tr>
		 * <tr align="center">
		 *     <td>\f$v-w\f$</td>
		 *     <td align="left">The length of existing edge \f$v-w\f$ is updated to be \f$l\f$.</td>
		 * </tr>
		 * </table>
		 * 
		 * \param   out_vertex      ID of vertex \f$v\f$.
		 * \param   in_vertex       ID of vertex \f$w\f$.
		 * \param   length          Desired length of edge between \f$v\f$ and \f$w\f$ (i.e., \f$l\f$).
		 * \param   is_directed     If \c true, desired type of edge will be \f$v \rightarrow w\f$,
		 *                          if \c false, desired type of edge will be \f$v-w\f$.
		 * 
		 * \paragraph example Example
		 * Source snippet:
		 * \include{lineno} MetricGraph_updateEdge_example.txt
		 * Expected output:
		 * \include MetricGraph_updateEdge_output.txt
		 */
		void updateEdge(uint32_t const out_vertex, uint32_t const in_vertex, long double const length, bool const is_directed = false);
		///@}

		// Save/load
		void    toFile      (std::string const file_name = "Saved files/My metric graph")   const;
		void    fromFile    (std::string const file_name);
	private:
		friend class Wander;

		using VertexList            = std::vector<uint32_t>;
		using LengthList            = std::vector<long double>;
		using DirectionList         = std::vector<bool>;
		using VertexNeighbourhood   = struct {uint32_t vertex_id; VertexList connected_vertices; LengthList lengths; DirectionList is_directed;};
		using EdgeList              = std::vector<VertexNeighbourhood>;
		using Edge                  = std::pair<uint32_t, uint32_t>;

		EdgeList                edges;
		std::vector<Wander *>   associated_wanders;

		// Access
		Edge getEdge(uint32_t const out_vertex, uint32_t const in_vertex, bool const is_directed = true, bool const strict_mode = false) const;
	};





} // rand_walks





#endif // __METRIC_GRAPH_HPP__
