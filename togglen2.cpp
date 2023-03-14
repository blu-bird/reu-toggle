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

// initializeState(n, option) by default will always return a string of n 1's. 
// if 'i' or 'o' is provided, to the vertices as given 
// in the adjacency matrix or with the right conventions for the Petersen graph/grid.
string initializeState(int n, char option)
{
  string startState = string(n, '1');
      int k = n / 2; // guaranteed to be even if we have petersen
      if (option == 'i')
      {
        startState = string(k, '0') + string(k, '1');
      }
      else if (option == 'o')
      {
        startState = string(k, '1') + string(k, '0');
      }
      else
      {
        startState = string(n, '1');
      }

  return startState;
}

// compile with g++ -o toggle togglen2.cpp
// ./toggle [n]
int main(int argc, char *argv[])
{
  int m;                                      // number of vertices in the graph
  nimberComps = unordered_map<string, int>(); // instantiate memoization table

        int n = stoi(argv[1]); //  m-gons
        int k = stoi(argv[2]); // twist number
        m = 2 * n;

        // create the adjacencies
        createGPetersenAdjs(n, k);
        string startState = initializeState(m, 'o');
        int nimVal = runGame(startState, m);
        cout << "nimber of graph from GP(" << n << ", " << k << ") ";
        cout << ": " << nimVal << "\n";

  return 0;
}