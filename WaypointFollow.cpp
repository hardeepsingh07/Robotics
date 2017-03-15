//
//  main.cpp
//  project2
//
//  Created by Hardeep Singh on 1/20/17.
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

bool navigator(char **);
int pilot(double, double);

//Variables
PlayerClient robot(gHostname, gPort);
Position2dProxy pp(&robot, gIndex);

int main(int argc, char * argv[]) {
    try
    {
        //setUp Robot
        robot.Read();
        pp.SetMotorEnable(true);
        
        //Print Intial Values
        cout << "*********************************************************************" << endl;
        cout << "X-Position: " << pp.GetXPos() << " || Y-Position: " << pp.GetYPos() << " || Yaw-Position: " << pp.GetYaw() << endl;
        cout << "*********************************************************************" << endl;
        
        //Read the file and store it in array
        bool reponse = navigator(argv);
        if(reponse) {
            cout << "Robot Goal reached successful" << endl;
        } else {
            cout << "Robot couldn't get to its goal" << endl;
        }
        return 0;
    } catch (PlayerCc::PlayerError & e) {
        std::cerr << e << std::endl;
        return 1;
    }
    
}

bool navigator(char *filename[]) {
    bool finished;
    
    //read number of lines in file to determine array size
    ifstream myFile;
    myFile.open(filename[1]);
    int lines_count = std::count(std::istreambuf_iterator<char>(myFile),std::istreambuf_iterator<char>(), '\n');
    myFile.close();
    
    //allocate array with line_count * 2 and fill in the data
    float points[lines_count * 2];
    int index = 0;
    ifstream myFile2;
    myFile2.open(filename[1]);
    if (myFile2.is_open())
    {
        std::string word;
        while (myFile2 >> word)
        {
            points[index] = atof(word.c_str());
            index++;
        }
        myFile2.close();
        finished = true;
    } else {
        finished = false;
        cout << "Unable to open file";
    }
    
    //if data was read successfully, move the robot with x-y using pilot function
    if(finished) {
        for (int i = 0; i < (lines_count * 2) - 1; i++) {
            int check = pilot(points[i], points[i+1]);
            if(check == 1) {
                cout << "Checked: Player Move Successful" << endl;
            } else {
                finished = false;
                cout << "UnChecked: Player Move Unsuccessful" << endl;
                break;
            }
            //increment the index
            i++;
        }
    }
    return finished;
}

int pilot(double x, double y) {
    double angle, distance;
    double closeEnough = .10;
    double baseSpeed = 10;
    double turnSpeed = 0.1;
    double turnCheck = 10;
    
    while(true) {
        //read robot position and store it for further calculations
        robot.Read();
        double xP = pp.GetXPos();
        double yP = pp.GetYPos();
        double zP = pp.GetYaw();
        
        //apply angle formula, to calculate the angle between current position and goal
        angle = (-1 * zP) + atan2((y - yP), (x - xP));

        //Check if angle is too wide. If yes, slow down, else use base speed
        if(fabs(rtod(angle)) > turnCheck) {
            pp.SetSpeed(turnSpeed, angle);
        } else {
           pp.SetSpeed(baseSpeed, angle);
        }
        
        //apply distance formula, to get the distance between current position and goal
        distance = sqrt(pow(pp.GetXPos() - x, 2) + pow((pp.GetYPos() - y), 2));
        
        //check distance value, is it closeEnough?
        if(distance < closeEnough) {
            pp.SetSpeed(0,0);
            return 1;
        }
        
        //print values
        cout << "X-Position: " << pp.GetXPos() << " || Y-Position: " << pp.GetYPos() << endl;
        cout << "Angle in Radian: " << angle << " || Angle in Degrees: " << rtod(angle) << endl;
        cout << "--------------------------------------------------------------" << endl;
    }
    return 0;
}
