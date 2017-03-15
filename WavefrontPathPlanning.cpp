//
//  proj4.cpp
//  Project4
//
//  Created by Hardeep Singh on 3/06/17.
//  Copyright © 2017 Hardeep Singh. All rights reserved.
//

#include <iostream> 
#include <string> 
#include <stdlib.h> 
#include <fstream> 
#include <math.h> 
#include <libplayerc++/playerc++.h>
#include "stdlib.h"
#include "args.h"
using namespace std;
using namespace PlayerCc;

//Variables
PlayerClient aRobot(gHostname, gPort);
Position2dProxy pp( & aRobot, gIndex);

#define SCALE_MAP 2

//map width (row) and height (col), robot row and row col position, goal row and col position
int aRow, aCol, robot_row, robot_col, goal_row, goal_col,
baseReplaceValue, replaceValue, visitedNode, xGlobalTemp, yGlobalTemp;

double temp_robot_row, temp_robot_col;
double grid_row, grid_col;

//temp variables
int counter = 0, tCounter = 0;

//setup variable for wavefront
int nothing, wall, grownWall, goal, robot, minimum_node, min_node_location, reset_min,temp_x,temp_y;

//trigger to print output to file rather than console (true = file, false = console)
bool trigger = false, xTrigger = true, yTrigger = true;

//dynamically allocate array according to user preferences
int * * aMap;

//declare functions here, first
void obstacleGrowth();
int propagate_wavefront(int, int);
void unpropagate(int, int);
int min_surrounding_node_value(int, int);
void moveRobotToGoal();
int conversion(bool);
int pilot(double, double);
void print_map();


int main(int argc, const char * argv[]) {

  //setUp Robot
  aRobot.Read();
  pp.SetMotorEnable(true);

  /* File Input Configuration Start */
  char inputLine1[80], nextChar;
  ifstream inFile("/usr/local/share/stage/worlds/bitmaps/scaled_hospital_section.pnm");

  // Read past first line
  inFile.getline(inputLine1, 80);

  //Get map size
  inFile >> aCol >> aRow;
  cout << "Rows = " << aRow << ", Columns = " << aCol << endl;

  //intialize a variable to robot and set minimum's
  nothing = 0;
  goal = 2;
  wall = (aRow * aCol) + 1; //marks wall
  robot = (aRow * aCol) + 2; //marks robot
  baseReplaceValue = (aRow * aCol) + 3; //marks intial start
  grownWall = (aRow * aCol) + 4; //marks the new wall
  visitedNode = (aRow * aRow) + 5; //marks the visited node
  replaceValue = robot;
  minimum_node = (aRow * aCol);
  min_node_location = (aRow * aCol);;
  reset_min = (aRow * aCol);

  //intialize the array with user specified length
  aMap = new int * [aRow];
  for (int i = 0; i < aRow; ++i) {
    aMap[i] = new int[aCol];
  }

  //Get Robot initial location in terms of row and col 
  temp_robot_row = pp.GetXPos();
  temp_robot_col = pp.GetYPos();
  conversion(false);

  //Get Goal location in terms of row and col
  temp_x = atof(argv[1]);
  temp_y = atof(argv[2]);
  goal_row = fabs((temp_y - 8) * 7);
  goal_col = fabs((temp_x + 20) * 7);

  cout << "Goal : " << goal_row << " || " << goal_col << endl;

  trigger = true;

  //Read Map
  for (int i = 0; i < aRow; i++) {
    for (int j = 0; j < aCol; j++) {
      inFile >> nextChar;
      if (!nextChar) {
        aMap[i / SCALE_MAP][j / SCALE_MAP] = (aRow * aCol) + 1;
      }
    }
  }
  cout << "Map input complete.\n";

  cout << "Starting Obstacle Growth" << endl << endl;
  obstacleGrowth();
  if (!trigger) {
    print_map();
  }
  cout << "Obstacle Growth Complete" << endl << endl;

  cout << "Starting Wavefront" << endl << endl;
  propagate_wavefront(robot_row, robot_col);
  if (!trigger) {
    print_map();
  }
  cout << "Wavefront Complete" << endl << endl;

  cout << "Starting Path Following" << endl << endl;

  //replace value will be 3 when goal is reached
  while (replaceValue != 2) {
    cout << "------------------------" << endl;
    cout << "Robot Location Before: " << robot_row << ":" << robot_col << endl;
    //start moving the robot toward goal
    moveRobotToGoal();
    cout << "Robot Location After: " << robot_row << ":" << robot_col << endl;
    cout << "------------------------" << endl;

    //update robot position
    if (!trigger) {
      print_map();
    }
    cout << "------------------------" << endl << endl;
    cout << "R Row | Col: " << robot_row << " | " << robot_col << " || G Row |  Col: " << goal_row << " | " << goal_col << endl;
  }
  cout << "Path Following Complete" << endl << endl;

  cout << "----------------------Process Finished Result------------" << endl;
  if (trigger) {
    print_map();
  } else {
    print_map();
  }
  cout << "---------------------------------------------------------" << endl;

  //delete array
  for (int i = 0; i < aRow; i++) {
    delete[] aMap[i];
  }
  delete[] aMap;
  return 0;
}

void obstacleGrowth() {
  //go through the all the map and where there is wall, grow it one more pixel for now
  for (int rRow = 0; rRow < aRow; rRow++) {
    for (int rCol = 0; rCol < aCol; rCol++) {
      if (aMap[rRow][rCol] == wall) {
        //up
        if ((rRow - 1) >= 0) {
          if (aMap[rRow - 1][rCol] == nothing) {
            aMap[rRow - 1][rCol] = grownWall;

            //grow two pixel in up direction
            if ((rRow - 2) >= 0) {
              if (aMap[rRow - 2][rCol] == nothing) {
                aMap[rRow - 2][rCol] = grownWall;

                // //grow three pixel in up direction
                // if ((rRow - 3) >= 0) {
                //   if (aMap[rRow - 3][rCol] == nothing) {
                //     aMap[rRow - 3][rCol] = grownWall;
                //   }
                // }
              }
            }
          }
        }

        //down
        if ((rRow + 1) <= (aRow - 1)) {
          if (aMap[rRow + 1][rCol] == nothing) {
            aMap[rRow + 1][rCol] = grownWall;

            //grow two pixel in down direction
            if ((rRow + 2) <= (aRow - 1)) {
              if (aMap[rRow + 2][rCol] == nothing) {
                aMap[rRow + 2][rCol] = grownWall;
                
                // //grow three pixel in down direction
                // if ((rRow + 3) <= (aRow - 1)) {
                //   if (aMap[rRow + 3][rCol] == nothing) {
                //     aMap[rRow + 3][rCol] = grownWall;
                //   }
                // }
              }
            }
          }
        }

        //left
        if ((rCol - 1) >= 0) {
          if (aMap[rRow][rCol - 1] == nothing) {
            aMap[rRow][rCol - 1] = grownWall;

            //grow two pixel in left direction
            if ((rCol - 2) >= 0) {
              if (aMap[rRow][rCol - 2] == nothing) {
                aMap[rRow][rCol - 2] = grownWall;
          
                // //grow three in left direction
                // if ((rCol - 3) >= 0) {
                //   if (aMap[rRow][rCol - 3] == nothing) {
                //     aMap[rRow][rCol - 3] = grownWall;
                //   }
                // }
              }
            }
          }
        }

        //right
        if ((rCol + 1) <= (aCol - 1)) {
          if (aMap[rRow][rCol + 1] == nothing) {
            aMap[rRow][rCol + 1] = grownWall;

            //grow two pixel in right direction
            if ((rCol + 2) <= (aCol - 1)) {
              if (aMap[rRow][rCol + 2] == nothing) {
                aMap[rRow][rCol + 2] = grownWall;
                
                // //grow three in right direction
                // if ((rCol + 3) <= (aCol - 1)) {
                //   if (aMap[rRow][rCol + 3] == nothing) {
                //     aMap[rRow][rCol + 3] = grownWall;
                //   }
                // }
              }
            }
          }
        }

      }
    }
  }
}

int propagate_wavefront(int robot_row, int robot_col) {
  //clear old wavefront
  unpropagate(robot_row, robot_col);

  //insert goal
  aMap[goal_row][goal_col] = goal;

  counter = 0;
  while (counter < (aRow * aCol)) {
    int pRow = 0;
    int pCol = 0;
    while (pRow < aRow && pCol < aCol) {
      if (aMap[pRow][pCol] != wall && aMap[pRow][pCol] != goal && aMap[pRow][pCol] != grownWall) {
        if (min_surrounding_node_value(pRow, pCol) < reset_min && aMap[pRow][pCol] == robot) {
          return min_node_location;
        } else if (minimum_node != reset_min) {
          aMap[pRow][pCol] = minimum_node + 1;
        }
      }

      //move to next block
      pCol++;
      if (pCol == aCol && pRow != aRow) {
        pRow++;
        pCol = 0;
      }
    }
    counter++;
  }
  return 0;
}

void unpropagate(int robot_row, int robot_col) {
  cout << "Starting Unpropagation" << endl << endl;

  //reset
  for (int row = 0; row < aRow; row++) {
    for (int col = 0; col < aCol; col++) {
      if (aMap[row][col] != wall && aMap[row][col] != goal && aMap[row][col] != grownWall) {
        aMap[row][col] = nothing;
      }
    }
  }

  //insert robot position
  aMap[robot_row][robot_col] = robot;

  cout << "Unpropagation Complete" << endl << endl;
}

//this function looks at a node and returns the lowest value around that node
int min_surrounding_node_value(int row, int col) {
  minimum_node = reset_min; //reset minimum

  //down
  if (row < (aRow - 1)) {
    if (aMap[row + 1][col] < minimum_node && aMap[row + 1][col] != nothing) {
      minimum_node = aMap[row + 1][col];
      min_node_location = 3;
    }
  }

  //up
  if (row > 0) {
    if (aMap[row - 1][col] < minimum_node && aMap[row - 1][col] != nothing) {
      minimum_node = aMap[row - 1][col];
      min_node_location = 1;
    }
  }

  //right
  if (col < (aCol - 1)) {
    if (aMap[row][col + 1] < minimum_node && aMap[row][col + 1] != nothing) {
      minimum_node = aMap[row][col + 1];
      min_node_location = 2;
    }
  }

  //left
  if (col > 0) {
    if (aMap[row][col - 1] < minimum_node && aMap[row][col - 1] != nothing) {
      minimum_node = aMap[row][col - 1];
      min_node_location = 4;
    }
  }
  return minimum_node;
}

void moveRobotToGoal() {
  //assign robots current position
  int rRow = robot_row;
  int rCol = robot_col;

  //mark the visting node to follow the path visually
  if (replaceValue == robot) {
    aMap[rRow][rCol] = baseReplaceValue;
  } else {
    aMap[rRow][rCol] = visitedNode;
  }

  //pre-defined maximum value to avoid miscalculations
  int up = (aRow * aCol);
  int down = (aRow * aCol);
  int left = (aRow * aCol);
  int right = (aRow * aCol);

  //up
  if ((rRow - 1) >= 0) {
    if (aMap[rRow - 1][rCol] != wall && aMap[rRow - 1][rCol] != nothing && aMap[rRow - 1][rCol] != grownWall) {
      up = aMap[rRow - 1][rCol];
    }
  }

  //down
  if ((rRow + 1) <= (aRow - 1)) {
    if (aMap[rRow + 1][rCol] != wall && aMap[rRow + 1][rCol] != nothing && aMap[rRow + 1][rCol] != grownWall) {
      down = aMap[rRow + 1][rCol];
    }
  }

  //left
  if ((rCol - 1) >= 0) {
    if (aMap[rRow][rCol - 1] != nothing && aMap[rRow][rCol - 1] != wall && aMap[rRow][rCol - 1] != wall) {
      left = aMap[rRow][rCol - 1];
    }
  }

  //right
  if ((rCol + 1) <= (aCol - 1)) {
    if (aMap[rRow][rCol + 1] != nothing && aMap[rRow][rCol + 1] != wall && aMap[rRow][rCol + 1] != wall) {
      right = aMap[rRow][rCol + 1];
    }
  }

  //get min
  int minValue = std::min(std::min(up, down), std::min(left, right));

  //Get the Location
  if (up == minValue) {
    robot_row = rRow - 1;
    robot_col = rCol;
  } else if (down == minValue) {
    robot_row = rRow + 1;
    robot_col = rCol;
  } else if (left == minValue) {
    robot_row = rRow;
    robot_col = rCol - 1;
  } else if (right == minValue) {
    robot_row = rRow;
    robot_col = rCol + 1;
  }

  //Move to New Location but before move store new location default value
  replaceValue = aMap[robot_row][robot_col];
  aMap[robot_row][robot_col] = robot;

  //convert fron array indexes to Grid points
  conversion(true);

  xTrigger = xGlobalTemp != -20 && xGlobalTemp != grid_row;
  yTrigger = yGlobalTemp != 9 && yGlobalTemp != grid_col;


  tCounter++;  
  if(xGlobalTemp == -20 && yGlobalTemp == 9)
  {
      pilot(grid_row,grid_col);
  }
  else if(xTrigger && yTrigger) {
      xTrigger = false;
      yTrigger = false;
      pilot(grid_row,grid_col);
  }
  else if (tCounter > 2) {
      pilot(grid_row,grid_col);
      tCounter = 0;
  }

  xGlobalTemp = grid_row;
  yGlobalTemp = grid_col;
}

int conversion(bool grid) {
  //Grid == True --> Grid to Index || Grid ++ False --> Index to Grid
  if (!grid) {
    //Grid point to array indexes
    robot_row = fabs((temp_robot_col - 8) * 7);
    robot_col = fabs((temp_robot_row + 20) * 7);

    cout << "-----Grid points to Array Indexes------" << endl;
    cout << "Grid Coordinates: " << temp_robot_row << " | " << temp_robot_col << endl;
    cout << "Robot_Row and Robot_Col Indexes: " << robot_row << " | " << robot_col << endl;
  } else {
    //array indexes to grid point
    grid_row = (robot_col / 7.0) - 20.0;
    grid_col=(((-1)*robot_row)/7)+8;

    cout << "-----Array indexes to Grid points------" << endl;
    cout << "Robot_Row and Robot_Col Indexes: " << robot_row << " | " << robot_col << endl;
    cout << "Grid Coordinates: " << grid_row << " | " << grid_col << endl;
  }
}

int pilot(double x, double y) {
  double angle, distance;
  double closeEnough = .10;
  double baseSpeed = .1;

  cout << "(" << x << ", " << y << ")" << endl;

  while (true) {
    aRobot.Read();

    double xP = pp.GetXPos();
    double yP = pp.GetYPos();
    double zP = pp.GetYaw();

    //apply angle formula, to calculate the angle between current position and goal
    angle = (-1 * zP) + atan2((y - yP), (x - xP));

    //use base speed and angle to turn
    pp.SetSpeed(baseSpeed, angle);
    pp.GoTo(x, y, angle);

    //apply distance formula, to get the distance between current position and goal
    double tDistance = sqrt(pow(pp.GetXPos() - x, 2) + pow((pp.GetYPos() - y), 2));
    distance = sqrt(pow(pp.GetXPos() - temp_x, 2) + pow((pp.GetYPos() - temp_y), 2));

    //check distance value, is it closeEnough?
    if (distance < closeEnough) {
      pp.SetSpeed(0, 0);
      return 1;
    }
    else if (tDistance < closeEnough) { 
        return 1;
    }

    //print values
    cout << "X-Position: " << pp.GetXPos() << " || Y-Position: " << pp.GetYPos() << endl;
    cout << "Angle in Radian: " << angle << " || Angle in Degrees: " << rtod(angle) << endl;
    cout << "--------------------------------------------------------------" << endl;
  }
  return 0;
}

void print_map() {
  if (trigger) {
    ofstream outFile("/usr/local/share/player/examples/libplayerc++/result.txt");

    for (int i = 0; i < aRow; i++) {
      for (int j = 0; j < aCol; j++) {
        if (aMap[i][j] == wall) {
          outFile << "W ";
        } else if (aMap[i][j] == grownWall) {
          outFile << "N ";
        } else if (aMap[i][j] == robot) {
          outFile << "*R* ";
        } else if (aMap[i][j] == goal) {
          outFile << "*G* ";
        } else if (aMap[i][j] == baseReplaceValue) {
          outFile << "*I* ";
        } else if (aMap[i][j] == visitedNode) {
          outFile << "X ";
        } else {
          outFile << aMap[i][j] << " ";
        }
      }
      outFile << endl;
    }
    cout << "Result File created successfully" << endl;

  } else {
    for (int i = 0; i < aRow; i++) {
      for (int j = 0; j < aCol; j++) {
        if (aMap[i][j] == wall) {
          cout << "W ";
        } else if (aMap[i][j] == grownWall) {
          cout << "N ";
        } else if (aMap[i][j] == robot) {
          cout << "*R* ";
        } else if (aMap[i][j] == goal) {
          cout << "*G* ";
        } else if (aMap[i][j] == baseReplaceValue) {
          cout << "*I* ";
        } else if (aMap[i][j] == visitedNode) {
          cout << "X ";
        } else {
          cout << aMap[i][j] << " ";
        }
      }
      cout << endl;
    }
  }
  cout << endl;
}
