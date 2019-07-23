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
#include <chrono>

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
	path.PushBack(to);
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
		tmp.PushInitial(&cur);
		GreedyDfs(&cur, tmp);
		if(tmp.Size() > path.Size() || (tmp.Size() == path.Size() && (mtgen() & 1)))
			path = std::move(tmp);
	}

	//set visit info
	std::vector<Graph::Node *> ordered_vec;
	path.GetOrderedVec(ordered_vec);
	for(Graph::Node *cur : ordered_vec)
		cur->vis = true;
}

inline bool RotationalTransform(Path &path)
{
	if(path.Front()->degree > path.Back()->degree
	   || (path.Front()->degree == path.Back()->degree && (mtgen() & 1)))
		path.Reverse();

	std::vector<Graph::Node *> neis;
	for(Graph::Node *nei : path.Back()->to)
		if(path.Exist(nei))
			neis.push_back(nei);

	if(neis.empty()) return false;

	path.Reverse(neis[ mtgen() % neis.size() ]);
	Graph::Node *target = path.Back();
	GreedyDfs(target, path);

	return true;
}

inline Path HybridHam()
{
	Path path{graph}, tmp{graph};
	int try_cnt = 0;
	do
	{
		path.Clear();
		GenerateInitialPath( path, tmp );
		printf("\rHybridHAM Try #%d: Initial Path Size = %ld / %ld, ", ++try_cnt, path.Size(), graph.GetNodes().size());
		int iter = graph.GetNodes().size();
		while((iter --) && path.Size() < graph.GetNodes().size())
			if( !RotationalTransform(path) ) break;
		printf("Final Path Size = %ld / %ld               ", path.Size(), graph.GetNodes().size());
	} while( path.Size() != graph.GetNodes().size() );
	printf("\n");
	return path;
}

inline std::string ConstructSolution(const Path &path)
{
	std::ostringstream out;
	std::vector<Graph::Node *> ordered_vec;
	path.GetOrderedVec(ordered_vec);
	for(Graph::Node *cur : ordered_vec)
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
	out << "curl " CURL_ARG " --request POST --data " << 
		"\'name=" << USER << "&password=" << PASS << "&path=" << solution << "\'" <<
		" \"http://www.hacker.org/oneofus/index.php\" > " WEBPAGE_FILE "\n";
}


int main()
{
	if(!pp.Parse(WEBPAGE_FILE))
	{
		printf("Error: Previous Solution Failed\n");
		return 0;
	}
	printf("Level Info: Width = %d, Height = %d\n", pp.GetWidth(), pp.GetHeight());

	graph.Construct(pp);

	auto time_start = std::chrono::steady_clock::now();
	Path path = HybridHam();
	auto time_end = std::chrono::steady_clock::now();

	std::chrono::milliseconds time_lasted = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);

	std::string sol = ConstructSolution(path);
	printf("Solution: %.512s(...)\nHybridHAM Time: %.3lf sec\n", sol.c_str(), time_lasted.count() / 1000.0f);
	OutputCurlCmd(sol);
	return 0;
}
