//
//  ball.c
//  
//
//  Created by Max Bucci on 5/8/14.
//
//

#include "header.h"
#include <stdio.h>
#include <math.h>

/* forward declarations */
//int checkObsBallCollision(polygon2D obs, ballNode *ball);
//int checkBallBallCollision(ballNode *b1, ballNode *b2);
//int checkBallBoundaryCollision(ballNode *ball, int winSize);


/* adds a new ballnode to the existing list. If no list exists
 initialize it with the new ball ballnode */
ballNode* addBall(ballNode *balls, ballNode *newNode) {
    
    if (!balls) {
        balls = newNode;
        return balls;
    }
    
    ballNode *temp;
    for (temp = balls; temp->next != NULL; temp = temp->next) {
    }
    temp->next = newNode;
    
    return balls;
}


/* prints the given list of ball nodes */
void printBallList(ballNode *list) {
    
    ballNode *temp;
    for (temp = list; temp != NULL; temp = temp->next) {
        printf("ball radius is %d\n", temp->radius);
    }
    
}


/* calculates a balls next point given its movement angle and velocity */
position2D calculateBallNextPoint(ballNode *ball) {
    
    position2D futurePt;
    
    
    futurePt.x = ball->point.x + ball->velocity*cos(degToRad(ball->angle));
    futurePt.y = ball->point.y + ball->velocity*sin(degToRad(ball->angle));
    
    return futurePt;
}


/* checks if a given ball is within the bounds of the environment. Returns 
 1 if it is, 0 otherwsie */
int inBounds(ballNode *b, int winSize) {
    
    position2D NE,SE,SW,NW;
    position2D N,E,S,W;
    
    NE.x = winSize;
    NE.y = winSize;
    SE.x = winSize;
    SE.y = 0;
    SW.x = 0;
    SW.y = 0;
    NW.x = 0;
    NW.y = winSize;
    
    N = E = S = W = b->point;
    N.y = b->point.y + b->radius - 1;
    E.x = b->point.x + b->radius - 1;
    S.y = b->point.y - b->radius + 1;
    W.x = b->point.x - b->radius + 1;
    
    if (left(SE,NE,E) && left(NE,NW,N) && left(NW,SW,W) && left(SW,SE,S)) return 1;
    
    return 0;
}


/* checks if a given point is within a given obstacle. This is done using
 the ray-casting algorithm. Returns 1 if the point is within the obstacle,
 0 otherwise. */
int inObstacle(position2D pt, polygon2D *obst, int nObst) {
    
    int i;
    
    for (i = 0; i < nObst; i++) {
        if (ptInPolygon(obst[i], pt)) return 1;
    }
    
    return 0;
}


/* Checks for collisions between the list of balls and a given obstacle. 
 Also, if there is a collision, the resulting movement of the ball after
 colliding with a specific line is calculated. If a ball will impact 
 the corner of an obstacle, it's direction reverses. */
int checkObsBallCollision(polygon2D obs, ballNode *ball) {
    
    int i,index;
    position2D a,b,futurePt;
    
    futurePt = calculateBallNextPoint(ball);
    
    for (i = 0; i < obs.n; i++) {
        index = i+1;
        if (i == obs.n - 1) index = 0;
        a.x = (double)obs.points[i].x;
        a.y = (double)obs.points[i].y;
        b.x = (double)obs.points[index].x;
        b.y = (double)obs.points[index].y;
        
        if (circleContainsPt(futurePt,a,ball->radius) || circleContainsPt(futurePt,b,ball->radius))
        {
            ball->angle += 180;
            return 1;
        }
        
        if (circleLineIntersect(futurePt,ball->radius,a,b) || intersectProp(ball->point,futurePt,a,b))
        {
            calculateBallLineCollision(ball,a,b);
            ball->a = a;
            ball->b = b;
            return 1;
        }
    }
    return 0;
}


/* Checks for collisions between two balls. This is done using the 
 circleContainsPt() function. Returns 1 if the two balls will collide, 0
 otherwise */
int checkBallBallCollision(ballNode *b1, ballNode *b2) {
    
    position2D futurePt1, futurePt2;
    
    futurePt1 = calculateBallNextPoint(b1);
    futurePt2 = calculateBallNextPoint(b2);
    
    if (circleContainsPt(futurePt1, futurePt2, b1->radius+b2->radius) || intersectProp(b1->point, futurePt1, b2->point, futurePt2)) {
        return 1;
    }
    return 0;
}


/* Checks for collisions between a given ball and the boundary. Returns 1
 if they're will be a collision, 0 otherwise. If there is a collision, the 
 resulting movement of the ball is also calculated */
int checkBallBoundaryCollision(ballNode *ball, int winSize) {
    
    position2D futurePt,NE,SE,SW,NW;
    
    futurePt = calculateBallNextPoint(ball);
    NE.x = winSize;
    NE.y = winSize;
    SE.x = winSize;
    SE.y = 0;
    SW.x = 0;
    SW.y = 0;
    NW.x = 0;
    NW.y = winSize;
    
    position2D *temp1 = circleLineIntersect(futurePt, ball->radius, SW,SE);
    position2D *temp2 = circleLineIntersect(futurePt, ball->radius, SE,NE);
    position2D *temp3 = circleLineIntersect(futurePt, ball->radius, NE,NW);
    position2D *temp4 = circleLineIntersect(futurePt, ball->radius, NW,SW);
    
    if (temp1|| intersectProp(ball->point, futurePt, SW,SE)) {
        calculateBallLineCollision(ball,SW,SE);
        return 1;
    }
    if (temp2 || intersectProp(ball->point, futurePt, SE,NE)) {
        calculateBallLineCollision(ball,SE,NE);
        return 1;
    }
    if (temp3 || intersectProp(ball->point, futurePt, NE,NW)) {
        calculateBallLineCollision(ball,NE,NW);
        return 1;
    }
    if (temp4 || intersectProp(ball->point, futurePt, NW,SW)) {
        calculateBallLineCollision(ball,NW,SW);
        return 1;
    }
    
    return 0;
}
