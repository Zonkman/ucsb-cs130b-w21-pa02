#include <iostream>
#include <string>
#include <unordered_set>

// "I Wrote This" by Alex Lancaster (6706998)

long CharToLong(char& c) {
    switch (c) {
        case 'A':
	    return 1;
	case 'C':
	    return 2;
	case 'G':
	    return 3;
	case 'T':
	    return 4;
	default:
	    return 100000000000000;
    }
}

// if end < start, the string is backwards!
long GetStringHash(std::string& lookup, int start, int end) {
    long mul = 1;
    long total = 0;
    if (start <= end) {
        for (int i = start; i <= end; ++i) {
	    char& c = lookup[i];
            total += mul*CharToLong(c);
	    mul *= 5;
	}
    }
    else {
        for (int i = start; i >= end; --i) {
	    char& c = lookup[i];
	    total += mul*CharToLong(c);
	    mul *= 5;
	}
    }
    return total;
}

// for testing only! we want readable output
std::string GetHashString(const long hash) {
    std::string ret = "";
    long hash2 = hash;
    while (hash2 > 0) {
	switch (hash2%5) {
	    case 1: ret += 'A'; break;
	    case 2: ret += 'C'; break;
	    case 3: ret += 'G'; break;
	    case 4: ret += 'T'; break;
	}
        hash2 /= 5;
    }
    return ret;
}

long GetStringHash(std::string& lookup) {
    return GetStringHash(lookup, 0, lookup.length() - 1);
}

void AddAllInStringRegion(std::unordered_set<long>& set, std::string str, int start, int end) {
    for (int i = start; i <= end; ++i) {
        for (int j = start; j <= end; ++j) {
	    set.insert(GetStringHash(str, i, j));
	}
    }
}

void PrintSet(std::unordered_set<long>& set) {
    std::unordered_set<long>::iterator ius;
    for (ius = set.begin(); ius != set.end(); ius++) {
        std::cout << GetHashString(*ius) << ",";
    }
    std::cout << std::endl;
}

int SolveHelper(std::unordered_set<long>& inputSet, std::string& goal, int tabulation[], int currSubproblem); 

void TryToFillOutRegions(int& minSol, std::unordered_set<long>& inputSet, std::unordered_set<long>& partialSet, std::string& goal, int tabulation[], int currSubproblem, int start, int end) {
    for (int i = start; i <= end; ++i) {
        for (int j = i; j <= end; ++j) { // different since we only need to check one direction.
	    long h = GetStringHash(goal, i, j);
            if (inputSet.find(h) != inputSet.end() || partialSet.find(h) != partialSet.end()) {
	        // success! we can do this move. next step
		int newSubproblem = currSubproblem;
		for (int k = i; k <= j; ++k) { newSubproblem += (1 << k); }
		int newSol = 1 + SolveHelper(inputSet, goal, tabulation, newSubproblem);
                if (newSol < minSol) { minSol = newSol; }
	    }
	}
    }
}

// subproblem is determined by what letters are already in the partial
int SolveHelper(std::unordered_set<long>& inputSet, std::string& goal, int tabulation[], int currSubproblem) {
    if (tabulation[currSubproblem] != -1) { return tabulation[currSubproblem]; }

    std::unordered_set<long> partialSet; // the other strings we have so far.
    // scan for filled-out regions and add all combinations within each region.
    int regionStart = -1;
    for (int regionScanner = 0; regionScanner < goal.length(); ++regionScanner) {
        bool alreadyAdded = (((currSubproblem >> regionScanner) & 1) == 1);
	if (alreadyAdded) {
	    if (regionStart == -1) { regionStart = regionScanner; }
	}
	else if (regionStart != -1) {
	    int regionEnd = regionScanner - 1;
            AddAllInStringRegion(partialSet, goal, regionStart, regionEnd);
	    regionStart = -1;
	}
    }
    if (regionStart != -1) { AddAllInStringRegion(partialSet, goal, regionStart, goal.length() - 1); }

    //PrintSet(partialSet);
    
    int minSol = 9999999;

    // similar region search to check all subproblems.
    regionStart = -1;
    for (int regionScanner = 0; regionScanner < goal.length(); ++regionScanner) {
        bool notAdded = (((currSubproblem >> regionScanner) & 1) == 0);
        if (notAdded) {
	    if (regionStart == -1) { regionStart = regionScanner; }
	}
	else if (regionStart != -1) {
	    int regionEnd = regionScanner - 1;
            TryToFillOutRegions(minSol, inputSet, partialSet, goal, tabulation, currSubproblem, regionStart, regionEnd);
	    regionStart = -1;
	}
    }
    if (regionStart != -1) {
        TryToFillOutRegions(minSol, inputSet, partialSet, goal, tabulation, currSubproblem, regionStart, goal.length() - 1); 
    }


    if (minSol > 999999) { return 9999999; }
    tabulation[currSubproblem] = minSol;
    return minSol;
}

int Solve() {
    std::string input; std::cin >> input;
    std::string goal; std::cin >> goal;

    if (input.length() > 18 || goal.length() > 18) { std::cout << "TOO LONG!" << std::endl; return 999999; }
    if (input == goal) { return 1; } // one copy if they're perfectly equal

    // generate all possible string to copy from the input
    std::unordered_set<long> inputSet;
    AddAllInStringRegion(inputSet, input, 0, input.length() - 1);
    
    int tabLength = 1 << goal.length();
    int tabulation[tabLength];
    for (int i = 0; i < tabLength - 1; ++i) { tabulation[i] = -1; }
    tabulation[tabLength - 1] = 0; // base case: all letters have been placed

    //PrintSet(inputSet);
    int sol = SolveHelper(inputSet, goal, tabulation, 0);
    
    return sol;
}

int main(int argc, char** argv) {
    int testCount;
    std::cin >> testCount;

    for (int i = 0; i < testCount; ++i) {
        int sol = Solve();
	if (sol > 999999) { std::cout << "impossible" << std::endl; }
	else { std::cout << sol << std::endl; } 
    }

    return 0;
}
