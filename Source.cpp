#include <array>
#include <fstream>
#include <stack>
#include <set>
#include <unordered_set>
#include <functional>
#include <vector>
#include <queue>
#include <iostream>

using std::stack;
using std::unordered_set;
using std::greater;
using std::array;
using std::cout;
using std::endl;
using std::abs;
using std::priority_queue;
using std::vector;
using std::swap;
using std::ostream;
using std::hash;
using std::ofstream;
using std::queue;

enum class Move { UP, DOWN, LEFT, RIGHT, NONE};
class Node {
public:

	Node(array<int,9> b, Node * f = nullptr, int d = 0, Move m = Move::NONE) : board(b), father(f), distance(d), move(m)
	{
		Mdistance = getManhatten();
	}

	//Node * getFather() {return father;}
	int getManhatten () const;
	int getDistance() const {return distance;}
	int getHeuristicValue () const {return Mdistance + distance ; }

	bool operator>(const Node& that) const
	{ return this->getHeuristicValue() > that.getHeuristicValue() ;}

	vector<Node> getSuccessors() const;
	Move getMove() {return move; }

	size_t getHashValue ()const{

		size_t seed = 0;
		for (auto  v : board)
			seed ^= v + 0x9e3779b9 + (seed << 6) + (seed >> 2);

		return seed;


	}

	bool isGoal () const {return Mdistance == 0;}
	friend inline bool operator==(const Node & lhs, const Node & rhs){ return lhs.board == rhs.board;}
	friend inline bool operator!=(const Node & lhs, const Node & rhs){ return ! operator==(lhs,rhs) ;}

	friend ostream& operator<<( ostream& out, const Node & n){
		for (size_t i = 0; i < n.board.size(); i++){
			out<<n.board.at(i)<<' ';
			if ( (i+1)%3 == 0)
				out<<endl;
		}

		return out;
	}
	Node * father;
private:


	Move move;
	array<int, 9> board;
	int Mdistance;
	int getCorrectCol(int tileValue) const { return tileValue%3;}
	int getCorrectRow(int tileValue) const { return tileValue/3;}

	int distance;

};

int Node::getManhatten() const {

	int sum = 0;

	int curRow;
	int curCol;
	int tileValue;
	for (size_t i = 0; i < board.size(); i++){
		tileValue = board.at(i);
		curRow = i/3;
		curCol = i%3;
		sum +=  abs(getCorrectCol(tileValue) - curCol) + abs (getCorrectRow(tileValue) - curRow);
	}

	return sum;

}
vector<Node> Node:: getSuccessors() const{

	vector<Node> successors;
	int zeroIndex;
	int zeroCol;
	int zeroRow;

	// find the index of the empty tile
	for ( size_t i = 0; i < board.size(); i++){
		if (board.at(i) == 0){
			zeroIndex = i;
			zeroCol = i%3;
			zeroRow = i/3;
			break;
		}
	}


	array<int,9> temp;
	int swapIndex;

	Node * ff = new Node(*this);
	if (zeroRow > 0 /*&& move != Move::DOWN */){ // swap with upper tile
		swapIndex = (zeroRow  - 1) * 3 + zeroCol;
		temp = board;
		swap(temp.at(zeroIndex), temp.at(swapIndex));
		Node s(temp, ff, distance + 1, Move::UP);
		successors.push_back(s);
	}

	if (zeroRow < 2 /*&& move != Move::UP*/) { //swap with lower tile
		swapIndex = (zeroRow + 1) * 3 + zeroCol;
		temp = board;
		swap(temp.at(zeroIndex), temp.at(swapIndex));
		Node s(temp, ff, distance + 1, Move::DOWN);
		successors.push_back(s);
	}

	if (zeroCol > 0 /*&& move != Move::RIGHT*/) { // swap with left tile
		swapIndex = zeroRow * 3 + zeroCol -1 ;
		temp = board;
		swap(temp.at(zeroIndex), temp.at(swapIndex));
		Node s(temp, ff, distance + 1, Move::LEFT);
		successors.push_back(s);
	}

	if (zeroCol < 2 /*&& move != Move::LEFT*/) { // swap with right tile
		swapIndex = zeroRow * 3 + zeroCol + 1 ;
		temp = board;
		swap(temp.at(zeroIndex), temp.at(swapIndex));
		Node s(temp, ff, distance + 1,Move::RIGHT);
		successors.push_back(s);
	}

	return successors;

}

namespace std {
	template <>	struct hash<Node>
	{

		size_t operator()(const Node & t) const
		{
			return t.getHashValue();
		}
	};
}

int AStarTreeVersion (Node initialState){
	priority_queue<Node, vector<Node>, greater<Node> > fringe;
	fringe.push(initialState);

	while (true){

		if (fringe.empty()) // no solution
			return -1;

		Node current = fringe.top();
		fringe.pop();

		if (current.isGoal())
			return current.getDistance();

		auto successors = current.getSuccessors();

		for (auto s : successors)

			if (s != current)
				fringe.push(s);

	}

}

int AStarGraphVersion(Node initialState){
	priority_queue<Node, vector<Node>, greater<Node> > fringe;
	fringe.push(initialState);

	unordered_set<Node> visited; // <---
//	visited.insert(initialState);// <---


	while (true){

		if (fringe.empty()) // no solution
			return -1;

		Node current = fringe.top();
		fringe.pop();

		if (visited.insert(current).second == false) continue;
		if (current.isGoal())
			return current.getDistance();

		auto successors = current.getSuccessors();

		for (auto s : successors){
				fringe.push(s); //<--
		}
	}
}




int getNodesAway(Node initialState, int depth){
	
	queue<Node> fringe;
	fringe.push(initialState);

	unordered_set<Node> visited; // <---
//	visited.insert(initialState);// <---
	int count = 0;

	while (true){

		if (fringe.empty()) // no solution
			return count;

		Node current = fringe.front();
		fringe.pop();
		

		if (visited.insert(current).second == false) continue;
		
		if (current.getDistance() == depth)
		{
			count++;
			continue;
		}

		auto successors = current.getSuccessors();

		for (auto s : successors){
			if (s != current)
					fringe.push(s); //<--
		}
	}
}






void solve(){
	array<int, 9> a=  {8,1,7,4,5,6,2,0,3};
	array<int, 9> goal =  {0, 1, 2, 3, 4, 5, 6, 7, 8};
	Node ini(a);


	cout<<"Graph solution "<<AStarGraphVersion(ini)<<endl;
	//cout<<"tree solution "<<AStarTreeVersion(ini)<<endl;
	

}


int main () {



	array<int, 9> goal =  {0, 1, 2, 3, 4, 5, 6, 7, 8};
	array<int, 9> a =  {1, 6, 4, 8, 7, 0, 3, 2, 5};
	array<int, 9> c =  {1, 6, 4, 8, 7, 0, 3, 2, 5};
	Node ini(a);
	Node n (a);
	Node b(goal);
	//cout<<n.getManhatten()<<endl;
	//cout<<"original case is\n"<<ini<<endl<<"going six level down and then up"<<endl;
	//
	//auto s = ini.getSuccessors(); // level one
	//s = s.at(0).getSuccessors(); //level two
	//s = s.at(0).getSuccessors(); //level three
	//s = s.at(0).getSuccessors(); //level four
	//s = s.at(0).getSuccessors(); //level five
	//s = s.at(0).getSuccessors(); //level six
	//Node  * temp = &s.at(0);

	//while (temp != nullptr){
	//	cout<<*temp<<endl;
	//	temp = temp->father;
	//}
	//
	//solve();
	
	cout<<getNodesAway(b,27)<<endl;
	
	


	system("pause");
}

