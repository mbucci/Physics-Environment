#include "header.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/* return 1 if point a is equal to point b */
int equalPoints(point2D a, point2D b) {
    
    return ((a.x == b.x) && (a.y == b.y));
}


/* **************************************** */
/* returns the signed area of triangle abc. The area is positive if c
 is to the left of ab, and negative if c is to the right of ab
 */
double signed_area2D(position2D a, position2D b, position2D c) {
    
    return (((b.x - a.x) * (c.y - a.y)) - ((b.y - a.y) * (c.x - a.x)));
}


/* **************************************** */
/* return 1 if p,q,r collinear, and 0 otherwise */
int collinear(position2D p, position2D q, position2D r) {
    
    return (signed_area2D(p,q,r) == 0.0);
}


/* **************************************** */
/* return 1 if c is  strictly left of ab; 0 otherwise */
int left (position2D a, position2D b, position2D c) {
    
    return (signed_area2D(a,b,c) > 0);
}


/* return 1 if ab properly intersects with cd */
int intersectProp(position2D a, position2D b, position2D c, position2D d) {
    
    if (collinear(a,b,c) || collinear(a,b,d) || collinear(a,c,d) || collinear(b,c,d))
        return 0;
    
    return ((left(a,b,c) ^ left(a,b,d)) && (left(c,d,a) ^ left(c,d,b)));
}


/* Ray casting algorithm for checking if a given point is within a given 
 polygon */
int ptInPolygon(polygon2D poly, position2D pt) {
    
    int i, j, c = 0;
    for (i = 0, j = poly.n-1; i < poly.n; j = i++) {
        if ( ((poly.points[i].y > pt.y) != (poly.points[j].y > pt.y)) &&
            (pt.x < (poly.points[j].x-poly.points[i].x) * (pt.y-poly.points[i].y) / (poly.points[j].y-poly.points[i].y) + poly.points[i].x) )
            c = !c;
    }
    return c;
}


/* converts degrees to radians */
double degToRad(double deg) {
    
    return deg * M_PI / 180;
}


/* converts radians to degrees */
double radToDeg(double rad) {
    
    return rad * 180 / M_PI;
}


/* Calculates the angle, in radians, between two positions. No negative
 angles */
double calculateAngle(position2D p1, position2D p2) {
    
    double angle = atan2(p2.y-p1.y, p2.x-p1.x);
    if (angle < 0) angle = 2 * M_PI + angle;
    
    return angle;
}


/* Calcualtes the distance between two positions */
double calculateDistance(position2D p1, position2D p2) {
    
    return sqrt(pow(p1.x - p2.x,2) + pow(p1.y - p2.y,2));
}


/* Checks if a given point is within a given circle. This is done by checking 
 if the distance to the point is less then or equal to the radius of the circle. 
 If it is, then the point is within or on the circle */
int circleContainsPt(position2D cPt, position2D pt, double circleRad) {
    
    double distToPt = sqrt(pow(cPt.x - pt.x,2) + pow(cPt.y - pt.y,2));
    if (circleRad >= distToPt) return 1;
    return 0;
}


/* Checks if a given circle intersects a given line. This is done by 
 computing the closest point on the line to the circle and then checking
 for intersection between that point and the circle. The closest point is
 calculated using vector projection */
position2D* circleLineIntersect(position2D c, double rad, position2D p1, position2D p2) {
    
    double lineDist = calculateDistance(p1,p2);
    double angle = calculateAngle(p1,p2);
    
    double cDist = calculateDistance(c,p1);
    double cAngle = calculateAngle(c,p1);
    
    double bAngle = cAngle - angle;
    if (bAngle < 0) bAngle = -bAngle;
    
    double projDist = abs(cDist*cos(bAngle));
    if (projDist > lineDist) projDist = lineDist;
    
    position2D *proj = (position2D*)malloc(sizeof(position2D));
    proj->x = p1.x + projDist*cos(angle);
    proj->y = p1.y + projDist*sin(angle);
    
    if (circleContainsPt(c, *proj, rad)) return proj;
    
    free(proj);
    return NULL;
}


/* Checks if a growing ball impacts any boundaries, balls or obstacles. If it
 does return 1, 0 otherwise */
int checkGrowth(ballNode *ball, ballNode *list, polygon2D *obs, int numObs, int winSize) {
    
    int i;
    ballNode *temp;
    
    if (list) {
        for (temp = list; temp != NULL; temp = temp->next) {
            if (checkBallBallCollision(ball, temp)) return 1;
        }
    }
    
    for (i = 0; i < numObs; i++) {
        if (checkObsBallCollision(obs[i], ball)) return 1;
    }
    
    if (checkBallBoundaryCollision(ball, winSize)) return 1;
    
    return 0;
}


/* Checks for and calculates collisions between balls with obstacles, boundaries
 and other balls */
void checkCollisions(ballNode *list, polygon2D *obs, int numObs, int winSize) {
    
    int i;
    ballNode *temp, *t1, *t2;
    
    for (t1 = list; t1 != NULL; t1 = t1->next) {
        for (t2 = list; t2 != NULL; t2 = t2->next) {
            if (t1 == t2) break;
            if (checkBallBallCollision(t1,t2)) {
                t1->collision = 1;
                t2->collision = 1;
                calculateBallBallCollisions(t1,t2);
            }
        }
    }
    
    
    for (temp = list; temp != NULL; temp = temp->next) {
        for (i = 0; i < numObs; i++) {
            if (checkObsBallCollision(obs[i], temp)) temp->collision = 1;
        }
        
        if (checkBallBoundaryCollision(temp, winSize)) temp->collision = 1;
    }
}


/* Converts a position in form (x,y) to a point2D */
point2D convertTopoint2d(char* data) {
    
    point2D temp;
    int i = 1;
    int count = 0;
    char *digit = NULL;
    digit = (char*)malloc(2 * sizeof(char));
    
    while (data[i] != ')') {
        if (data[i] == ',') {
            temp.x = atoi(digit);
            count = 0;
            digit[1] = 0;
            digit[2] = 0;
        } else {
            digit[count] = data[i];
            count++;
        }
        i++;
    }
    temp.y = atoi(digit);
    return temp;
}

