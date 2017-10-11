# search.py
# ---------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
# 
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).


"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
          state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
         actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def tinyMazeSearch(problem):
    """
    Returns a sequence of moves that solves tinyMaze.  For any other maze, the
    sequence of moves will be incorrect, so only use this for tinyMaze.
    """
    from game import Directions
    s = Directions.SOUTH
    w = Directions.WEST
    return  [s, s, w, s, w, w, s, w]

def depthFirstSearch(problem):
    """
    Search the deepest nodes in the search tree first.

    Your search algorithm needs to return a list of actions that reaches the
    goal. Make sure to implement a graph search algorithm.

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:	
    print "Start:", problem.getStartState()
    print "Is the start a goal?", problem.isGoalState(problem.getStartState())
    print "Start's successors:", problem.getSuccessors(problem.getStartState())
    """
    #print "Start:", problem.getStartState() 
    """
    getStartState returns the node in terms of some tuple (x,y)
    Start: (5, 5)
    """
    #print "Is the start a goal?", problem.isGoalState(problem.getStartState())
    """
    this is just a boolean to test goal state
    """
    #print "Start's successors:", problem.getSuccessors(problem.getStartState())
    """
    Start's successors: [((5, 4), 'South', 1), ((4, 5), 'West', 1)]
    essentially 2 d cordinates, which tells position, direction, and length or cost
    """
    st = util.Stack() # stack to go till the last node in depth tree
    explored_list = [] # visited nodes
    st.push((problem.getStartState(),[]))#second arg would store all the moves , initialize to empty
    while not st.isEmpty():
        n, movements_list = st.pop()
        explored_list.append(n)
        if n not in explored_list:
            #print n
            if problem.isGoalState(n):
                #import pdb
                #pdb.set_trace()
                #print movements_list
                return movements_list
                #return ['South','South','West','South','West','West','South', 'West']# add code to return directions later
                """
                the framework processes the returned list to make the moves,
                when the goal state is found, we must have traversed the path
                according to the search strategy and we must have stored the 
                direction of each node visited, while we are at it !!
                returning an empty list or some malformed list leads to game being stuck !!
                """
            successor_nodes = problem.getSuccessors(n)
            for each_successor in (successor_nodes):
                p,d,c = each_successor # p-osition same as node, d-irection, c-ost of path
                #import pdb
                #pdb.set_trace()
                # .append() does not work as it doesnot return a list created in place, rather it assigns return type 
                # of append() to new list, list conacatenation operator should be used here
                #new_movements_list = movements_list.append(d) 
                new_movements_list = movements_list + [d] # this assignment creates new list for each successor
                #movements_list.append(d)
		st.push((p,new_movements_list)) #store traversed path so far
                #print each_successor
    return []
    
    #print "DFS is done"
    #import sys
    #sys.exit(0)
    #import pdb
    #pdb.set_trace()
    "*** YOUR CODE HERE ***"
    #util.raiseNotDefined()

def breadthFirstSearch(problem):
    """Search the shallowest nodes in the search tree first."""
    "*** YOUR CODE HERE ***"
    q = util.Queue()
    explored_list = []
    q.push((problem.getStartState(),[]))
    while not q.isEmpty():
        # first explore then mark explored, queue the children, do not explore them
        node, movements_list = q.pop()
        if node not in explored_list:
            if(problem.isGoalState(node)):
                return movements_list
            for successor in problem.getSuccessors(node):
                p,d,c = successor
                new_movement = movements_list + [d]
                q.push((p, new_movement))
    
            explored_list.append(node)
    return []
    #import pdb
    #pdb.set_trace()
    #util.raiseNotDefined()

def uniformCostSearch(problem):
    """Search the node of least total cost first."""
    "*** YOUR CODE HERE ***"
    pq = util.PriorityQueue()
    explored_list = []
    pq.push((problem.getStartState(),[]),0) # first node cost is zero
    while not pq.isEmpty():
        node, movements_list = pq.pop()
        if node not in explored_list:
            if (problem.isGoalState(node)):
                return movements_list
            for successor in problem.getSuccessors(node):
                p,d,c = successor
                new_movement = movements_list + [d]
                pq.push((p,new_movement),problem.getCostOfActions(new_movement))
            explored_list.append(node)
    return []
    #util.raiseNotDefined()

def nullHeuristic(state, problem=None):
    """
    A heuristic function estimates the cost from the current state to the nearest
    goal in the provided SearchProblem.  This heuristic is trivial.
    """
    return 0

def aStarSearch(problem, heuristic=nullHeuristic):
    """Search the node that has the lowest combined cost and heuristic first."""
    "*** YOUR CODE HERE ***"
    #util.raiseNotDefined()
    pq = util.PriorityQueue()
    explored_list = []
    pq.push((problem.getStartState(),[]),heuristic(problem.getStartState(),problem))
    while not pq.isEmpty():
        node, movements_list = pq.pop()
        if node not in explored_list:
            if(problem.isGoalState(node)):
                return movements_list
            for successor in problem.getSuccessors(node):
                p,d,c = successor
                new_movement = movements_list + [d]
                pq.push((p,new_movement),(problem.getCostOfActions(new_movement)+heuristic(p, problem)))
            explored_list.append(node)
    return []


# Abbreviations
bfs = breadthFirstSearch
dfs = depthFirstSearch
astar = aStarSearch
ucs = uniformCostSearch
