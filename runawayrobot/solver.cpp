#include <iostream>
#include <cstdlib>
#include <queue>
#include <cstring>
#define USER "Username"
#define PASS "Password"
#define CURL_ARG "-x \"socks5://127.0.0.1:1080/\""
using namespace std;
int actionMax, actionMin, mapWidth, mapHeight, level;
bool Map[800][800];

struct node
{
	int x, y;
	string Path;
};

bool newMap[500][500];
int row, col;
string Path;
bool bfs()
{
	queue<node> que;
	que.push({0, 0, ""});
	while(!que.empty())
	{
		node current = que.front();
		if(current.x+1 == row && current.y+1 == col)
		{
			Path = current.Path;
			return true;
		}
		que.pop();

		node D = current;
		D.x ++;
		if(D.x < row && ~newMap[D.x][D.y])
		{
			D.Path += 'D';
			newMap[D.x][D.y] = true;
			que.push(D);
		}

		node R = current;
		R.y ++;
		if(R.y < col && ~newMap[R.x][R.y])
		{
			R.Path += 'R';
			newMap[R.x][R.y] = true;
			que.push(R);
		}
	}
	return false;
}
int main()
{
	ios::sync_with_stdio(false);
	string line;
	while(getline(cin, line))
	{
		size_t s;
		if((s = line.find("FVterrainString")) != string::npos)
		{
			line = line.substr(s);
			break;
		}
	}
	size_t split = line.find_first_of('&');
	string str0 = line.substr(0, split);
	string str1 = line.substr(split);

	char mstr[6400000];
	sscanf(str0.c_str(), "FVterrainString=%s", mstr);
	sscanf(str1.c_str(), "&FVinsMax=%d&FVinsMin=%d&FVboardX=%d&FVboardY=%d&FVlevel=%d\">",
			&actionMax, &actionMin, &mapWidth, &mapHeight, &level);

	for(int i=0; i<mapHeight; ++i)
		for(int j=0; j<mapWidth; ++j)
			Map[i][j] = mstr[i * mapWidth + j] == 'X';

	clock_t timer = clock();
	for(int steps = actionMin; steps <= actionMax; ++steps)
		for(col=1; col <= steps + 1; col++)
		{
			row = steps - col + 2;

			for(int i=0; i<row; ++i)
				for(int j=0; j<col; ++j)
				{
					newMap[i][j] = false;
					for(int ox=i, oy=j; ~newMap[i][j] && ox < mapWidth && oy < mapHeight; ox+=row-1, oy+=col-1)
						newMap[i][j] |= Map[ox][oy];
				}

			if(~newMap[0][0] && ~newMap[row-1][col-1] && bfs())
			{
				cout << "curl " << CURL_ARG << " \"http://www.hacker.org/runaway/index.php?name="
					<< USER << "&password=" << PASS << "&path=" << Path << "\" > webpage" << endl;

				cerr << "level=" << level << endl;
				cerr << "width=" << mapWidth << " height=" << mapHeight 
					<< " minStep=" << actionMin << " maxStep=" << actionMax << endl;
				cerr << "solution=" << Path << endl;
				cerr << "time=" << (float)(clock() - timer) / CLOCKS_PER_SEC << " sec" << endl;

				return 0;
			}
		}

	return 1;
}
