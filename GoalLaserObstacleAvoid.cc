//
//  main.cpp
//  project3
//
//  Created by Hardeep Singh on 2/05/17.
//  Copyright © 2017 Hardeep Singh. All rights reserved.
//

#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <libplayerc++/playerc++.h>
#include "stdlib.h"
#include "args.h"
using namespace std;
using namespace PlayerCc;

int pilot(double, double);

PlayerClient robot(gHostname, gPort);
Position2dProxy pp(&robot, gIndex);
LaserProxy lp(&robot, gIndex);

int main(int argc, char* argv[])
{
    pp.SetMotorEnable (true);
    
    double x = atof(argv[1]);
    double y = atof(argv[2]);
    
    int result = pilot(x,y);
    if(result == 1) {
        cout << "Goal Reached Successfully!" << endl;
    } else {
        cout << "Goal unable to reach, Try again!" << endl;
    }
}

int pilot(double x, double y) {
    double angle, distance;
    double closeEnough = .30;
    double baseSpeed = 10;
    double turnSpeed = 0.1;
    double turnCheck = 10;

    while(true) {
        robot.Read();
        
        //get minimum distance on right and left
        double right = lp.GetMinRight();
        double left = lp.GetMinLeft();
        
        cout << "Right : " << right << " || " << "Left: " << left << endl;

        //limit the robot from touching obstacle and force to go around
        if(right < 0.7) {
            //Handle obstacle on right side
            if(lp[1] < 0.5 && lp[90] > 5) {
                pp.SetSpeed(1, 0);
            } else {
              pp.SetSpeed(0.1, 1); 
            }
        } else if(left < 0.7) {
            //Handle obstacle on left side
            if((lp[179] < 0.5 && lp[90] > 5) || right <= 1) {
                pp.SetSpeed(1, 0);
            } else {
                pp.SetSpeed(0.1, -1);
            }
        }
        else {
            //Handle moving toward GOAL location (Project 2 -: Code)
            double xP = pp.GetXPos();
            double yP = pp.GetYPos();
            double zP = pp.GetYaw();
            
            //Generate Angle
            angle = (-1 * zP) + atan2((y - yP), (x - xP));
                        
            if(fabs(rtod(angle)) > turnCheck) {
                  pp.SetSpeed(turnSpeed, angle);
            } else {
                pp.SetSpeed(baseSpeed, angle);
            }
            
            //Generate Distance
            distance = sqrt(pow(pp.GetXPos() - x, 2) + pow((pp.GetYPos() - y), 2));
            
            if(distance < closeEnough) {
                pp.SetSpeed(0,0);
                return 1;
            }
            
            cout << "X-Position: " << pp.GetXPos() << " || Y-Position: " << pp.GetYPos() << endl;
            cout << "Angle in Radian: " << angle << " || Angle in Degrees: " << rtod(angle) << endl;
            cout << "--------------------------------------------------------------" << endl;
        }
    }
    return 0;
}
