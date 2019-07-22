#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "pageparser.hpp"
#include <algorithm>
#include <vector>

class Graph
{
	friend class Path;
	public:
		struct Node { std::vector<Node *> to; int degree; bool vis; };
	private:
		std::vector<Node> nodes;
		inline void add_edge( Node *u, Node *v ) { u->to.push_back(v); u->degree ++; }
	public:
		std::vector<Node> &GetNodes() { return nodes; }
		inline void Construct(const PageParser &pp);
		inline const PageParser::Node &GetNodeInfo(const PageParser &pp, const Node *cur) { return pp.GetBoard( cur - nodes.data() ); }
		inline void ResetVis() { for(Node &cur : nodes) cur.vis = false; }
};

inline void Graph::Construct( const PageParser &pp )
{
	nodes.resize(pp.GetWidth() * pp.GetHeight());

	for(int i = 0; i < pp.GetWidth(); ++i)
		for(int j = 0; j < pp.GetHeight(); ++j)
		{
			int cur_idx = pp.GetIdx(i, j);
			const PageParser::Node &cur = pp.GetBoard(cur_idx);
			for(int k = 0; k < pp.GetWidth(); ++k)
			{
				if(k == i) continue;

				int nei_idx = pp.GetIdx(k, j);
				const PageParser::Node &nei = pp.GetBoard(nei_idx);
				if(cur.color == nei.color || cur.shape == nei.shape)
					add_edge( nodes.data() + cur_idx, nodes.data() + nei_idx );
			}
			for(int k = 0; k < pp.GetHeight(); ++k)
			{
				if(k == j) continue;

				int nei_idx = pp.GetIdx(i, k);
				const PageParser::Node &nei = pp.GetBoard(nei_idx);
				if(cur.color == nei.color || cur.shape == nei.shape)
					add_edge( nodes.data() + cur_idx, nodes.data() + nei_idx );
			}
		}

	//sort neighbours by its degree
	for(Node &cur : nodes)
		if(!cur.to.empty())
			std::sort(cur.to.begin(), cur.to.end(), [](const Node *l, const Node *r) { return l->degree < r->degree; });
}

class Path 
{ 
	private:
		const Graph::Node *nodes_begin_ptr; 
		size_t nodes_size;
		std::vector<Graph::Node *> vec;
		std::vector<bool> exist;
	public:
		Path( const Graph &graph )
		{
			nodes_begin_ptr = graph.nodes.data();
			nodes_size = graph.nodes.size();
			vec.reserve(graph.nodes.size());
			exist.resize(nodes_size);
		}
		inline void Push(Graph::Node *cur) { vec.push_back(cur); exist[cur - nodes_begin_ptr] = true; }
		inline void Clear() { vec.clear(); exist = std::vector<bool>(nodes_size); }
		inline bool Exist(Graph::Node *cur) const { return exist[ cur - nodes_begin_ptr ]; }
		inline size_t Size() const { return vec.size(); }
		inline const std::vector<Graph::Node *> &GetVec() const { return vec; }
		inline std::vector<Graph::Node *> &GetVec() { return vec; }
		inline const Graph::Node *Back() const { return vec.back(); }
		inline Graph::Node *&Back() { return vec.back(); }
		inline const Graph::Node *Front() const { return vec.front(); }
		inline Graph::Node *&Front() { return vec.front(); }
};

#endif
