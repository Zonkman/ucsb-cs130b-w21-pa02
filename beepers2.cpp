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

// there are up to 11*1024 subproblems to solve
int SolveHelper(int ptCount, int tabSize, int tabulation[], Point ptInfo[], int idx) {
    if (tabulation[idx] != -1) { return tabulation[idx]; }
    
    int currPoint = idx >> ptCount;
    int powerSize = 1 << ptCount;
    int allPointsMask = powerSize - 1;
    int idxPointsPart = (idx & allPointsMask);

    if (idxPointsPart == allPointsMask) { // complete the cycle! go home
        int cost = (ptInfo[0]).Distance(ptInfo[currPoint]);
        tabulation[idx] = cost;
	return cost;
    }

    int availPoints[ptCount];
    int availPointsCount = 0;

    for (int i = 0; i < ptCount; ++i) {
	if (i == currPoint) { continue; }
        int singleMask = 1 << i;
	if ((idx & singleMask) == 0) {
	    availPoints[availPointsCount] = i; 
	    ++availPointsCount;
	}
    }

    int minSolution = 9999999;

    for (int i = 0; i < availPointsCount; ++i) {
        int cost = (ptInfo[currPoint]).Distance(ptInfo[availPoints[i]]);
        int newIdx = (availPoints[i]*powerSize) + idxPointsPart + (1 << availPoints[i]);
        int currSolution = cost + SolveHelper(ptCount, tabSize, tabulation, ptInfo, newIdx);
	if (currSolution < minSolution) { minSolution = currSolution; }
    }

    tabulation[idx] = minSolution;

    return minSolution;
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

    
    Point ptInfo[1+beeperCount];
    int ptUniqueCount = 1;
    ptInfo[0] = Point(xStart, yStart);

    for (int i = 0; i < beeperCount; ++i) {
	int xBi, yBi; std::cin >> xBi >> yBi;
	bool notUnique = false;
	for (int j = 0; j < ptUniqueCount; ++j) {
	     if (ptInfo[j].x == xBi && ptInfo[j].y == yBi) {
	          notUnique = true; break;
	     }
	}
	if (notUnique) { continue; }
        ptInfo[ptUniqueCount] = Point(xBi, yBi);
	++ptUniqueCount;
    }

    if (ptUniqueCount == 1) { return 0; }

    Point ptInfo2[ptUniqueCount];
    for (int i = 0; i < ptUniqueCount; ++i) { ptInfo2[i] = ptInfo[i]; }

    int minSolution = 9999999;

    int powerSize = 1 << ptUniqueCount;
    int tabSize = ptUniqueCount*powerSize;
    int tabulation[tabSize];
    for (int i = 0; i < tabSize; ++i) { tabulation[i] = -1; }

    // which point to visit first?
    for (int i = 1; i < ptUniqueCount; ++i) {
	int cost = (ptInfo2[0]).Distance(ptInfo2[i]);
        int startIdx = (powerSize*i) + (1 << i) + 1;
	int currSolution = cost + SolveHelper(ptUniqueCount, tabSize, tabulation, ptInfo2, startIdx);
	if (currSolution < minSolution) { minSolution = currSolution; }
    }

    return minSolution;
}

int main(int argc, char** argv) {
    int testCount;
    std::cin >> testCount;

    for (int i = 0; i < testCount; ++i) {
        std::cout << Solve() << std::endl;
    }

    return 0;
}
