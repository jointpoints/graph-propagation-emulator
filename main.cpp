/**
 * @file main.cpp
 * @author JointPoints, 2021, github.com/jointpoints
 */
//#include "ui/ui.hpp"
#include "graph/graph.hpp"
#include <iostream>





int main(void)
{
	gpe::Graph g("star2.gexf");
	std::cout << g.get_vertex_count() << ' ' << g.get_edge_count() << '\n';
	return 0;
}
