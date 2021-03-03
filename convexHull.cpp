#include <iostream>
#include <cstdio>
#include <algorithm>
#include <unordered_set>

// "I Wrote This" by Alex Lancaster (6706998)

struct Point {
    long x; //long just in case cross product gets stupid
    long y;
    Point(int _x, int _y) {
        x = _x; y = _y;
    }
    Point() {
        x = -1000000; y = -1000000;
    }
    int ManhattanDistance(Point& other) {
        int xDist = (other.x - x >= 0)?(other.x - x):(x - other.x);
	int yDist = (other.y - y >= 0)?(other.y - y):(y - other.y);
	return xDist + yDist;
    }
};

// determine the winding order of a triangle.
// positive: counterclockwise
// negative: clockwise (we don't like this)
// zero: colinear
// this is just a cross product
int WindingOrder(Point& a, Point& b, Point& c) {
    long r = - ((b.y - a.y)*(c.x - b.x)) + ((b.x - a.x)*(c.y - b.y));
    if (r > 0) { return 1; } if (r < 0) { return -1; }
    return 0;
}

// if there are ever any segfaults the error is probably here
int GetMedianX(Point pts[], int ptsCount) {
    int xList[ptsCount];
    for (int i = 0; i < ptsCount; ++i) { xList[i] = (int)((pts[i]).x); }
    std::nth_element(xList, xList + (ptsCount/2), xList + ptsCount);
    return xList[ptsCount/2];
}

// naturally the solution is the points in counterclockwise order
void SolveHelper(Point pts[], int ptsCount, Point solution[], int& solutionCount) {
    
    // base case when there are 1, 2, or 3 points
    if (ptsCount == 1) { solutionCount = 1; solution[0] = pts[0]; return; }
    if (ptsCount == 2) { solutionCount = 2; solution[0] = pts[0]; solution[1] = pts[1]; return; }
    if (ptsCount == 3) { 
        int w = WindingOrder(pts[0], pts[1], pts[2]);
	solutionCount = 3;
	if (w > 0) { solution[0] = pts[0]; solution[1] = pts[1]; solution[2] = pts[2]; }
	else if (w < 0) { solution[0] = pts[0]; solution[1] = pts[2]; solution[2] = pts[1]; }
	else { // colinear... pick two
	    solutionCount = 2;
	    int d01 = (pts[0]).ManhattanDistance(pts[1]);
	    int d02 = (pts[0]).ManhattanDistance(pts[2]);
	    int d12 = (pts[1]).ManhattanDistance(pts[2]);

	    if (d01 >= d02 && d01 >= d12) { solution[0] = pts[0]; solution[1] = pts[1]; }
	    else if (d02 >= d01 && d02 >= d12) { solution[0] = pts[0]; solution[1] = pts[2]; }
	    else { solution[0] = pts[1]; solution[1] = pts[2]; }
	}
	return;
    }

    //divide the points into left and right sides and solve the subproblems
    int pivot = GetMedianX(pts, ptsCount);

    Point leftPts[ptsCount];
    int leftPtsCount = 0;
    Point rightPts[ptsCount];
    int rightPtsCount = 0;
    Point medianPts[ptsCount];
    int medianPtsCount = 0;

    for (int i = 0; i < ptsCount; ++i) {
        if ((pts[i]).x < pivot) { leftPts[leftPtsCount] = pts[i]; ++leftPtsCount; }
	else if ((pts[i].x) > pivot) { rightPts[rightPtsCount] = pts[i]; ++rightPtsCount; }
	else { medianPts[medianPtsCount] = pts[i]; ++medianPtsCount; }
    }

    if (leftPtsCount == 0 && rightPtsCount == 0) {
        // they're all in a vertical line on the median.
	// this edge case succs lol
        Point highestPoint = medianPts[0]; Point lowestPoint = medianPts[0];
	// there has to be two or more different points here. no way they're the same.
	for (int i = 1; i < medianPtsCount; ++i) {
	    if (medianPts[i].y > highestPoint.y) {
	        highestPoint = medianPts[i];
	    }
	    if (medianPts[i].y < lowestPoint.y) {
	        lowestPoint = medianPts[i];
	    }
	}
	solutionCount = 2;
	solution[0] = lowestPoint; solution[1] = highestPoint; return;
    }

    // otherwise give median points to whichever subproblem has less; neither can be empty
    if (leftPtsCount < rightPtsCount) {
        for (int i = 0; i < medianPtsCount; ++i) {
	    leftPts[leftPtsCount] = medianPts[i]; ++leftPtsCount;
	}
    }
    else {
        for (int i = 0; i < medianPtsCount; ++i) {
	    rightPts[rightPtsCount] = medianPts[i]; ++rightPtsCount;
	}
    }

    Point leftSolution[leftPtsCount];
    int leftSolutionCount = 0;
    Point rightSolution[rightPtsCount];
    int rightSolutionCount = 0;

    SolveHelper(leftPts, leftPtsCount, leftSolution, leftSolutionCount);
    SolveHelper(rightPts, rightPtsCount, rightSolution, rightSolutionCount);

    // now we have to merge the left and right solutions by finding the tangents

    // starting positions
    int rightmostOfLeftIdx = 0;
    for (int i = 1; i < leftSolutionCount; ++i) {
        if (leftSolution[i].x > leftSolution[rightmostOfLeftIdx].x) {
            rightmostOfLeftIdx = i;
	}	
    }

    int leftmostOfRightIdx = 0;
    for (int i = 1; i < rightSolutionCount; ++i) {
        if (rightSolution[i].x < rightSolution[leftmostOfRightIdx].x) {
	    leftmostOfRightIdx = i;
	}	
    }

    // upper tangent
    int rightUpper = leftmostOfRightIdx;
    int leftUpper = rightmostOfLeftIdx;
    bool found = false;
    // exploit the winding of triangles to check if the tangent intersects.
    while (!found) {
        found = true;
	while (WindingOrder(rightSolution[rightUpper],
			    leftSolution[leftUpper],
			    leftSolution[(leftUpper+1)%leftSolutionCount]) < 0) {
	    found = false;
	    leftUpper = (leftUpper+1)%leftSolutionCount; // move counterclockwise
	}
	while (WindingOrder(leftSolution[leftUpper],
			    rightSolution[rightUpper],
			    rightSolution[(rightUpper+rightSolutionCount-1)%rightSolutionCount]) > 0) {
	    found = false;
	    rightUpper = (rightUpper+rightSolutionCount-1)%rightSolutionCount; // move clockwise
	}
    }

    int rightLower = leftmostOfRightIdx;
    int leftLower = rightmostOfLeftIdx;
    found = false;
    // exploit the winding of triangles to check if the tangent intersects.
    while (!found) {
        found = true;
	while (WindingOrder(rightSolution[rightLower],
			    leftSolution[leftLower],
			    leftSolution[(leftLower+leftSolutionCount-1)%leftSolutionCount]) > 0) {
	    found = false;
	    leftLower = (leftLower+leftSolutionCount-1)%leftSolutionCount; // move clockwise
	}
	while (WindingOrder(leftSolution[leftLower],
			    rightSolution[rightLower],
			    rightSolution[(rightLower+1)%rightSolutionCount]) < 0) {
	    found = false;
	    rightLower = (rightLower+1)%rightSolutionCount; // move counterclockwise
	}
    }

    if (leftLower == leftUpper && rightLower == rightUpper) {
        // the only way this could have happened is if all points are colinear,
	// and it is not a vertical line.
	// so just choose the extremes of the line as found by x-position
	Point lowestPoint = pts[0]; Point highestPoint = pts[0];
	for (int i = 1; i < ptsCount; ++i) {
	    if (pts[i].x > highestPoint.x) {
	        highestPoint = pts[i];
	    }
	    if (pts[i].x < lowestPoint.x) {
	        lowestPoint = pts[i];
	    }
	}
	solutionCount = 2;
	solution[0] = lowestPoint; solution[1] = highestPoint; return;
    }

    Point almostSolution[ptsCount];
    int almostSolutionCount = 1;
    almostSolution[0] = leftSolution[leftUpper];
    int currLeftIdx = leftUpper;

    /*std::cout << "left upper is " << leftSolution[leftUpper].x << " " << leftSolution[leftUpper].y;
    std::cout << std::endl;

    std::cout << "left lower is " << leftSolution[leftLower].x << " " << leftSolution[leftLower].y;
    std::cout << std::endl;

    std::cout << "rt upper is " << leftSolution[rightUpper].x << " " << leftSolution[rightUpper].y;
    std::cout << std::endl;

    std::cout << "rt lower is " << leftSolution[rightLower].x << " " << leftSolution[rightLower].y;
    std::cout << std::endl;*/

    // go counterclockwise on the left until added leftLower
    while (currLeftIdx != leftLower) {
	currLeftIdx = (currLeftIdx+1)%leftSolutionCount;
        almostSolution[almostSolutionCount] = leftSolution[currLeftIdx];
	++almostSolutionCount;
    }
    
    almostSolution[almostSolutionCount] = rightSolution[rightLower];
    ++almostSolutionCount;
    int currRightIdx = rightLower;
    // continue counterclockwise on the right until added rightUpper
    while (currRightIdx != rightUpper) {
        currRightIdx = (currRightIdx+1)%rightSolutionCount;
	almostSolution[almostSolutionCount] = rightSolution[currRightIdx];
	++almostSolutionCount;
    }
    // polygon complete!

    // remove any colinear points in this merge
    solutionCount = 0;
    for (int i = 0; i < almostSolutionCount; ++i) {
	if (WindingOrder(almostSolution[(i+almostSolutionCount-1)%almostSolutionCount],
			 almostSolution[i],
			 almostSolution[(i+1)%almostSolutionCount]) == 0) {
	    continue;
	}
        solution[solutionCount] = almostSolution[i];
	++solutionCount;
    }


}

void Solve(int ptCount) {
    
    Point pts[ptCount];
    std::unordered_set<int> ptUniquenessTable;
    int ptsUniqueCount = 0;
    for (int i = 0; i < ptCount; ++i) {
        int x, y; std::cin >> x >> y;
        int uniqueFactor = x + (20010*y); // dont worry this is always within range
	if (ptUniquenessTable.find(uniqueFactor) != ptUniquenessTable.end()) {
	    continue;
	}
        ptUniquenessTable.insert(uniqueFactor);
        pts[ptsUniqueCount] = Point(x,y); ++ptsUniqueCount;
    }

    Point solution[ptsUniqueCount];
    int solutionCount = 0;

    SolveHelper(pts, ptsUniqueCount, solution, solutionCount);

    std::cout << solutionCount << std::endl;
    for (int i = 0; i < solutionCount; ++i) {
        std::cout << (solution[i]).x << " " << (solution[i].y) << std::endl;
    }
}

int main(int argc, char** argv) {

    while (true) {
        int pointCount; std::cin >> pointCount;
        if (pointCount <= 0) { break; }

	Solve(pointCount);
    }

    return 0;
}
