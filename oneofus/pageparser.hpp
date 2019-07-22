#ifndef PAGE_PARSER_HPP
#define PAGE_PARSER_HPP

#include <vector>
#include <string>
#include <cstring>
#include <fstream>

class PageParser
{
	public:
		struct Node { int x, y, shape, color; };
	private:
		std::vector<Node> m_board;
		int width, height;
	public:
		inline bool Parse( const char *filename );
		int GetWidth() const { return width; }
		int GetHeight() const { return height; }
		int GetIdx(int i, int j) const { return i * height + j; }
		const std::vector<Node> &GetBoard() const { return m_board; }
		const Node &GetBoard(int i, int j) const { return m_board[ GetIdx(i, j) ]; }
		const Node &GetBoard(int idx) const { return m_board[ idx ]; }
};

inline bool PageParser::Parse( const char *filename )
{
	std::ifstream fin{ filename };
	std::string line;

	char *board_str;
	while(std::getline(fin, line))
	{
		auto pos = line.find("\t\t\tFlashVars=");
		if(pos == std::string::npos) continue;

		board_str = new char[line.length()];
		sscanf(line.c_str(), "\t\t\tFlashVars=\"x=%d&y=%d&board=%s", &width, &height, board_str);

		int len = strlen(board_str) - 1;
		int stride = len / (width * height);

		m_board.resize(width * height);
		for(int i = 0; i < width * height; ++i)
		{
			m_board[i].x = i / height;
			m_board[i].y = i % height;
			int l = i * stride, mid = i * stride + stride / 2, r = (i + 1) * stride;
			for(int x = l; x < mid; ++x)
			{
				m_board[i].shape <<= 8;
				m_board[i].shape += board_str[x];
			}
			for(int x = mid; x < r; ++x)
			{
				m_board[i].color <<= 8;
				m_board[i].color += board_str[x];
			}
		}

		delete[] board_str;

		return true;
	}
	return false;
}

#endif
