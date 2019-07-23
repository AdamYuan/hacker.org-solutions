#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "pageparser.hpp"
#include <algorithm>
#include <vector>

class Graph
{
	public:
		struct Node { std::vector<Node *> to; int degree; };
	private:
		std::vector<Node> nodes;
		inline void add_edge( Node *u, Node *v ) { u->to.push_back(v); u->degree ++; }
	public:
		//delete copy behaviours
		Graph() = default;
		Graph( const Graph & ) = delete;
		Graph &operator = ( const Graph & ) = delete;
		Graph( Graph && ) noexcept = default;
		Graph &operator = ( Graph && ) noexcept = default;
		std::vector<Node> &GetNodes() { return nodes; }
		inline void Construct(const PageParser &pp);
		inline const PageParser::Node &GetNodeInfo(const PageParser &pp, const Node *cur) { return pp.GetBoard( cur - nodes.data() ); }

		friend class Path;
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
		size_t nodes_size, path_size;

		struct Splay 
		{
			bool rev_tag; Graph::Node *ptr = nullptr;
			Splay *parent;  union { struct { Splay *lson, *rson; }; Splay *ch[2]; };
			inline void reverse() { std::swap(lson, rson); rev_tag ^= 1; }
			inline void pushdown() { if(rev_tag) { if(lson) lson->reverse(); if(rson) rson->reverse(); rev_tag = false; } }
			inline bool which_son() { return parent->rson == this; }
			inline void rotate()
			{
				bool a = which_son(), b = !a; Splay *fa = parent, *gfa = fa->parent;
				Splay *mov = ch[b]; if(mov) mov->parent = fa; fa->ch[a] = mov;
				parent = gfa; if(gfa) gfa->ch[fa->which_son()] = this;
				fa->parent = this; ch[b] = fa;
			}
			inline void fix() { if(parent) parent->fix(); pushdown(); }
			inline void splay(Splay *&root)
			{
				fix(); root = this;
				while( parent ) {
					if( parent->parent ) (which_son() == parent->which_son() ? parent : this)->rotate();
					rotate();
				}
			}
			inline void pushback(Splay *splay)
			{
				pushdown();
				if(!rson) { rson = splay; splay->parent = this; }
				else rson->pushback( splay );
			} 
			inline void pushvector(std::vector<Graph::Node *> &vec)
			{
				pushdown();
				if(lson) lson->pushvector(vec);
				vec.push_back(ptr);
				if(rson) rson->pushvector(vec);
			}
			inline Splay *back() { pushdown(); return rson ? rson->back() : this; }
			inline Splay *front() { pushdown(); return lson ? lson->front() : this; }
		};
		inline Splay *new_splay( Graph::Node *ptr ) 
		{ 
			Splay *spl = splay_pool.data() + (ptr - nodes_begin_ptr);
			spl->ptr = ptr; spl->lson = spl->rson = spl->parent = nullptr;
			return spl;
		}
		std::vector<Splay> splay_pool; Splay *splay_root;

	public:
		inline void Clear()
		{ 
			splay_pool = std::vector<Splay>(nodes_size);
			splay_root = nullptr;
			path_size = 0;
		}
		Path( const Graph &graph )
		{
			nodes_begin_ptr = graph.nodes.data();
			nodes_size = graph.nodes.size();
			Clear();
		}
		//delete copy behaviours
		Path( const Path & ) = delete;
		Path &operator = ( const Path & ) = delete;
		Path( Path && ) noexcept = default;
		Path &operator = ( Path && ) noexcept = default;

		inline bool Exist(Graph::Node *cur) const { return splay_pool[ cur - nodes_begin_ptr ].ptr; }
		inline void PushInitial(Graph::Node *cur)
		{
			splay_root = new_splay(cur);
			++path_size;
		}
		inline void PushBack(Graph::Node *cur)
		{ 
			Splay *spl = new_splay(cur);
			splay_root->pushback(spl);
			spl->splay(splay_root);
			++path_size;
		}
		inline void Reverse() { splay_root->reverse(); }
		inline void Reverse(Graph::Node *cur) { //reverses sequence after cur
			splay_pool[ cur - nodes_begin_ptr ].splay(splay_root);
			if(splay_root->rson) splay_root->rson->reverse();
		}
		inline void GetOrderedVec(std::vector<Graph::Node *> &vec) const 
		{
			vec.clear();
			vec.reserve(nodes_size);
			splay_root->pushvector(vec); 
		}
		inline size_t Size() const { return path_size; }
		inline const Graph::Node *Back() const { return splay_root->back()->ptr; }
		inline Graph::Node *&Back() { return splay_root->back()->ptr; }
		inline const Graph::Node *Front() const { return splay_root->front()->ptr; }
		inline Graph::Node *&Front() { return splay_root->front()->ptr; }
};

#endif
