// code compiled with gcc 10.3, may not be backwards compatible with older versions of c++
#include <fstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <cctype>
#include <algorithm>
#include <iostream>
using namespace std;

vector<unordered_set<int>> adjMatrix;
unordered_map<string, int> nimberComps;

// createAdjs(n, inLines) creates the graph from the adjacency matrix, where inLines is a list of binary strings corresponding to each vertex in order and n is the number of vertices. This will wipe adjMatrix and fill it with the adjacencies in this graph.
void createAdjs(int n, vector<string> inLines)
{
  // adjMatrix = {}
  for (int i = 0; i < n; i++)
  {
    unordered_set<int> iAdjs;
    for (int j = 0; j < n; j++)
    {
      if (inLines[i][j] == '1')
      {
        iAdjs.insert(j);
      }
    }
    adjMatrix.push_back(iAdjs);
  }
}

// createGPetersenAdjs(n, k) will wipe adjMatrix, create the adjacencies for the generalized Petersen graph GP(n, k) and store them in adjMatrix.
void createGPetersenAdjs(int n, int k)
{
  adjMatrix = vector<unordered_set<int>>();
  for (int j = 0; j < 2 * n; j++)
  {
    if (j < n)
    {
      adjMatrix.push_back(unordered_set<int>({(j + n - 1) % n, (j + 1) % n, j + n}));
    }
    else
    {
      adjMatrix.push_back(unordered_set<int>({j - n, (j + n - k) % n + n, (j + k) % n + n}));
    }
  }
  return;
}

// createGridAdjs(h, w) will wipe adjMatrix, create the adjacencies for an h x w grid and store them in adjMatrix.
void createGridAdjs(int h, int w)
{
  adjMatrix = vector<unordered_set<int>>();
  for (int k = 0; k < h * w; k++)
  {
    unordered_set<int> adjs = unordered_set<int>();
    // r is the row of position k, indexed from 0
    int r = k / w;
    // c is the column of position k, indexed from 0
    int c = k % w;

    if (r != 0)
      adjs.insert(k - w);
    if (r < h - 1)
      adjs.insert(k + w);
    if (c != 0)
      adjs.insert(k - 1);
    if (c < w - 1)
      adjs.insert(k + 1);

    adjMatrix.push_back(adjs);
  }
  return;
}

// createLadderTwistAdjs(w, k) will wipe adjMatrix, create the adjacencies for a "twisted ladder" grid of size 2 x w, where the top row is linked as usual, and every kth vertex is connected on the bottom row.
// Precondition: w >= 3, k < w / 2
void createLadderTwistAdjs(int w, int k)
{
  adjMatrix = vector<unordered_set<int>>();
  for (int j = 0; j < 2 * w; j++)
  {

    if (j == 0)
    {
      adjMatrix.push_back({1, w});
    }
    else if (j == w)
    {
      adjMatrix.push_back({0, w + k});
    }
    else if (j == w - 1)
    {
      adjMatrix.push_back({w - 2, 2 * w - 1});
    }
    else if (j == 2 * w - 1)
    {
      adjMatrix.push_back({w - 1, 2 * w - k - 1});
    }
    else if (j < w)
    {
      adjMatrix.push_back({j - 1, j + 1, j + w});
    }
    else if (j - w < k)
    {
      adjMatrix.push_back({(j + w - k - 1) % w + w, (j + k) % w + w, j - w});
    }
    else if (2 * w - j <= k)
    {
      adjMatrix.push_back({(j + w - k) % w + w, (j + k + 1) % w + w, j - w});
    }
    else
    {
      adjMatrix.push_back({(j + w - k) % w + w, (j + k) % w + w, j - w});
    }
  }
  return;
}

// printSet prints theSet within curly braces, separated by commas
void printSet(unordered_set<string> theSet)
{
  cout << "{";
  for (unordered_set<string>::iterator it = theSet.begin(); it != theSet.end(); ++it)
  {
    cout << *it << ", ";
  }
  cout << "}\n";
}

// printAdjs() prints the adjacencies in adjMatrix nicely.
void printAdjs()
{
  for (int i = 0; i < adjMatrix.size(); i++)
  {
    cout << i << ": ";
    unordered_set<int> adjs = adjMatrix[i];
    cout << "{";
    for (unordered_set<int>::iterator it = adjs.begin(); it != adjs.end(); ++it)
    {
      cout << *it << ", ";
    }
    cout << "}\n";
  }
}

// toggle(gameState, place, n) toggles the state of place and all of its neighbors.
// precondition: place must be between 0 and n - 1 inclusive, and the gameState has all 0s and 1s.
string toggle(string gameState, int place, int n)
{
  unordered_set<int> adj = adjMatrix.at(place);
  char nextState[n];
  for (int i = 0; i < n; i++)
  {
    if (adj.count(i) == 1 || i == place)
    {
      nextState[i] = 1 - (gameState[i] - '0') + '0';
    }
    else
    {
      nextState[i] = gameState[i];
    }
  }
  return nextState;
}

// getNextStates(n, gameState) finds all valid next states that the game can progress to from the current state, gameState. n is the number of vertices in the graph.
// RULE: the number of 1's must STRICTLY decrease, and we can only toggle 1's.
unordered_set<string> getNextStates(int n, string gameState)
{
  unordered_set<string> nextStates;
  for (int i = 0; i < n; i++)
  {
    if (gameState[i] == '1')
    {
      int onCounter = 0;
      unordered_set<int> adjSet = adjMatrix[i];
      // count number of neighbors that are on
      for (unordered_set<int>::iterator it = adjSet.begin(); it != adjSet.end(); ++it)
      {
        if (gameState[*it] == '1')
        {
          onCounter++;
        }
      }
      // len(adjMatrix[i]) - onCounter is number of off neighbors
      // want 1 + onCounter > len(adjMatrix[i]) - onCounter (if it's 1)
      // want onCounter > len(adjMatrix[i]) - onCounter + 1 (if it's 0)
      // check if the move is valid, and if so, compute the next state and add it to list
      if ((gameState[i] - '0') + 2 * onCounter > adjSet.size() + (1 - (gameState[i] - '0')))
        nextStates.insert(toggle(gameState, i, n));
    }
  }
  return nextStates;
}

// mex(natSet) computes the minimum excluded natural number of a set of integers
int mex(unordered_set<int> natSet)
{
  int currMex = 0;
  while (natSet.count(currMex) == 1)
  {
    currMex++;
  }
  return currMex;
}

// rotate(state, n) rotates a state for GP(n, _) by one vertex counterclockwise, where n is the length of the outer/inner cycles
string rotate(string state, int n)
{
  return state.substr(1, n - 1) + state.substr(0, 1) + state.substr(n + 1, n - 1) + state.substr(n, 1);
}

// runGame(startState, n) recursively computes the nimber of gameState by traversing through the whole subtree from that point, memoizing game states it has already seen.
int runGame(string startState, int n)
{
  unordered_set<string> nextStates = getNextStates(n, startState);

  // debug
  // cout << startState << ": ";
  // printSet(nextStates);

  if (nextStates.size() == 0)
  {
    nimberComps.emplace(startState, 0);
    return 0;
  }

  else
  {
    unordered_set<int> childNimbers;
    for (unordered_set<string>::iterator s = nextStates.begin(); s != nextStates.end(); ++s)
    {
      // also check if cyclic permutations are present
      // cycle through currRotState and check if each one is in nimberComps
      // if we get a match then we should return that as the nimber
      string currRotState = *s;
      bool matchFound = false;
      for (int k = 0; k < n; k++)
      {
        if (nimberComps.count(currRotState) == 1)
        {
          childNimbers.insert(nimberComps[currRotState]);
          matchFound = true;
          break;
        }
        // n is the number of vertices here
        currRotState = rotate(currRotState, n / 2);
      }

      // if (nimberComps.count(*s) == 1)
      // {
      //   childNimbers.insert(nimberComps[*s]);
      // }
      if (!matchFound)
      {
        int gameNimVal = runGame(*s, n);
        nimberComps.emplace(*s, gameNimVal);
        childNimbers.insert(gameNimVal);
      }
    }
    int stateNimber = mex(childNimbers);

    // debug
    // cout << startState << ": " << stateNimber << "\n";

    return stateNimber;
  }
}

// from a game starting with all 1's on GP(w-1, k), after making a move on the inside, this is the starting state of the board
string innerTwistStart(int w, int k)
{
  string toprow = string(1, '0') + string(w - 2, '1') + string(1, '0');
  string bottomrow = string(1, '0') + string(k - 1, '1') + string(1, '0') + string(w - 2 - 2 * k, '1') + string(1, '0') + string(k - 1, '1') + string(1, '0');
  return toprow + bottomrow;
}

// from a game starting with all 1's on GP(w-1, k), after making a move on the outside, this is the starting state of the board
string outerTwistStart(int w, int k)
{
  return string(2, '0') + string(w - 4, '1') + string(3, '0') + string(w - 2, '1') + string(1, '0');
}

// options:
// petersen n k
// grid h w
// file filename
int main()
{
  int n; // number of vertices in the graph
  // vector<string> adjLines; // list of the adjacencies for each vtx, represented as strings

  // read from a file
  // ifstream graphfile;
  // graphfile.open("gp41.txt");
  // if (graphfile.is_open())  {
  //   // read the size from the file
  //   string size;
  //   getline(graphfile, size);
  //   n = stoi(size);
  //   // cout << n;
  //   // read each of the n successive lines from the file
  //   // precondition -- there are n lines of binary strings after the first
  //   string adjLine;
  //   while (getline(graphfile, adjLine))  {
  //     adjLines.push_back(adjLine);
  //   }
  // }
  // graphfile.close();

  // create graph matrix from file input
  // createAdjs(n, adjLines);

  // nimberComps = unordered_map<string, int>();
  // n = 3;
  // createGridAdjs(3, 1);
  // string startState = "011";
  // unordered_set<string> nextStates = getNextStates(n, startState);
  // for (unordered_set<string>::iterator it = nextStates.begin(); it != nextStates.end(); ++it) {
  //     cout << *it << ' ';
  //   }

  // createLadderTwistAdjs(7, 2);
  // printAdjs();

  // why would you do this
  // n = 30;
  // int k = 10;
  // createGPetersenAdjs(n, k);
  // string startState = string(n, '1') + string(n, '0');
  // int nimVal = runGame(startState, 2 * n);
  // cout << "nimber for GP(30, 10) (half 1's): " << nimVal << '\n';

  // generate grids
  // 2 x n grids, 3 x n grids
  //  for (int k = 1; k < 10; k++)  {
  //    nimberComps = unordered_map<string, int>();
  //    n = 2 * k;
  //    string startState = string(n, '1');  // string(k-2, '1') + string(2, '0') + string(k-1, '1') + string(1, '0');
  // halfed -- string(k-2, '1') + string(2, '0') + string(k-1, '1') + string(1, '0');
  // string(n, '1');
  // diagonal, 1's on opposite sides: string startState = string(2, '0') + string(k-3, '1') + string(2, '0') + string(k-3, '1') + string(2, '0');
  // hat, 1's on same side: string(2, '0') + string(k - 4, '1') + string(3, '0') + string(k - 2, '1') + string(1, '0');
  //    createGridAdjs(2, k);
  //    int nimVal = runGame(startState, n);
  //    cout << "nimber for 2 x " << k << " augmented grid: " << nimVal << '\n';
  //  }
  // createGPetersenAdjs(7, 3);
  // string startState = string(14, '1');
  // unordered_set<string> nextStates = getNextStates(14, startState);
  // for (unordered_set<string>::iterator it = nextStates.begin(); it != nextStates.end(); ++it)
  // {
  //   cout << *it << "\n";
  // }

  // createLadderTwistAdjs(13, 2);
  // printAdjs();

  // testing the unrolling strat
  // for (int j = 13; j < 25; j++)
  // {
  //   nimberComps = unordered_map<string, int>();
  //   n = 2 * j;
  //   string startState = outerTwistStart(j, 2); // innerTwistStart(j, 2);
  //   createLadderTwistAdjs(j, 2);
  //   // createLadderTwistAdjs(j, 2);
  //   int nimVal = runGame(startState, n);
  //   cout << "nimber for inner LadderTwist(" << j << ", 2): " << nimVal << '\n';
  // }
  // createLadderTwistAdjs(8, 2);
  // printAdjs();

  cout << "running\n";
  int k = 29;
  for (int i = 1; i <= k / 2; i++)
  {
    nimberComps = unordered_map<string, int>();
    n = 2 * k;
    string startState = string(k, '0') + string(k, '1'); // string(n, '1'); // string(k, '0') + string(k, '1'); // string(k, '1') + string(k, '0'); string(n, '1');
    createGPetersenAdjs(k, i);
    int nimVal = runGame(startState, n);
    cout << "nimber for GP(" << k << ", " << i << ") with 1's on inside: " << nimVal << '\n';
  }

  // generate generalized Petersen graphs iteratively
  // for (int k = 7; k < 25; k++)
  // {
  //   nimberComps = unordered_map<string, int>();
  //   n = 2 * k;
  //   string startState = string(n, '1'); // string(k, '1') + string(k, '0'); string(n, '1');
  //   createGPetersenAdjs(k, 5);
  //   int nimVal = runGame(startState, n);
  //   cout << "nimber for GP(" << k << ", 5): " << nimVal << '\n';
  // }

  // starting config
  // string startState = string(n, '1');

  // // unit text for runGame
  // cout << runGame(startState, n);

  // // unit test for mex
  // cout << mex({5, 0, 0, 2, 3});

  // unit test for nextStates
  // unordered_set<string> nextStates = getNextStates(n, startState);
  // for (unordered_set<string>::iterator it = nextStates.begin(); it != nextStates.end(); ++it) {
  //   cout << *it;
  // }

  // unit test for graph adjacency
  // cout << "made graph\n";
  // for (int i = 0; i < n; i++)  {
  //   unordered_set<int> adjLine = adjMatrix[i];
  //   cout << i << ": ";
  //   for (unordered_set<int>::iterator it = adjLine.begin(); it != adjLine.end(); ++it) {
  //     cout << *it << ' ';
  //   }
  //   cout << '\n';
  // }

  return 0;
}
