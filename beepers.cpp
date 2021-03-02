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

// find
int GetRoot(int ptRoots[], int v) {
    int v2 = v;
    for (int i = 0; i < 20; ++i) {
	if (v2 == ptRoots[v2]) { return v2; }
        v2 = ptRoots[v2];
	ptRoots[v2] = ptRoots[ptRoots[v2]]; // compression
    }
    return v2;
}

// union
void SetRoot(int ptRoots[], int newRoot, int v) {
    if (ptRoots[v] != v) {
        SetRoot(ptRoots, newRoot, ptRoots[v]);
    }
    ptRoots[v] = newRoot;
}

// there are up to 3^11 = 177147 subproblems to solve
int SolveHelper(int ptCount, int tabSize, int tabulation[], Point ptInfo[], int ptRoots[], int idx) {
    if (tabulation[idx] != -1) { return tabulation[idx]; }

    //if (idx >= Power(3, ptCount) - 1) { return 0; } // base case: already a cycle

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
        if (adjacencyInfo[i] < 2) {
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
        int ptRootsNew[ptCount];
	for (int j = 0; j < ptCount; ++j) { ptRootsNew[j] = ptRoots[j]; }

	//but then we need to check early cycles and repeats... union find
	if (GetRoot(ptRootsNew, availEdges[0]) == GetRoot(ptRootsNew, availEdges[i])) { continue; }
	
	// recursively set new root	
	SetRoot(ptRootsNew, availEdges[0], availEdges[i]);

        int cost = (ptInfo[availEdges[0]]).Distance(ptInfo[availEdges[i]]);
	int newIdx = idx + Power(3, availEdges[0]) + Power(3, availEdges[i]);
	int subproblem = SolveHelper(ptCount, tabSize, tabulation, ptInfo, ptRootsNew, newIdx);
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

    int ptRoots[1+beeperCount];
    for (int i = 0; i < 1+beeperCount; ++i) { ptRoots[i] = i; }

    return SolveHelper(1+beeperCount, tabSize, tabulation, ptInfo, ptRoots, 0);
}

int main(int argc, char** argv) {
    int testCount;
    std::cin >> testCount;

    for (int i = 0; i < testCount; ++i) {
        std::cout << Solve() << std::endl;
    }

    return 0;
}
