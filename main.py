# import random 

# adjMatrix is the representation of the graph G that we are playing on. It has keys [0, ..., n-1] and each key has value a subset of [0, ..., n-1]. 
adjMatrix = {}

# createAdjs(n, adjLists) creates the graph from the adjacency matrix, where adjLists is a list of binary strings corresponding to each vertex in order and n is the number of vertices. This will wipe adjMatrix and fill it with the adjacencies in this graph. 
def createAdjs(n, adjLists):
  adjMatrix = {}
  for i in range(n): 
    iAdjs = set([])
    for j in range(n): 
      if adjLists[i][j] == '1': 
        iAdjs.add(j)
    adjMatrix[i] = iAdjs
  return 


# createGPetersenAdjs(n, k) will wipe adjMatrix, create the adjacencies for the generalized Petersen graph GP(n, k) and store them in adjMatrix. 
def createGPetersenAdjs(n, k): 
  adjMatrix = {}
  for k in range(2 * n): 
    if k < n: 
      adjMatrix[k] = set([(k-1) % n, (k+1) % n, k + n])
    else: 
      adjMatrix[k] = set([k - n, (k - 1) % n + n, (k + 1) % n + n])
  return 

# createGPetersenAdjs(h, w) will wipe adjMatrix, create the adjacencies for an h x w grid and store them in adjMatrix. 
def createGridAdjs(h, w): 
  for k in range(h * w): 
    adjs = set([])
    # r is the row of position k, indexed from 0
    r = k // w 
    # c is the column of position k, indexed from 0
    c = k % w 
  
    if r != 0: 
      adjs.add(k - w)
    if r < h - 1: 
      adjs.add(k + w)
    if c != 0: 
      adjs.add(k - 1)
    if c < w - 1: 
      adjs.add(k + 1)
      
    adjMatrix[k] = adjs 
  return 


# getAdjacencies(k) gets the set of all adjacencies for k in the h by w grid 
# precondition: k must be between 0 and h * w
def getAdjacencies(k): 
  return adjMatrix.get(k)


# flip(gameState, place) flips the state of place from 0 to 1 or vice versa. this does NOT modify gameState in-place! 
# precondition: gameState is an string of 0's and 1's. 
def flip(gameState, place): 
  nextState = gameState[0:place] + str(1 - int(gameState[place])) + gameState[place + 1:]
  return nextState 


# toggle(gameState, place) toggles the state of place and all of its neighbors. 
# precondition: place must be between 0 and n - 1 inclusive, and the gameState has all 0s and 1s.
def toggle(gameState, place, n): 
  adj = adjMatrix.get(place) # getAdjacencies(place)
  return "".join([str(1 - int(gameState[k]))  if (k in adj or k == place) else gameState[k] for k in range(n)])
  # nextState = flip(gameState, place) 
  # for j in adj: 
  #   nextState = flip(nextState, j)
  # return nextState 

  
# getNextStates(n, gameState) finds all valid next states that the game can progress to from the current state, gameState. n is the number of vertices in the graph. 
# RULE: the number of 1's must STRICTLY decrease, and we can only toggle 1's.  
def getNextStates(n, gameState): 
  nextStates = set([])
  for i in range(n): 
    if gameState[i] == '1':
      onCounter = 0 
      # count number of neighbors that are on
      for j in adjMatrix[i]:
        if gameState[j] == '1': 
          onCounter += 1
      # len(adjMatrix[i]) - onCounter is number of off neighbors
      # want 1 + onCounter > len(adjMatrix[i]) - onCounter
      # check if the move is valid, and if so, compute the next state and add it to list
      if 1 + 2 * onCounter > len(adjMatrix[i]): 
        nextStates.add(toggle(gameState, i, n))
  return nextStates


# mex(natSet) returns the minimal excluded natural number of natList, starting from 0. 
def mex(natSet):
  currMex = 0
  while currMex in natSet: 
    currMex += 1
  return currMex 
  # for i in natList: 
  #   if i == currMex: 
  #     currMex += 1
  #   elif i > currMex: # all others must be greater 
  #     break 
  # return currMex

  
# runGame(startState, n) recursively computes the nimber of gameState by traversing through the whole subtree from that point, memoizing game states it has already seen. 
nimberComps = {}
def runGame(startState, n):
  nextStates = getNextStates(n, startState)
  if len(nextStates) == 0: 
    nimberComps[startState] = 0 
    return 0 
  else: 
    childNimbers = set([])
    for s in nextStates: 
      if s in nimberComps.keys(): 
        childNimbers.add(nimberComps[s])
      else: 
        gameNimVal = runGame(s, n)
        nimberComps[s] = gameNimVal 
        childNimbers.add(gameNimVal)
    return mex(childNimbers)

    
# input text file as graph
# the first line is the number of vertices in the graph 
# the next n lines (numbered 0 ... n-1) consist of a single bit-string of s length n. on line i, s[j] = 1 iff i and j are connected in the graph. 
# graphFile = open("gp41.txt", "r")
# lines = [s.strip() for s in graphFile.readlines()]
# n = int(lines[0])
# print(lines[1:])
# createAdjs(n, lines[1:])
# print(adjMatrix)
# {0: [1, 3, 4], 1: [0, 2, 5], 2: [1, 3, 6], 3: [0, 2, 7], 4: [0, 5, 7], 5: [1, 4, 6], 6: [2, 5, 7], 7: [3, 4, 6]}

    
# generate generalized Petersen graphs iteratively
# createGPetersenAdjs(4, 1)
# k = 15
# for k in range(23,24): 
#   adjMatrix = {}
#   nimberComps = {}
#   n = 2 * k
#   startState = '1' * n
#   createGPetersenAdjs(k, 1)
#   nimVal = runGame(startState, n)
#   print("nimber for GP({}, 1): {}".format(k, nimVal))
  

# generate grids
# # # 2 x n grids 
# for k in range(1, 25): 
#   adjMatrix = {}
#   nimberComps = {}
#   n = 2 * k
#   startState = '1' * n 
#   createGridAdjs(2, k)
#   nimVal = runGame(startState, n)
#   print("nimber for 2 x {} grid: {}".format(k, nimVal))

# # 3 x n grids 
for k in range(1, 25): 
  nimberComps = {}
  n = 3 * k
  startState = '1' * n 
  createGridAdjs(3, k)
  nimVal = runGame(startState, n)
  print("nimber for 3 x {} grid: {}".format(k, nimVal))
  

# unit tests
# print(adjMatrix)
# {0: [3, 1, 4], 1: [0, 2, 5], 2: [1, 3, 6], 3: [2, 0, 7], 4: [0, 7, 5], 5: [1, 4, 6], 6: [2, 5, 7], 7: [3, 6, 4]}

# game states are a string of n 0's or 1's
# startState = '1' * n
# print(getNextStates(n, startState))
# print(mex([5, 0, 0, 2, 3]))
# print(runGame(startState, n))


