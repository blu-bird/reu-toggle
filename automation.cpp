// options:
// petersen n k
// grid h w
// file filename
//

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

// readGraphLines(filename) reads the lines of the graph from the file and stores them in the global variable adjMatrix, and returns the value of n, which is the number of vertices in the graph
int readGraphLines(string filename)
{
  int n = -1;              // default value if there is no file to read
  vector<string> adjLines; // list of the adjacencies for each vtx, represented as strings, if reading from file

  ifstream graphfile;
  graphfile.open(filename);
  if (graphfile.is_open())
  {
    // read the size from the file
    string size;
    getline(graphfile, size);
    n = stoi(size);
    // read each of the n successive lines from the file
    // precondition -- there are n lines of binary strings after the first
    string adjLine;
    while (getline(graphfile, adjLine))
    {
      adjLines.push_back(adjLine);
    }
    graphfile.close();

    // create graph matrix from file input
    createAdjs(n, adjLines);
  }
  else
  {
    cout << "file not found, please check to see if you have the right file \n";
  }

  return n;
}

// initializeState(n, argc, argPos, argv[]) by default will always return a string of n 1's. if there are enough command line arguments, instead return the next command line argument which will be a bit string of length n, with 1s and 0s assigned to the vertices as given in the adjacency matrix or with the right conventions for the Petersen graph/grid.
string initializeState(int n, int argc, int argPos, char *argv[])
{
  string startState = string(n, '1');
  if (argPos < argc)
  {
    if (*argv[1] == 'p')
    {
      int k = n / 2; // guaranteed to be even if we have petersen
      if (*argv[argPos] == 'i')
      {
        startState = string(k, '0') + string(k, '1');
      }
      else if (*argv[argPos] == 'o')
      {
        startState = string(k, '1') + string(k, '0');
      }
      else if (*argv[argPos] != 'a')
      {
        startState = argv[argPos];
      }
      // otherwise, don't do anything, should be all 1's
    }
    else
    {
      startState = argv[argPos];
    }
  }
  return startState;
}

// options:
// petersen n k [i/o/a]
// grid h w
// file filename
int main(int argc, char *argv[])
{
  int n;                                      // number of vertices in the graph
  nimberComps = unordered_map<string, int>(); // instantiate memoization table

  // no command line arguments
  if (argc < 2)
  {
    cout << "please provide command line arguments \n";
  }
  else
  {
    char graphOption = tolower(argv[1][0]);
    // cout << graphOption << "\n";

    // parse the first command line option
    switch (graphOption)
    {
    case 'f': // read from file
      if (argc < 3)
      {
        cout << "please provide filename";
        break;
      }
      else
      {
        string filename = argv[2]; // get file name
        n = readGraphLines(filename);
        // run game
        if (n != -1)
        {
          string startState = initializeState(n, argc, 3, argv);
          int nimVal = runGame(startState, n);
          cout << "nimber of graph from file " << filename << ": " << nimVal << "\n";
        }
      }
      break;

    case 'p': // construct petersen graph
      if (argc < 4)
      {
        cout << "please provide two parameters to generate Petersen graph";
        break;
      }
      else
      {
        int m = stoi(argv[2]); // number of m-gons
        int k = stoi(argv[3]); // twist number
        n = 2 * m;

        // create the adjacencies
        createGPetersenAdjs(m, k);

        string startState = initializeState(n, argc, 4, argv);
        int nimVal = runGame(startState, n);
        cout << "nimber of graph from GP(" << m << ", " << k << ") ";
        if (argc >= 5)
        {
          switch (*argv[4])
          {
          case 'i':
            cout << "(1's inside)";
            break;
          case 'o':
            cout << "(1's outside)";
            break;
          case 'a':
            cout << "(all 1's)";
            break;
          case '1':
          case '0':
            cout << "(custom)";
            break;
          default:
            cout << "(default, all 1's)";
            break;
          }
        }
        else
        {
          cout << "(no args, all 1's)";
        }
        cout << ": " << nimVal << "\n";
      }
      break;

    case 'g': // construct grid
      if (argc < 4)
      {
        cout << "please provide two parameters to generate grid";
        break;
      }
      else
      {
        int h = stoi(argv[2]); // number of m-gons
        int w = stoi(argv[3]); // twist number
        n = h * w;

        // create the adjacencies
        createGridAdjs(h, w);

        string startState = initializeState(n, argc, 4, argv);
        int nimVal = runGame(startState, n);
        cout << "nimber of " << h << " x " << w << "grid: " << nimVal << "\n";
      }
      break;
    }
  }

  // nimberComps = unordered_map<string, int>();
  // n = 3;
  // createGridAdjs(3, 1);
  // string startState = "011";
  // unordered_set<string> nextStates = getNextStates(n, startState);
  // for (unordered_set<string>::iterator it = nextStates.begin(); it != nextStates.end(); ++it) {
  //     cout << *it << ' ';
  //   }

  // generate grids
  // 2 x n grids, 3 x n grids
  // for (int k = 1; k < 20; k++)  {
  //   nimberComps = unordered_map<string, int>();
  //   n = 3 * k;
  //   string startState = string(n, '1');
  //   createGridAdjs(3, k);
  //   int nimVal = runGame(startState, n);
  //   cout << "nimber for 3 x " << k << " grid: " << nimVal << '\n';
  // }

  // generate generalized Petersen graphs iteratively
  // for (int k = 23; k < 24; k++)  {
  //   nimberComps = unordered_map<string, int>();
  //   n = 2 * k;
  //   string startState = string(n, '1');
  //   createGPetersenAdjs(k, 1);
  //   int nimVal = runGame(startState, n);
  //   cout << "nimber for GP(" << k << ", 1): " << nimVal << '\n';
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