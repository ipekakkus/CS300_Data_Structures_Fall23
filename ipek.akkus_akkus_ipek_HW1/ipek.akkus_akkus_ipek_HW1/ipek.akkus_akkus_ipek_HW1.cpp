// CS 300 Homework1
// MAZE GENERATION
// IPEK AKKUS
// 30800

#include <iostream> 
#include <vector> // For Maze Matrices
#include "Stack.cpp" //Templated stack
#include <fstream>
#include <string>  
#include <random>
using namespace std;

class RandGen { // Taken directly from the CS 201 codes
public:
    int RandInt(int min, int max) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(min, max);
        return dist(gen);
    }
};

struct mazeCell
{
    // Set coordinates in the matrix
    int x;
    int y;

    // All walls are present in the beginning (1 represents existing wall, 0 represents emptiness).   
    int leftWall = 1;
    int rightWall = 1;
    int upWall = 1;
    int downWall = 1;

    // Track whether the cell has been visited during maze generation and maze solving.
    bool visited_gen = false;
    bool visited_path = false;

    // Track whether the direction is failed to go (in maze implementation)
    bool tryDown = false;
    bool tryUp = false;
    bool tryRight = false;
    bool tryLeft = false;
};

/*
    Iterated Dept First Search Algorithm is used for Maze Generation
    The algorithm:
        Initialize a stack with the start index (0,0)
        Loop Until: stack is empty
            Pop current index off the stack and mark it as visited
            If current index has any unvisited neighbors
                Choose a random unvisited neighbor index
                Remove the wall between the chosen neighbor index and the current index
                Push the current index on the stack
                Push the randomly choose neighbor index on the stack
            Continue the loop.
*/


// ------------------------------- GENERATE MAZE PART -------------------------------------

/*
    To break a wall between 2 cells, there are 2 criteria
    - The neighbor should be unvisited
    - The coordinates of the upcoming cell should not be out of bounds
        Being not out of bound have patterns, as well
            - The cells in 0th column --> left side is not accessible
            - The cells in 0th row --> bottom side is not accessible
            - The cells in Nth column --> right side is not accessible (for N being user-defined max colNum)
            - The cells in Mth row --> up side is not accessible (for N being user-defined max rowNum)

    popDirection functions check these two criteria
        If the neighbor cell satisfies with the criteria, it marks the currentCell's and neighbor's corresponding walls as 0 (broken)
        If not, then return false
*/

bool popDown(vector<vector<mazeCell>>& maze, mazeCell& temp)
{
    if (temp.x + 1 <= maze.size() - 1 && !maze[temp.x + 1][temp.y].visited_gen) // Check for the criteria explained above
    {
        // Break down the walls
        maze[temp.x][temp.y].downWall = 0;
        maze[temp.x + 1][temp.y].upWall = 0;
        // Mark it as visited
        maze[temp.x + 1][temp.y].visited_gen = true;

        return true;
    }
    return false;
}
bool popUp(vector<vector<mazeCell>>& maze, mazeCell& temp)
{
    if (temp.x - 1 >= 0 && !maze[temp.x - 1][temp.y].visited_gen) // Check for the criteria explained above
    {
        // Break down the walls
        maze[temp.x][temp.y].upWall = 0;
        maze[temp.x - 1][temp.y].downWall = 0;
        // Mark it as visited
        maze[temp.x - 1][temp.y].visited_gen = true;

        return true;
    }
    return false;
}
bool popRight(vector<vector<mazeCell>>& maze, mazeCell& temp)
{
    if (temp.y + 1 <= maze[0].size() - 1 && !maze[temp.x][temp.y + 1].visited_gen) // Check for the criteria explained above
    {
        // Break down the walls
        maze[temp.x][temp.y].rightWall = 0;
        maze[temp.x][temp.y + 1].leftWall = 0;
        // Mark it as visited
        maze[temp.x][temp.y + 1].visited_gen = true;

        return true;
    }
    return false;
}
bool popLeft(vector<vector<mazeCell>>& maze, mazeCell& temp)
{
    if (temp.y - 1 >= 0 && !maze[temp.x][temp.y - 1].visited_gen) // Check for the criteria explained above
    {
        // Break down the walls
        maze[temp.x][temp.y].leftWall = 0;
        maze[temp.x][temp.y - 1].rightWall = 0;
        // Mark it as visited
        maze[temp.x][temp.y - 1].visited_gen = true;

        return true;
    }
    return false;
}
bool popRandom(vector<vector<mazeCell>>& maze, mazeCell& temp, Stack<mazeCell>& mazeStack, bool& remainWays)
{
    // Randomly chooses a direction (1: down, 2: left, 3: up, 4: right)
    int randomDirection;
    RandGen randomGenerator;
    randomDirection = randomGenerator.RandInt(1, 4);

    // Try popping the walls in the randomly chosen direction
    if (randomDirection == 1) // Try popping the down wall
    {
        if (popDown(maze, temp)) // If popping down works
        {
            // Move to the down cell and push it onto the stack
            temp = maze[(temp.x + 1)][temp.y];
            mazeStack.push(maze[temp.x][temp.y]);
            return true;
        }
        temp.tryDown = true; // Mark that we tried going down
    }
    else if (randomDirection == 2) // Try popping the left wall
    {
        if (popLeft(maze, temp)) // If popping left works
        {
            // Move to the left cell and push it onto the stack
            temp = maze[temp.x][temp.y - 1];
            mazeStack.push(maze[temp.x][temp.y]);
            return true;
        }
        temp.tryLeft = true; // Mark that we tried going left
    }
    else if (randomDirection == 3) // Try popping the up wall
    {
        if (popUp(maze, temp)) // If popping up works
        {
            // Move to the up cell and push it onto the stack
            temp = maze[temp.x - 1][temp.y];
            mazeStack.push(maze[temp.x][temp.y]);
            return true;
        }
        temp.tryUp = true; // Mark that we tried going up
    }
    else // Try popping the right wall
    {
        if (popRight(maze, temp)) // If popping right works
        {
            // Move to the right cell and push it onto the stack
            temp = maze[temp.x][temp.y + 1];
            mazeStack.push(maze[temp.x][temp.y]);
            return true;
        }
        temp.tryRight = true; // Mark that we tried going right
    }

    // If all directions are tried and none of them worked, mark that there are no remaining ways
    if (temp.tryRight && temp.tryUp && temp.tryLeft && temp.tryDown)
    {
        remainWays = false; // Declare that we failed to go in any direction in that cell 
        return true; // Escape the loop
    }
    return false; // Continue trying other directions
}


void initializeMaze(vector<vector<mazeCell>>& maze) {
    // Sets the x y values of the struct in a matrix form
    for (int x = 0; x < maze.size(); x++) {
        for (int y = 0; y < maze[0].size(); y++) {
            maze[x][y].x = x;
            maze[x][y].y = y;
        }
    }
}
void ReverseStackPrinter(Stack<mazeCell>& stack, ofstream& pout, const int& M)
{
    // Recursively prints the stack elements in reverse order, representing the steps of the requested path.
    mazeCell t;
    if (stack.isEmpty())
    {
        return; // Exit path for void type functions
    }
    stack.pop(t);
    ReverseStackPrinter(stack, pout, M);
    // Convert and write coordinates to the output file in the requested format
    pout << t.y << " " << (M - t.x - 1) << endl;
}
void saveMazeToFile(const vector<vector<mazeCell>>& mazeMatrix, int M, int N, int index) {
    //Output should be in a file, not in the terminal. That's why we need to save them to a newly opened file
    ofstream out;
    string filename = "maze_" + to_string(index) + ".txt";

    out.open(filename.c_str());
    out << M << " " << N << endl;

    // Output in the requested format, flipping upside down
    int z = M - 1;
    for (int x = 0; x < M; x++) {
        for (int y = 0; y < N; y++) {
            out << "x=" << y << " y=" << x << " l=" << mazeMatrix[z][y].leftWall << " r=" << mazeMatrix[z][y].rightWall << " u=" << mazeMatrix[z][y].upWall << " d=" << mazeMatrix[z][y].downWall << std::endl;
        }
        z--;
    }
    out.close();
}

void generateAndSaveMazes(int numOfMaze, int rowNum, int colNum, vector<vector<vector<mazeCell>>>& mazesVector) {

    for (int i = 1; i <= numOfMaze; i++) {
        // Mazes are matrices consists of vectors of mazeCell's
        vector<vector<mazeCell>> mazeMatrix(rowNum, vector<mazeCell>(colNum));
        initializeMaze(mazeMatrix); // Set the matrix form

        Stack<mazeCell> mazeStack; // Generate an empty mazeStack
        mazeStack.push(mazeMatrix[0][0]); //Initialize the stack with the start cell with index (0,0)
        mazeMatrix[0][0].visited_gen = true; // Mark it as visited
        mazeCell currentCell = mazeMatrix[0][0]; // Get the currentCell in order to manipulate the neighbors

        //Loop Until : stack is empty
        while (!mazeStack.isEmpty()) {
            // Two flags for watching the cell status, triedPath for if all ways tried and failedWay for if any way is failed
            // Try & fail is related with the having unvisited neighbor or not
            bool triedPath = false;
            bool failedWay = true;
            while (!triedPath) {
                triedPath = popRandom(mazeMatrix, currentCell, mazeStack, failedWay); // All walls of each cell may have unvisited neighbor (except special border cases), try all ways
            }
            if (!failedWay) { // If all ways failed
                mazeStack.pop(currentCell); // Bactrack to the previous cell and continue loop there to check whether any directions remained or not
            }
        }
        // After the loop, the matrix obtained should be pushed to the mazesVector
        mazesVector.push_back(mazeMatrix);
        saveMazeToFile(mazeMatrix, rowNum, colNum, i);  // Output is required to be in a file in specifed format
    }
}

// --------------------------------- SOLVE MAZE PART ---------------------------------------

bool moveUp(vector<vector<mazeCell>>& maze, mazeCell& temp, mazeCell& prev) {
    // Check if the upper wall is open, not the previous step, and the upper cell wasn't visited before
    if (maze[temp.x][temp.y].upWall == 0 && !(prev.x == temp.x - 1 && prev.y == temp.y) && !maze[temp.x - 1][temp.y].visited_path) {
        maze[temp.x - 1][temp.y].visited_path = true; // Mark the upper cell as visited
        prev = temp; // Update the previous cell
        temp = maze[temp.x - 1][temp.y]; // Move to the upper cell
        return true; // Return true to indicate a successful move
    }
    return false; // Return false if the move is not possible
}
bool moveDown(vector<vector<mazeCell>>& maze, mazeCell& temp, mazeCell& prev) {
    // Check if the lower wall is open, not the previous step, and the lower cell wasn't visited before
    if (maze[temp.x][temp.y].downWall == 0 && !(prev.x == temp.x + 1 && prev.y == temp.y) && !maze[temp.x + 1][temp.y].visited_path) {
        maze[temp.x + 1][temp.y].visited_path = true; // Mark the lower cell as visited
        prev = temp; // Update the previous cell
        temp = maze[temp.x + 1][temp.y]; // Move to the lower cell
        return true; // Return true to indicate a successful move
    }
    return false; // Return false if the move is not possible
}
bool moveRight(vector<vector<mazeCell>>& maze, mazeCell& temp, mazeCell& prev, int colNum) {
    // Check if the right wall is open, within the column bounds, not the previous step, and the right cell wasn't visited before
    if (maze[temp.x][temp.y].rightWall == 0 && temp.y + 1 <= colNum && !(prev.x == temp.x && prev.y == temp.y + 1) && !maze[temp.x][temp.y + 1].visited_path) {
        maze[temp.x][temp.y + 1].visited_path = true; // Mark the right cell as visited
        prev = temp; // Update the previous cell
        temp = maze[temp.x][temp.y + 1]; // Move to the right cell
        return true; // Return true to indicate a successful move
    }
    return false; // Return false if the move is not possible
}
bool moveLeft(vector<vector<mazeCell>>& maze, mazeCell& temp, mazeCell& prev) {
    // Check if the left wall is open, not the previous step, and the left cell wasn't visited before
    if (maze[temp.x][temp.y].leftWall == 0 && !(prev.x == temp.x && prev.y == temp.y - 1) && !maze[temp.x][temp.y - 1].visited_path) {
        maze[temp.x][temp.y - 1].visited_path = true; // Mark the left cell as visited
        prev = temp; // Update the previous cell
        temp = maze[temp.x][temp.y - 1]; // Move to the left cell
        return true; // Return true to indicate a successful move
    }
    return false; // Return false if the move is not possible
}

void savePathToFile(Stack<mazeCell>& solveStack, int mazeID, int x1, int y1, int x2, int y2, int rowNum) {
    ofstream path_out; // Output stream for the path file 
    // Constructing the filename based on maze ID, entry, and exit points
    string filename = "maze_" + to_string(mazeID) + "_path_" + to_string(x1) + "_" + to_string(y1) + "_" + to_string(x2) + "_" + to_string(y2) + ".txt";
    // In the matrix (0,0) cell is implemented to the top left corner. That's why having upWall and leftWall 1 is normal in the first line.
    path_out.open(filename.c_str()); // Open the file with the generated filename 
    ReverseStackPrinter(solveStack, path_out, rowNum); // Print the solution path to the file in reverse order 
    path_out.close(); // Close the output file 
}

int main()
{
    // GENERATE MAZE SECTON
    int numOfMaze, rowNum, colNum;

    cout << "Enter the number of mazes: ";
    cin >> numOfMaze; // K in the homework document

    cout << "Enter the number of rows and columns (M and N): ";
    cin >> rowNum >> colNum; // M and N, respectiely in the homework document

    vector<vector<vector<mazeCell>>> mazesVector; // Empty vector of matrices to carry matrix information -- Note that matrices are also vectors of vectors

    generateAndSaveMazes(numOfMaze, rowNum, colNum, mazesVector); // Generates the maze
    cout << "All mazes are generated." << endl;

    // SOLVE MAZE SECTON

    //Since all the mazes are generated its time to solve them!
    int mazeID, x1, x2, y1, y2;

    cout << "Enter a maze ID between 1 and " << numOfMaze << " inclusive to find a path: ";
    cin >> mazeID;
    cout << endl;

    cout << "Enter x and y coordinates of the entry points (x,y) or (column,row): ";
    cin >> x1 >> y1;
    cout << endl;
    cout << "Enter x and y coordinates of the exit points (x,y) or (column,row): ";
    cin >> x2 >> y2;
    cout << endl;
    
    Stack<mazeCell> solveStack; // Stack for solving the maze
    vector<vector<mazeCell>> theMaze; //maze that we will solve
    mazeCell temp; //temporary current cell
    mazeCell prev; //temporary previous cell -helps avoid going to the previous cell that we came from-

    // Convert coordinates for the starting point
    int a, b;
    a = rowNum - 1 - y1; // Convert row coordinate to match matrix indexing
    b = x1;

    theMaze = mazesVector[mazeID - 1];

    temp = theMaze[a][b]; // Set the current cell as the starting point
    solveStack.push(temp); // Push the starting point onto the stack

    // Convert coordinates for exiting point
    a = rowNum - 1 - y2; // Convert row coordinate to match matrix indexing
    b = x2;

    while (!(temp.x == a && temp.y == b)) {

        bool isFailed = true;

        if (moveUp(theMaze, temp, prev)) {
            solveStack.push(theMaze[temp.x][temp.y]); // Move up, push the new cell to the stack
            isFailed = false;
        }
        else if (moveDown(theMaze, temp, prev)) {
            solveStack.push(theMaze[temp.x][temp.y]); // Move down, push the new cell to the stack
            isFailed = false;
        }
        else if (moveRight(theMaze, temp, prev, colNum)) {
            solveStack.push(theMaze[temp.x][temp.y]); // Move right, push the new cell to the stack
            isFailed = false;
        }
        else if (moveLeft(theMaze, temp, prev)) {
            solveStack.push(theMaze[temp.x][temp.y]); // Move left, push the new cell to the stack
            isFailed = false;
        }

        if (isFailed) {
            // Bactracking part
            solveStack.pop(temp); // Currently executed cell is no longer in the stack 
            solveStack.pop(temp); // Turn back to the previous cell, to check other not tried neighbors 
            solveStack.push(temp); // In order to manipulate the previous cell, make it the top element 
        }
    }

    // Call the solveMaze function to encapsulate the maze-solving steps
    //solveMaze(mazesVector, mazeID, x1, y1, x2, y2, rowNum, colNum);

    // Outputs redirected to the relevant file
    savePathToFile(solveStack, mazeID, x1, y1, x2, y2, rowNum);

    return 0;
}

