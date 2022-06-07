import random 

## default height/width
# height 
h = 3
# width
w = 4

# ask for inputs from user
hStr = input("Set height (or press enter for default): ")
if not hStr.strip() == '': 
  h = int(hStr)

wStr = input("Set width (or press enter for default): ")
if not wStr.strip() == '': 
  w = int(wStr)

# create board, a string
# the board is laid out as a list of length hk, as follows: 
#    0   1   2   3 
# 0  0 - 1 - 2 - 3
#    |   |   |   |
# 1  4 - 5 - 6 - 7
#    |   |   |   |
# 2  8 - 9 - 10- 11

tempBoard = input("Input board, or enter \"r\" for a random board, or press enter for all 0s: \n")
# will break if input is not all 0s and 1s 
tempBoard = tempBoard.strip() 
if tempBoard == '':
  board = '0' * (h * w)
elif tempBoard[0] == 'r': 
  seed = input("Enter seed (or press enter for arbitrary seed): ").strip()
  if seed: 
    random.seed(int(seed))
  board = "".join([str(random.randint(0,1)) for _ in range(h*w)])
else:
  board = tempBoard[:h*w]

# maintain set of forbidden places to toggle, currently set as one of the middle rows (if the height is even it picks the higher of the two)
forbidden = set([(h-1) // 2 * w + k for k in range(w)])

# getAdjacencies(k) gets the set of all adjacencies for k in the h by w grid 
# precondition: k must be between 0 and h * w
def getAdjacencies(k): 
  # r is the row of position k, indexed from 0
  r = k // w 
  # c is the column of position k, indexed from 0
  c = k % w 
  adjacencies = []
  if r != 0: 
    adjacencies.append(k - w)
  if r < h - 1: 
    adjacencies.append(k + w)
  if c != 0: 
    adjacencies.append(k - 1)
  if c < w - 1: 
    adjacencies.append(k + 1)
  return adjacencies

# flip(gameState, place) flips the state of place from 0 to 1 or vice versa. this does NOT modify gameState in-place! 
# precondition: gameState is an string of 0's and 1's. 
def flip(gameState, place): 
  nextState = gameState[0:place] + str(1 - int(gameState[place])) + gameState[place + 1:]
  return nextState 

# toggle(gameState, place) toggles the state of place and all of its neighbors. 
# precondition: place must be between 0 and hk - 1 inclusive, and the gameState array has all 0s and 1s.
def toggle(gameState, place): 
  nextState = flip(gameState, place) 
  adj = getAdjacencies(place)
  for j in adj: 
    nextState = flip(nextState, j)
  return nextState 

# prettify(gameState) returns a string that when printed, shows gameState in a h by w grid of 0s and 1s. 
def prettify(gameState): 
  separatedRows = []
  tempState = gameState
  # store each row in separatedRows
  for i in range(h): 
    separatedRows.append(tempState[0:w])
    tempState = tempState[w:]
  return "\n".join(separatedRows)

# processMove(command) returns the position in the board that is to be toggled. 
# precondition: the move is in the format "r c" where r ranges between 1 and h and c ranges between 1 and w, inclusive, or is a string that starts with 'q'
def processMove(command):
  if command[0] == 'q': 
    return -1
  idxs = command.strip().split(" ")
  return (int(idxs[0]) - 1) * w + int(idxs[1]) - 1

# states is the list of board states in the order that they have been executed
states = [board]
# moveNum is the number of the move 
moveNum = 0 

# main game loop
while (True): 
  print("Move {}:\n".format(moveNum))
  print(prettify(board))
  print() 
  command = input("Enter position to toggle in format \"r c\" (1 <= r <= {}, 1 <= c <= {}) or \"q\" to quit.\n>> ".format(h, w))
  print() 
  pos = processMove(command) 
  # print(pos) [debugging]

  # check if the user wants to quit
  if pos == -1: 
    break
    
  # check if move is not in range, and go to next iteration of loop if it is
  if pos < 0 or pos >= h * w: 
    print("Your move is invalid :(")
    print()
    continue 

  # check if the move is forbidden
  if pos in forbidden: 
    print("This move is forbidden :(")
    print() 
    continue
  
  # update the state
  board = toggle(board, pos) 
  states.append(board) 
  moveNum += 1
  
### END OF LOOP 

# post-processing 
print() 
print("History:")
for bd in states:
  print(prettify(bd))
  print() 
  

# # unit tests 
# for i in range(h * w): 
#   print(getAdjacencies(i))

# print(prettify(board))
# for i in range(h * w): 
#   print(prettify(toggle(board, i)))
#   print()
