//
//  header.h
//  
//
//  Created by Max Bucci on 5/7/14.
//
//

#ifndef _header_h
#define _header_h


typedef struct _point2d {
    int x,y;
} point2D;

typedef struct _position2d {
    double x,y;
}position2D;

typedef struct _polygon2d {
    int n;
    point2D dir;
    point2D* points;
} polygon2D;

typedef struct ballNode {
    int radius, collision, noMoveCnt;
    double angle, velocity, accel, accelAngle;
    position2D point, a, b;
    struct ballNode *next;
} ballNode;


/* geom.c */
int collinear(position2D p, position2D q, position2D r);
int left (position2D a, position2D b, position2D c);
int intersectProp(position2D a, position2D b, position2D c, position2D d);
int ptInPolygon(polygon2D poly, position2D pt);
double degToRad(double deg);
double radToDeg(double rad);
double calculateAngle(position2D p1, position2D p2);
double calculateDistance(position2D p1, position2D p2);
int circleContainsPt(position2D cPt, position2D pt, double circleRad);
position2D* circleLineIntersect(position2D c, double rad, position2D p1, position2D p2);
void checkCollisions(ballNode *list, polygon2D *obs, int numObs, int winSize);
int checkGrowth(ballNode *ball, ballNode *list, polygon2D *obs, int numObs, int winSize);
point2D convertTopoint2d(char* data);

/* ball.c */
ballNode* addBall(ballNode *balls, ballNode *newNode);
void printBallList(ballNode *list);
position2D calculateBallNextPoint(ballNode *ball);
int inBounds(ballNode *b, int winSize);
int inObstacle(position2D pt, polygon2D *obst, int nObst);
int checkBallBoundaryCollision(ballNode *ball, int winSize);
int checkBallBallCollision(ballNode *b1, ballNode *b2);
int checkObsBallCollision(polygon2D obs, ballNode *ball);

/* physics.c */
void updateVelocityWithAccel(ballNode *ball);
void calculateBallLineCollision(ballNode *ball, position2D a, position2D b);
void calculateBallBallCollisions(ballNode *b1, ballNode *b2);


#endif