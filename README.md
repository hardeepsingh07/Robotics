# Robotics
Player/Stage C++ Programs

1. WaypointFollow 
   - Take file containing waypoint as an input. 
   - Follows each waypoint till end of file is not reached.
  
2. GoalLaserObstacleAvoid 
   - Take Goal co-ordinates as an argument and calculate the path to reach goal. 
   - Avoid obstacles using laser. 
   
3. WaypointFollow 
   - Take Goal co-ordinates as an argument and .pnm file path for map representation. 
   - Read map into 2D-Array while marking walls and open spaces  
   - Grow obstacle  
   - Calculate wayfront propogation map between robot and goal 
   - Calculate waypoints betweens robot and goal using wayfront propogation map
   - Perform path relaxation on calculated waypoints 
