# Physics-Environment
Basic Environment simulating collisions between balls of varying speeds and sizes with various shapes. 
Requires OpenGL

Compile with make.

To Run:
  ./plan <objects txt file> <gravity on or off (1 on, 0 off)>
  
  Clicking and holding will spawn a new ball in the environment, the longer the click is held the bigger the ball get.
  Releasing the click will create that ball. If the growing ball hits a boundary, object or other ball it will stop
  growing and be created. Balls can not be created within an existing ball or object. A ball is initialized with
  a mass proportional to its size and a speed inversely proportional to its size/mass. 
  
  Use "q" to quit the OpenGl window.

Object files have form:

  Number of objects in file
  *Number of vertices of object
  (x coord, y coord)
  .
  .
  .
  
  
