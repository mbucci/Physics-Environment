/* plan.c
 
 Laura Toma
 
 What it does:
 
 
 */

#include "header.h"
#include "rtimer.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif


GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

GLint fillmode = 0;

/* forward declarations of functions */
void display(void);
void keypress(unsigned char key, int x, int y);
void mousePress(int button, int state, int x, int y);
void main_menu(int value);
void timerfunc();


/* ******************** */
/* global variables */
/* ******************** */
const int WINDOWSIZE = 500;

/* the current position of the robot.  Will be be updated by next_move() */
point2D current_pos;

/* the array of obstacles */
int nobstacles;
polygon2D* obst = NULL;

/* click information */
position2D clickPt;
int radius = 0;
int onClick = 0;

/* balls list */
ballNode *ballList;

/* gravity */
int gravity = 0;


/* Reads in obstacle data from .txt files in the current directory */
void readObstacles(char* inputfname){
    char mode1[] = "r";
    char *file1 = NULL;
    char buff[10];
    file1 = (char*)malloc(100 * sizeof(char));
    
    getcwd(file1);
    strcat(file1, "/");
    strcat(file1, inputfname);
    FILE *file_ptr1;
    file_ptr1 = fopen(file1, mode1);
    
    int obstCount = 0;
    int pointCount = 0;
    while (fscanf(file_ptr1, "%s", buff) != EOF) {
        if (!obst) {
            nobstacles = buff[0] - '0';
            obst = (polygon2D*)malloc(nobstacles * sizeof(polygon2D));
        } else if (buff[0] == '*') {
            char *num = buff;
            num++;
            obst[obstCount].n = atoi(num);
            obst[obstCount].points = (point2D*)malloc(obst[obstCount].n * sizeof(point2D));
            obst[obstCount].dir.x = 1;
            obst[obstCount].dir.y = -1;
            obstCount++;
            pointCount = 0;
        } else {
            obst[obstCount-1].points[pointCount] = convertTopoint2d(buff);
            pointCount++;
        }
    }
}


void print_obstacle(polygon2D p) {
    printf("n=%d : ", p.n);
    int i;
    for (i=0; i<p.n; i++) {
        printf("(%d, %d) ", p.points[i].x, p.points[i].y);
    }
    printf("\n");
}


/* ****************************** */
/* create and initialize  the obstacles array from input file */
//note: the obstacles are in [0,WINDOWSIZE]x[0,WINDOWSIZE]
void initialize_scene(char* inputfname) {
    
    //here should read scene from file
    readObstacles(inputfname);

    printf("initialized %d obstacles:\n", nobstacles);
    int i;
    for (i=0; i<nobstacles; i++) {
        print_obstacle(obst[i]);
    }
}


/* ****************************** */
int main(int argc, char** argv) {
    
    if (argc!=3) {
        printf("usage: %s fname. y/n gravity \n", argv[0]);
        exit(1);
    }
    initialize_scene(argv[1]);
    gravity = atoi(argv[2]);
    
    /* initialize GLUT  */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
    glutInitWindowPosition(100,100);
    glutCreateWindow(argv[0]);
    
    /* register callback functions */
    glutDisplayFunc(display);
    glutKeyboardFunc(keypress);
    glutMouseFunc(mousePress);
    glutIdleFunc(timerfunc);
    
    /* init GL */
    /* set background color white*/
    glClearColor(1, 1, 1, 1);
    /* here we can enable depth testing and double buffering and so
     on */
    
    /* give control to event handler */
    glutMainLoop();
    
    free(obst);
    return 0;
}


/* ****************************** */
/* draws the boundaries of the obstacles */
void draw_obstacle(polygon2D p){

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    //set color
    glColor3fv(magenta);
    
    glBegin(GL_POLYGON);
    int i;
    for (i=0; i<p.n; i++) {
        //draw a small square centered at (points[i].x, points[i].y)
        glVertex2f(p.points[i].x, p.points[i].y);
    }
    glEnd();
}


/* draws a circle given a center point, radius, fillType (i.e. filled or not) 
 and color */
void draw_circle(position2D center, int radius, int fillType, GLfloat *color) {
 
    glPolygonMode(GL_FRONT_AND_BACK, fillType);
    glColor3fv(color);
    glLineWidth(3.0);
    
    glBegin(GL_POLYGON);
    for (int i=0; i < 360; i++)
    {
        float degInRad = degToRad(i);
        glVertex2f(center.x+radius*cos(degInRad),center.y+radius*sin(degInRad));
    }
    
    glEnd();
}


/* ****************************** */
void display(void) {
    
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); //clear the matrix
    
    
    /* the default GL window is [-1,1]x[-1,1] with the origin in the
     center the points are in the range (0,0) to (WINSIZE,WINSIZE), so
     they need to be mapped to [-1,1]x [-1,1] */
    glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);
    glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0);
    
    //draw obstacles
    int i;
    for (i=0; i<nobstacles; i++) {
        draw_obstacle(obst[i]);
    }

    ballNode *temp;
    for (temp = ballList; temp != NULL; temp = temp->next) {
        draw_circle(temp->point, temp->radius, GL_FILL, blue);
    }
    
    position2D pt;
    pt.x = clickPt.x;
    pt.y = clickPt.y;
    
    draw_circle(pt, radius, GL_LINE, cyan);
    
    /* execute the drawing commands */
    glFlush();
}



/* ****************************** */
void keypress(unsigned char key, int x, int y) {
    switch(key) {
        case 'q':
            exit(0);
            break;
    }
}


/* Records mouse presses, used for initiating the balls in the environment */
void mousePress(int button, int state, int x, int y) {
    
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        onClick = 1;
        clickPt.x = (double)x;
        clickPt.y = (double)WINDOWSIZE - y;
        
        //prevents growing ball within existing ball
        ballNode *temp;
        for (temp = ballList; temp != NULL; temp = temp->next) {
            if (circleContainsPt(temp->point,clickPt,temp->radius)) onClick = 0;
        }
        
        //prevents growing ball within obstacle
        if (inObstacle(clickPt,obst,nobstacles)) onClick = 0;
    }
    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
        onClick = 0;
    }
}


/* Handler for window re-size event. Called back when the window first appears and
 whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
    
    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset
    gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height);
}


/* Move all the balls in the ball list using their velocity and angle. 
 If gravity is turned on, updates movement with gravity as well. If a 
 Ball hasn't moved in the last two frames it's direction is changed
 randomly, this prevents premature clumping of the balls.
 */
void moveBalls(ballNode *list) {
    
    ballNode *temp;
    position2D bottomPt;
    for (temp = list; temp != NULL; temp = temp->next) {
        if (gravity) updateVelocityWithAccel(temp);
        bottomPt = temp->point;
        bottomPt.y -= temp->radius ;
        
        if (inBounds(temp,WINDOWSIZE) && !inObstacle(bottomPt,obst,nobstacles) && !(temp->collision)) {
            temp->point = calculateBallNextPoint(temp);
            temp->noMoveCnt = 0;
        }  else if (temp->noMoveCnt > 1) {
            temp->angle += (rand() % 361);
        } else {
            temp->noMoveCnt++;
        }
        temp->collision = 0;
    }
}


/* idle timer function for openGl. A growing ball has its radius increase 
 by one for each frame the mouse is held down. Growing balls are drawn in 
 cyan and without fill, once a growing ball stops growing (i.e. mouse click 
 released or collision with ball, obstacle or boundary) its color changes
 to blue and it is filled. After each frame, look for collisions and 
 move balls accordingly.
 */
void timerfunc() {
    
    static int lastFrameTime=0;
    //lastFrameTime++;
    //note: a static variable, remembered from one call to the next
    int now, elapsed_ms;
    
    now = glutGet (GLUT_ELAPSED_TIME);
    elapsed_ms = now - lastFrameTime;
    lastFrameTime=now;
    
    if (onClick) {
        radius++;
        ballNode temp;
        temp.radius = radius;
        temp.point = clickPt;
        temp.velocity = 0.0;
        temp.next = NULL;
        if (checkGrowth(&temp, ballList, obst, nobstacles, WINDOWSIZE)) {
            onClick = 0;
            radius--;
        }
    } else {
        if (radius > 0) {
            ballNode *newNode = (ballNode*)malloc(sizeof(ballNode));
            newNode->point = clickPt;
            newNode->radius = radius;
            newNode->collision = 0;
            newNode->angle = rand() % 361;
            newNode->velocity = 30.0 / pow(radius,2) ;
            newNode->accelAngle = 270;
            newNode->accel = 0.3;
            newNode->noMoveCnt = 0;
            newNode->next = NULL;
            ballList = addBall(ballList, newNode);
            //printBallList(ballList);
        }
        clickPt.x = 0.0;
        clickPt.y = 0.0;
        radius = 0;
    }
    
    checkCollisions(ballList, obst, nobstacles, WINDOWSIZE);
    moveBalls(ballList);
    glutPostRedisplay();
}
