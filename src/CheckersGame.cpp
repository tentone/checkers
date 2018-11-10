#include <string>
#include <iostream>

#ifndef CHECKERS_GAME_CPP
#define CHECKERS_GAME_CPP

#define BLOCK_EMPTY 0
#define BLOCK_P1 1
#define BLOCK_P2 2

using namespace std;

class CheckersGame
{
	public:
		int board[8][8];

		//Create a new game
		CheckersGame()
		{
			int i, j;
			for(i = 0; i < 8; i++)
			{
				for(j = 0; j < 8; j++)
				{
					this -> board[i][j] = 0;
				}
			}
		}

		//Print game board to cout
		void print()
		{
			int i, j;

			cout << "[";
			for(i = 0; i < 8; i++)
			{
				for(j = 0; j < 8; j++)
				{
					cout << this->board[i][j] << ", ";
				}

				if(i == 7)
				{
					cout << "]";
				}
				else
				{
					cout << "\n";
				}
			}
			cout << endl;
		}
};

#endif
