// Copyright (C) 2004  Michael Seyfert <michael@codesand.org>
/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
Solve the 9x9 (or NxN) sudoku puzzle!
This program reads a puzzle board from stdin, and
outputs all solutions.

The goal is to find all solutions that use all numbers 1 through 9
for every row and column. Every 3x3 subsquare must also use all
numbers 1 through 9.
*/

#include<iostream>
#include<bitset>
#include<cstring>
#include<cstdlib>
using namespace std;

typedef int num_t;

static const size_t K = 3; // size of subsquares N = K*K
static const size_t N = K*K; // NxN board
static size_t num_solutions;

//============================================================
// PuzzleBoard
//============================================================
typedef num_t board_t[N][N];
class PuzzleBoard{
public:
	PuzzleBoard();
	PuzzleBoard(board_t b_);

	bool Check()const; //return false if board is invalid

private:

	board_t b;

	friend istream& operator>>(istream &stream,PuzzleBoard &board);
	friend ostream& operator<<(ostream &stream,const PuzzleBoard &board);

	void SetupBitsets(); //setup bitsets, set is_valid
	bitset<N> horz[N]; // which numbers have we already used?
	bitset<N> vert[N];
	bitset<N> subsqr[N/K][N/K]; //this subsquare has which numbers?

	bool is_valid;

	friend void RecurseSolve(const PuzzleBoard &b);
};

istream& operator>>(istream &stream,PuzzleBoard &board);
ostream& operator<<(ostream &stream,const PuzzleBoard &board);
void RecurseSolve(const PuzzleBoard &b);

//============================================================

PuzzleBoard::PuzzleBoard()
{
	for(size_t i = 0;i < N;++i)
		for(size_t j = 0;j < N;++j)
			b[i][j] = 0;
	is_valid = false;
}

PuzzleBoard::PuzzleBoard(board_t b_)
{
	memcpy(&b,&b_,sizeof(board_t));
	SetupBitsets();
}

void PuzzleBoard::SetupBitsets()
{
	size_t i,j;
	num_t t;

	for(i = 0;i < N;i++)for(j = 0;j < N;j++){
		t = b[i][j];
		if(t == 0)continue;

		//horizontal
		//is in range? isn't already used?
		if(t < 0 || t > (num_t)N || horz[i][t-1]){
			is_valid = false;
			return;
		}
		horz[i][t-1] = true;

		//vertical
		if(vert[j][t-1]){
			is_valid = false;
			return;
		}
		vert[j][t-1] = true;

		//subsqr
		int is=(i/K),js=(j/K);
		if(subsqr[is][js][t-1]){
			is_valid = false;
			return;
		}
		subsqr[is][js][t-1] = true;
	}

	is_valid = true;
}

bool PuzzleBoard::Check()const
{
	return is_valid;
}

istream& operator>>(istream &stream,PuzzleBoard &board)
{
	//read a puzzle board from a stream
	num_t t;
	for(size_t i = 0;i < N;++i)
		for(size_t j = 0;j < N;++j){
			stream >> t;
			if(t < 0 || t > (num_t)N) t = 0;
			board.b[i][j] = t;
		}
	board.SetupBitsets();
	return stream;
}

ostream& operator<<(ostream &stream,const PuzzleBoard &board)
{
	//output a puzzle board to stream
	for(size_t i = 0;i < N;++i){
		for(size_t j = 0;j < N;++j)
			stream << ' ' << board.b[i][j];
		stream << '\n';
	}
	return stream;
}

void RecurseSolve(const PuzzleBoard &b)
{
	size_t i,j,k;
	for(i = 0;i < N;++i){
		for(j = 0;j < N;++j){
			if(b.b[i][j] != 0) continue;

			//we found an unused spot, try a new value
			// go through unused horizontal nums
			for(k = 0;k < N;++k){
				if(!b.horz[i][k] && !b.vert[j][k] && !b.subsqr[i/K][j/K][k]){

					//found a possible value for this spot
					PuzzleBoard r(b);
					r.b[i][j] = k + 1;
					r.horz[i][k] = true;
					r.vert[j][k] = true;
					r.subsqr[i/K][j/K][k] = true;

					RecurseSolve(r);
				}
			}
			return; //this isn't a solution, not a leaf

		}
	}

	//b is a solution!
	num_solutions++;
	cout << b << '\n';
}
//============================================================

int main(int argc,char **argv)
{
	//starting puzzle board
	PuzzleBoard board;
	cin >> board;

	if(!board.Check()){
		cout << "Invalid input board.\n";
		return EXIT_FAILURE;
	}

	cout << "Starting board:\n" << board << endl;

	cout << "Solutions:\n";
	RecurseSolve(board);
	cout << "Number of solutions = " << num_solutions << endl;
}
