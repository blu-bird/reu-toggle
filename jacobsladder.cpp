#include <iostream>
#include <unordered_map>
#include <vector>
#include <unordered_set>
#include <fstream>
using namespace std;

vector<int> memTable;

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

// compute the nimber for the Jacob's Ladder game for all n
// precondition: n is at least 0
int jacobsLadderNim(int n, ofstream *nums, ofstream *zeroes)
{

  // loop invariant -- everything from 1 to i has been computed
  for (int i = 1; i <= n; i++)
  {
    if (i <= 6) // base case
    {
      int base = memTable[i];
      *nums << base << "\n";
      if (base == 0)
      {
        *zeroes << i << "\n";
      }
    }
    else // non base case
    {
      unordered_set<int> childNims;
      childNims.insert(memTable[i - 3]);
      childNims.insert(memTable[i - 4]);
      childNims.insert(memTable[i - 5]);

      for (int k = 1; k <= i - 6; k++)
      {
        int leftVal = memTable[k];
        int rightVal = memTable[i - 5 - k];
        childNims.insert(leftVal ^ rightVal);
      }

      int nimVal = mex(childNims);
      *nums << nimVal << "\n";
      if (nimVal == 0)
      {
        *zeroes << i << "\n";
      }
      memTable.push_back(nimVal);
    }
  }

  return memTable[n];
}

// isNumber(number) computes whether number is a string that can be parsed as a number
// taken from https://stackoverflow.com/questions/29248585/c-checking-command-line-argument-is-integer-or-not
bool isNumber(char number[])
{
  for (int i = 0; number[i] != 0; i++)
  {
    // if (number[i] > '9' || number[i] < '0')
    if (!isdigit(number[i]))
      return false;
  }
  return true;
}

int main(int argc, char *argv[])
{
  // put in initial values
  memTable.push_back(0); // n = 0
  memTable.push_back(1); // n = 1
  memTable.push_back(1); // n = 2
  memTable.push_back(1); // n = 3
  memTable.push_back(2); // n = 4
  memTable.push_back(2); // n = 5
  memTable.push_back(0); // n = 6

  // parse command line args
  int n = 100;
  if (argc >= 2 && isNumber(argv[1]))
  {
    n = stoi(argv[1]);
  }
  else if (argc >= 2)
  {
    cout << "invalid argument\n";
    return 1; // error out
  }

  ofstream zeroFile;
  ofstream seqFile;
  zeroFile.open("zeroes.txt");
  seqFile.open("octal11337.txt");

  cout << "computing\n";
  // computes all nimbers through n - 3, inclusive
  jacobsLadderNim(n, &seqFile, &zeroFile);

  zeroFile.close();
  seqFile.close();
}