#include <iostream>
#include <cstdio>

// "I Wrote This" by Alex Lancaster (6706998)

struct Point {
    int x;
    int y;
    Point(int _x, int _y) { x = _x; y = _y; }
    Point() {x = -1; y = -1; }
    int Distance(Point& other) {
        int xDif = (x - other.x >= 0)?(x - other.x):(other.x - x);
	int yDif = (y - other.y >= 0)?(y - other.y):(other.y - y);
	return xDif + yDif; // manhattan distance
    }
};

int Power(int b, int x) { // naive because small x
    if (x < 0) {return 0; }
    if (x == 0) { return 1; }
    return b*Power(b, x-1);
}

// there are up to 3^11 = 177147 subproblems to solve
int SolveHelper(int ptCount, int tabSize, int tabulation[], Point ptInfo[], int idx) {
    if (tabulation[idx] != -1) { return tabulation[idx]; }

    if (idx >= Power(3, ptCount) - 1) { return 0; } // base case: already a cycle

    // unpack a little
    int adjacencyInfo[ptCount];
    int m = 1;
    for (int i = 0; i < ptCount; ++i) {
        adjacencyInfo[i] = (idx/m)%3;
	m *= 3;
    }

    int availEdges[ptCount];
    int availEdgesCount = 0;
    for (int i = 0; i < ptCount; ++i) {
        if (adjacencyInfo[i] != 2) {
	    availEdges[availEdgesCount] = i; ++availEdgesCount;
	}
    }

    // there has to be two available edges!!
    if (availEdgesCount < 2) { return 0; }
    if (availEdgesCount == 2) { 
        return (ptInfo[availEdges[0]]).Distance(ptInfo[availEdges[1]]);
    }

    int minCounter = 9999999;
    for (int i = 1; i < availEdgesCount; ++i) {
	if (adjacencyInfo[availEdges[0]] == 1 && adjacencyInfo[availEdges[i]] == 1) { continue; }
        int cost = (ptInfo[availEdges[0]]).Distance(ptInfo[availEdges[i]]);
	int newIdx = idx + Power(3, availEdges[0]) + Power(3, availEdges[i]);
	int subproblem = SolveHelper(ptCount, tabSize, tabulation, ptInfo, newIdx);
	if (cost + subproblem < minCounter) {
	    minCounter = cost + subproblem;
	}
    }

    tabulation[idx] = minCounter;
    return minCounter;
}

int Solve() {
    int xSize, ySize; // we probably won't be needing these.
    std::cin >> xSize >> ySize;
    
    int xStart, yStart; std::cin >> xStart >> yStart;

    int beeperCount; std::cin >> beeperCount;
    if (beeperCount == 0) { return 0; }
    if (beeperCount == 1) { 
	Point s = Point(xStart, yStart);
	int xB0, yB0; std::cin >> xB0 >> yB0;
	Point b0 = Point(xB0, yB0);
        return 2*s.Distance(b0);
    }
    if (beeperCount > 10) { return -1; }

    int tabSize = Power(3, 1+beeperCount);
    int tabulation[tabSize];
    for (int i = 0; i < tabSize; ++i) { tabulation[i] = -1; }

    Point ptInfo[1+beeperCount];
    ptInfo[0] = Point(xStart, yStart);

    for (int i = 0; i < beeperCount; ++i) {
	int xBi, yBi; std::cin >> xBi >> yBi;
        ptInfo[1+i] = Point(xBi, yBi);
    }

    return SolveHelper(1+beeperCount, tabSize, tabulation, ptInfo, 0);
}

int main(int argc, char** argv) {
    int testCount;
    std::cin >> testCount;

    for (int i = 0; i < testCount; ++i) {
        std::cout << Solve() << std::endl;
    }

    return 0;
}
