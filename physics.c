;//
//  physics.c
//  
//
//  Created by Max Bucci on 4/22/14.
//
//

#include "header.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


/* calculates the x component of velocity in a two-body elastic collision, 
 using the masses, initial velocities, and angles of the two bodies */
double calculateXVel(double v1x, double v2x, double m1, double m2, double theta1, double theta2, double phi) {
    
    double temp = v1x * cos(theta1 - phi) * (m1 - m2);
    temp += 2 * m2 * v2x * cos(theta2 - phi);
    temp = temp / (m1 + m2);
    temp = temp * cos(phi);
    temp += v1x * sin(theta1 - phi) * cos(phi + (M_PI/2));
    
    //printf("x vel is %f\n", temp);
    return temp;
}


/* calculates the y component of velocity in a two-body elastic collision,
 using the masses, initial velocities, and angles of the two bodies */
double calculateYVel(double v1y, double v2y, double m1, double m2, double theta1, double theta2, double phi) {
    
    double temp = v1y * cos(theta1 - phi) * (m1 - m2);
    temp += 2 * m2 * v2y * cos(theta2 - phi);
    temp = temp / (m1 + m2);
    temp = temp * sin(phi);
    temp += v1y * sin(theta1 - phi) * sin(phi + (M_PI/2));
    
    //printf("y vel is %f\n", temp);
    return temp;
}


/* Adds to effect of downward acceleration (i.e. gravity) to a given ball */
void updateVelocityWithAccel(ballNode *ball) {
    
    double velX, velY;
    velX = ball->velocity*cos(degToRad(ball->angle)) + ball->accel*cos(degToRad(ball->accelAngle));
    velY = ball->velocity*sin(degToRad(ball->angle)) + ball->accel*sin(degToRad(ball->accelAngle));
    
    ball->velocity = sqrt(pow(velX,2) + pow(velY,2));
    ball->angle = radToDeg(atan2(velY,velX));
}


/* Calculates the resulting direction of ball which collides with a given
 line. This is done by determing the incident angle, and then updating the
 ball's movement angle with that angle of reflection */
void calculateBallLineCollision(ballNode *ball, position2D a, position2D b) {
    
    position2D futurePt = calculateBallNextPoint(ball);
    double theta = calculateAngle(ball->point, futurePt);
    double lineAngle = calculateAngle(a,b);
    
    double reflection = 2*lineAngle - theta;
    ball->angle = radToDeg(reflection);
}


/* Calculates the resulting velocities and directions of balls involved
 in a collision. This is done using equations for two-body elastic collision */
void calculateBallBallCollisions(ballNode *b1, ballNode *b2) {
    
    double theta, dist;
    double m1,m2,theta1,theta2;
    double v1x, v1y, v2x, v2y;
    double vel1X, vel1Y, vel2X, vel2Y;
    double angle1, angle2;
    position2D b12, b22;
    
    theta = calculateAngle(b1->point,b2->point);
    dist = calculateDistance(b1->point,b2->point);
    
    m1 = M_PI * pow(b1->radius,2);
    m2 = M_PI * pow(b2->radius,2);
    b12 = calculateBallNextPoint(b1);
    b22 = calculateBallNextPoint(b2);
    theta1 = calculateAngle(b1->point,b12);
    theta2 = calculateAngle(b2->point,b22);
    v1x = fabs(b1->velocity * cos(theta1));
    v1y = fabs(b1->velocity * sin(theta1));
    v2x = fabs(b2->velocity * cos(theta2));
    v2y = fabs(b2->velocity * sin(theta2));
    
    vel1X = calculateXVel(v1x,v2x,m1,m2,theta1,theta2,theta);
    vel1Y = calculateYVel(v1y,v2y,m1,m2,theta1,theta2,theta);
    vel2X = calculateXVel(v2x,v1x,m2,m1,theta2,theta1,theta);
    vel2Y = calculateYVel(v2y,v1y,m2,m1,theta2,theta1,theta);
    
    b1->velocity = sqrt(pow(vel1X,2) + pow(vel1Y,2));
    b2->velocity = sqrt(pow(vel2X,2) + pow(vel2Y,2));
    
    angle1 = atan2(vel1Y,vel1X);
    angle2 = atan2(vel2Y,vel2X);
    if (angle1 < 0) angle1 = 2 * M_PI + angle1;
    if (angle2 < 0) angle2 = 2 * M_PI + angle2;
    
    b1->angle = radToDeg(angle1);
    b2->angle = radToDeg(angle2);
}
