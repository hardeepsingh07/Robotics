# Robotics
Robotics C++ Programs

1. WaypointFollow < /br>
   -Take files an input, containing waypoints. Follows each waypoint till file end is not reached. < /br>
  
2. GoalLaserObstacleAvoid </br>
   - Take Goal co-ordinates as an argument and calculate the path to reach goal while avoid obstacles using laser. < /br>
   
3. WaypointFollow < /br>
   - Takes Goal co-ordinates as an argument and pnm file path for map representation. < /br>
   - Read map into 2D-Array while marking walls and open space  < /br>
   - Grow obstacle  < /br>
   - Calculate wayfront propogation between robot and goal < /br>
   - Calculate waypoints betweens robot and goal using wayfront propogation < /br>
   - Perform path relaxation on calculated waypoints  < /br>
