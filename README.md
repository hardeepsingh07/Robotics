# Robotics
Robotics C++ Programs

1. WaypointFollow 
   -Take files an input, containing waypoints. Follows each waypoint till file end is not reached.
  
2. GoalLaserObstacleAvoid 
   - Take Goal co-ordinates as an argument and calculate the path to reach goal while avoid obstacles using laser. 
   
3. WaypointFollow 
   - Takes Goal co-ordinates as an argument and pnm file path for map representation. 
   - Read map into 2D-Array while marking walls and open space  
   - Grow obstacle  
   - Calculate wayfront propogation between robot and goal 
   - Calculate waypoints betweens robot and goal using wayfront propogation
   - Perform path relaxation on calculated waypoints 
