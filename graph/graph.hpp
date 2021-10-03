/**
 * @file graph.hpp
 * @author JointPoints, 2021, github.com/jointpoints
 */
#ifndef __GPE_GRAPH_HPP__
#define __GPE_GRAPH_HPP__





#include <set>
#include <vector>
#include <string>





/**
 * @namespace gpe
 * @brief Namespace containing all core components of Graph Propagation Emulator
 *
 * This namespace unites together containers for graph topology and propagation
 * emulations.
 */
namespace gpe
{





/**
 * @class Graph
 * @brief A container for graph
 *
 * This class is a container for graphs with the following properties:
 * 
 *  * graph is @b allowed to be directed;
 *  * graph is @b allowed to be weighted;
 *  * graph is @b allowed to have cycles;
 *  * graph is @b not @b allowed to have duplicate edges.
 * 
 * Container is intended to provide fast checks of edge existence and fast iteration over
 * vertices and edges, however, it does not provide functions for graph modification at
 * all (they would have been relatively slow, if existed). The only way to alter the
 * structure of the graph is to load it from the GEXF file.
 */
class Graph final
{



private:

	/// Represents edge
	using Edge                  = struct Edge{uint64_t const target; long double const length; bool const inline operator==(uint64_t const other) const {return this->target == other;};};
	/// Stroes vertices which are adjacent to some vertex
	using EdgeLess              = struct {bool const inline operator()(Edge const &edge1, Edge const &edge2) const {return edge1.target < edge2.target;};};
	using VertexNeighbourhood   = std::set<Edge, EdgeLess>;
	/// Stores data about vertex
	using Vertex                = struct Vertex{std::string id; std::string label; VertexNeighbourhood neighbours; bool const inline operator==(std::string const id) const {return this->id == id;};};

	/// Stores list of all vertices and, altogether, the set of all edges
	std::vector<Vertex> self;



public:
	
	/// @name Constructors and destructors
	/// @{
	
	/**
	 * @brief Default constructor
	 *
	 * Creates empty graph.
	 */
	Graph(void) = default;

	/**
	 * @brief Read-from-file constructor
	 *
	 * Reads a graph from a given GEXF file, replaces the existing graph with the new one.
	 * 
	 * @param gexf_path A path to the .GEXF file.
	 * 
	 * @throw gpe::error::FileDoesNotExist if @c gexf_path does not exist.
	 * @throw gpe::error::WrongFileFormat if @c gexf_path is not a proper .GEXF file.
	 * @throw gpe::error::WrongFileFormat if @c gexf_path contains edges with non-positive
	 * weights.
	 * @throw gpe::error::WrongFileFormat if @c gexf_path contains repeating/overlapping
	 * edges.
	 * 
	 * @warning Edge type @c mutual is interpreted as @c undirected.
	 */
	explicit Graph(std::string const gexf_path);

	/**
	 * @brief Default destructor
	 *
	 * Deletes the graph.
	 */
	~Graph(void) = default;

	/// @}



	/// @name Access
	/// @{

	/**
	 * @brief Number of edges
	 *
	 * Counts the number of edges (both directed and undirected) in the graph.
	 * 
	 * @return Number of edges.
	 */
	size_t const get_edge_count(void) const;

	/**
	 * @brief Number of vertices
	 *
	 * Counts the number of vertices in the graph.
	 * 
	 * @return Number of vertices.
	 */
	size_t const get_vertex_count(void) const;

	/**
	 * @brief Check connectivity
	 *
	 * Checks whether a given graph strongly connected.
	 */
	bool const is_connected(void) const;

	/// @}



}; // class Graph





}; // namespace gpe





#endif // __GPE_GRAPH_HPP__
