#define WEBPAGE_FILE "webpage"
#define POST_FILE "post.html"
#define CURL_COMMAND_FILE "curl_cmd.sh"
#define USER "Username"
#define PASS "Password"
#define CURL_ARG "-x \"socks5://127.0.0.1:1080/\""

#include <algorithm>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <set>
#include <random>

#include "pageparser.hpp"
#include "graph.hpp"

PageParser pp;
Graph graph;

std::random_device rd{};
std::mt19937 mtgen{ rd() };

inline void GreedyDfs(Graph::Node *cur, Path &path)
{
	cur->vis = true;
	Graph::Node *to = nullptr;
	for(Graph::Node *nei : cur->to)
	{
		if(nei->vis) continue;
		if(mtgen() % 1000 == 0) continue;
		to = nei;
		break;
	}

	if(!to) return;
	path.Push(to);
	GreedyDfs(to, path);
}

inline void GenerateInitialPath(Path &path, Path &tmp)
{
	path.Clear();

	int max_deg = 0;
	for(auto &cur : graph.GetNodes())
		max_deg = std::max(cur.degree, max_deg);

	for(auto &cur : graph.GetNodes())
	{
		if(cur.degree != max_deg) continue;
		graph.ResetVis();

		tmp.Clear();
		tmp.Push(&cur);
		GreedyDfs(&cur, tmp);
		if(tmp.Size() > path.Size() || (tmp.Size() == path.Size() && mtgen() & 1))
			path = std::move(tmp);
	}

	//set visit info
	for(Graph::Node *cur : path.GetVec())
		cur->vis = true;
}

inline bool RotationalTransform(Path &path)
{
	if(path.Front()->degree > path.Back()->degree
	   || (path.Front()->degree == path.Back()->degree && mtgen() & 1))
		std::reverse(path.GetVec().begin(), path.GetVec().end());

	std::vector<Graph::Node *> neis;
	for(Graph::Node *nei : path.Back()->to)
		if(path.Exist(nei))
			neis.push_back(nei);

	if(neis.empty()) return false;

	Graph::Node *nei = neis[ mtgen() % neis.size() ];
	auto it = path.GetVec().begin();
	for(; it != path.GetVec().end(); ++it)
		if(*it == nei) break;

	std::reverse(it + 1, path.GetVec().end());
	Graph::Node *target = path.Back();
	GreedyDfs(target, path);

	return true;
}

inline Path HybridHam()
{
	Path path{graph}, tmp{graph};
	while(true)
	{
		path.Clear();
		GenerateInitialPath( path, tmp );
		fprintf(stderr, "HybridHAM Try: Initial Path Size = %ld / %ld\n", path.Size(), graph.GetNodes().size());
		int iter = graph.GetNodes().size();
		while((iter --) && path.Size() < graph.GetNodes().size())
			if( !RotationalTransform(path) ) break;
		if(path.Size() == graph.GetNodes().size()) return path;
	}
}

inline std::string ConstructSolution(const Path &path)
{
	std::ostringstream out;
	for(Graph::Node *cur : path.GetVec())
	{
		const auto &info = graph.GetNodeInfo(pp, cur);
		out << std::hex << std::uppercase << info.y << "," << info.x;
		if(cur != path.Back()) out << "_";
	}
	return out.str();
}

void OutputCurlCmd(const std::string &solution )
{
	std::ofstream out{ CURL_COMMAND_FILE };
	out << "curl " << CURL_ARG << " --request POST --data " << 
		"\'name=" << USER << "&password=" << PASS << "&path=" << solution << "\'" <<
		" \"http://www.hacker.org/oneofus/index.php\" > " << WEBPAGE_FILE << "\n";
}


int main()
{
	if(!pp.Parse(WEBPAGE_FILE))
	{
		fprintf(stderr, "Previous Solution Failed\n");
		return 0;
	}
	for(int i = 0; i < pp.GetWidth(); ++i)
	{
		for(int j = 0; j < pp.GetHeight(); ++j)
			fprintf(stderr, "%d %d, ", pp.GetBoard(i, j).color, pp.GetBoard(i, j).shape);
		fprintf(stderr, "\n");
	}

	graph.Construct(pp);

	Path path = HybridHam();

	std::string sol = ConstructSolution(path);
	fprintf(stderr, "Solution: %s\n", sol.c_str());
	OutputCurlCmd(sol);

	return 0;
}
