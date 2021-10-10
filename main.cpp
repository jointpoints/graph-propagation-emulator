/**
 * @file main.cpp
 * @author JointPoints, 2021, github.com/jointpoints
 */
//#include "ui/ui.hpp"
#include "propagation/propagation.hpp"
#include <iostream>





int main(void)
{
	gpe::Graph g("star2.gexf");
	std::cout << g.get_vertex_count() << ' ' << g.get_edge_count() << '\n';
	gpe::FirstSaturationInfo info{false, false, 0.0001L, 500.0L};
	gpe::first_saturation(g, "0", 0.1, info);
	return 0;
}
