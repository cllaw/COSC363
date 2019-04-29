//  ========================================================================
//  COSC363: Computer Graphics (2019);  University of Canterbury.
//
//  FILE NAME: Assignment_1.cpp
//
//  Author: Chuan Law
//
//  Assignment 1: Alien Invasion!
//  Use left/right arrow keys to rotate the scene
//  Use up/down arrow keys to move camera up/down
//  Use HOME to toggle between views
//  Use 'c' to shoot cannon ball
//  Use 's' to launch spaceship
//  ========================================================================
 
#include <iostream>
#include <fstream>
#include <climits>
#include <math.h> 
#include <GL/freeglut.h>
#include "loadTGA.h"

GLUquadric *q;
#define GL_CLAMP_TO_EDGE 0x812F   //To get rid of seams between textures
using namespace std;

//--Globals ---------------------------------------------------------------
float *x, *y, *z;  //vertex coordinate arrays
int *t1, *t2, *t3; //triangles
int nvrt, ntri;    //total number of vertices and triangles

// Initial Camera view
float cam_hgt = 500;
float angle=0, look_x, look_z=1., eye_x, eye_z = -250;  //Camera parameters

// Robot consts
int theta = 20;
int head_theta = 40;
int patrol_theta = 0;
int arm_theta = 0;
int rotate_direction = 1;
int rotate_direction_head = 1;

// Canonball Fire
float canonball_x = 0;
float canonball_y = 0;
bool canon_fired = false;
bool peak_reached = false;

// Moving Crate
int crate_z_pos = 0;
int crate_z_move = 1;

// Spaceship Launch
int spaceship_pos_y = 0;
bool launched = false;

// Spaceship View
bool spaceship_view = false;

// Blinking Light on spaceship
bool blinked = false;
int light_theta = 0;

const int N = 17; // Number of points for the UFO
float vx[N] = {0, 0, 5.0, 5.5, 6.0, 6.5, 7.0, 7.5, 8.0, 9.0, 10.0, 15.0, 16.0, 18.0, 20.0, 20.0, 20.0};
float vy[N] = {0, 10.0, 10.0, 9.5, 9.0, 8.5, 8.0, 7.5, 7.0, 6.0, 5.0, 5.0, 4.0, 3.0, 2.0, 1.0, 0.0};
float vz[N] = {0};

GLuint texId[8];

void loadGLTextures()				// Load bitmaps And Convert To Textures
{
	glGenTextures(8, texId); 		// Create texture ids
	// *** right ***
	glBindTexture(GL_TEXTURE_2D, texId[0]);
	loadTGA("boulder-bay_rt.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** front ***
	glBindTexture(GL_TEXTURE_2D, texId[1]);
	loadTGA("boulder-bay_ft.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** left ***
	glBindTexture(GL_TEXTURE_2D, texId[2]);
	loadTGA("boulder-bay_lf.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** back***
	glBindTexture(GL_TEXTURE_2D, texId[3]);
	loadTGA("boulder-bay_bk.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	

	// *** top ***
	glBindTexture(GL_TEXTURE_2D, texId[4]);
	loadTGA("boulder-bay_up.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	
    
    // *** down ***
	glBindTexture(GL_TEXTURE_2D, texId[6]);
	loadTGA("boulder-bay_dn.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // *** crate ***
    glBindTexture(GL_TEXTURE_2D, texId[5]);
    loadTGA("crate.tga");
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	//Set texture parameters
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
    
    // *** castle walls ***
	glBindTexture(GL_TEXTURE_2D, texId[7]);
	loadTGA("castle_brick.tga");
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

//--Draws a character model constructed using GLUT objects ------------------
void drawRobot()
{
   float lgt_pos1[] = {-10.0f, 14.0f, 0.0f, 1.0f};  // light source for the model
   float spotdir[] = {-0.5, -1, 0}; // direction of the cone to appear like a circle when tilted into the ground
    
    glPushMatrix();
        glScalef(7, 7, 7);
        
        // Buttons
        glPushMatrix();
            // Blinking Red
            if (blinked) glColor3f(1.0, 0.0, 0.0);
            else glColor3f(0.5, 0.0, 0.0);
            glTranslatef(0.6, 6, 0);
            glutSolidSphere(0.9, 10, 10);
        glPopMatrix();
        
        glPushMatrix();
            // Blinking Green
            if (blinked) glColor3f(0.0, 1.0, 0.0);
            else glColor3f(0.0, 0.5, 0.0);
            glTranslatef(-0.6, 6, 0);
            glutSolidSphere(0.9, 10, 10);
        glPopMatrix();
        
        glPushMatrix();
            // Blinking Blue
            if (blinked) glColor3f(0.0, 0.0, 1.0);
            else glColor3f(0.0, 0.0, 0.5);
            glTranslatef(0.6, 5, 0);
            glutSolidSphere(0.9, 10, 10);
        glPopMatrix();
        
        glPushMatrix();
            // Blinking Yellow
            if (blinked) glColor3f(1.0, 1.0, 0.0);
            else glColor3f(0.5, 0.5, 0.0);
            glTranslatef(-0.6, 5, 0);
            glutSolidSphere(0.9, 10, 10);
        glPopMatrix();
        
        glPushMatrix();
          glRotatef(head_theta, 0, -1, 0);
          glColor4f(0.75, 0.75, 0.75, 1);		//Head
          glTranslatef(0, 7.7, 0);
          glutSolidSphere(1.4, 100, 100);
          glPushMatrix();
            glRotatef(90, 0, -1, 0); // Rotate the spotlight direction to face the head direction
            glLightfv(GL_LIGHT1, GL_POSITION, lgt_pos1);   //Spotlight position
            glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spotdir);
          glPopMatrix();
          glTranslatef(0, 0, -1.5);
        
          glPushMatrix();
              glColor3f(0, 0, 0);
              glTranslatef(0, 0, -0.1);
              gluDisk(q, 0.0, 0.7, 20,2); // Eye pupil
          glPopMatrix();
          
          if (blinked) glColor3f(0.30, 0.86, 0.90);
          else glColor3f(0.0, 0.65, 0.70);
          glTranslatef(0, 0, 0);
          gluDisk(q, 0.0, 1.0, 20,2);  // Eye Ring with light
        glPopMatrix();

        glPushMatrix();                 //Torso
          glColor3f(0.3, 0.3, 0.3);		
          glTranslatef(0, 5.5, 0);
          glScalef(3, 3, 1.4);
          glutSolidCube(1);
        glPopMatrix();

        glPushMatrix();
            glColor3f(0.4, 0.4, 0.4);				
            glPushMatrix();                 //Right leg
              glTranslatef(-0.8, 4, 0);
              glRotatef(theta, -1, 0, 0);
              glTranslatef(0.8, -4, 0);
              glTranslatef(-0.8, 2.2, 0);
              glScalef(1, 4.4, 1);
              glutSolidCube(1);
            glPopMatrix();
            
            glPushMatrix();                 //Left leg            
              glTranslatef(0.8, 4, 0);       
              glRotatef(theta, 1, 0, 0);     
              glTranslatef(-0.8, -4, 0);     
              glTranslatef(0.8, 2.2, 0);
              glScalef(1, 4.4, 1);
              glutSolidCube(1);
            glPopMatrix();
        glPopMatrix();

        glPushMatrix();
        //glRotatef(0 + theta, 0, 1, 0); // Moving shield
            glPushMatrix();
                glColor3f(1., 1., 1.);		//Right arm
                glPushMatrix();
                  glTranslatef(-2, 6.5, 0);
                  glTranslatef(0, arm_theta, 0);
                  glTranslatef(2, -6.5, 0);
                  glTranslatef(-2.5, 5, 0);
                  glScalef(1, 5, 3);
                  glutSolidCube(1);
                glPopMatrix();

                glPushMatrix();                 //Left arm
                  glTranslatef(2, 6.5, 0);
                  glTranslatef(0, arm_theta, 0);
                  glTranslatef(-2, -6.5, 0);
                  glTranslatef(2.5, 5, 0);
                  glScalef(1, 5, 3);
                  glutSolidCube(1);
                glPopMatrix();
                
                glPushMatrix();             // connectors
                    glColor3f(0.4, 0.4, 0.4);	
                    glTranslatef(-2.0, 6 + arm_theta, 0);
                    glutSolidCube(1);
                glPopMatrix();
                
                glPushMatrix();
                    glColor3f(0.4, 0.4, 0.4);	
                    glTranslatef(2.0, 6 + arm_theta, 0);
                    glutSolidCube(1);
                glPopMatrix();
            glPopMatrix();
        glPopMatrix();
        
    glPopMatrix();
}

//-- Loads mesh data in OFF format    -------------------------------------
void loadMeshFile(const char* fname)  
{
	ifstream fp_in;
	int num, ne;

	fp_in.open(fname, ios::in);
	if(!fp_in.is_open())
	{
		cout << "Error opening mesh file" << endl;
		exit(1);
	}

	fp_in.ignore(INT_MAX, '\n');				//ignore first line
	fp_in >> nvrt >> ntri >> ne;			    // read number of vertices, polygons, edges

    x = new float[nvrt];                        //create arrays
    y = new float[nvrt];
    z = new float[nvrt];

    t1 = new int[ntri];
    t2 = new int[ntri];
    t3 = new int[ntri];

	for(int i=0; i < nvrt; i++)                         //read vertex list 
		fp_in >> x[i] >> y[i] >> z[i];

	for(int i=0; i < ntri; i++)                         //read polygon list 
	{
		fp_in >> num >> t1[i] >> t2[i] >> t3[i];
		if(num != 3)
		{
			cout << "ERROR: Polygon with index " << i  << " is not a triangle." << endl;  //not a triangle!!
			exit(1);
		}
	}

	fp_in.close();
	cout << " File successfully read." << endl;
}

//--Function to compute the normal vector of a triangle with index tindx ----------
void normal(int tindx)
{
	float x1 = x[t1[tindx]], x2 = x[t2[tindx]], x3 = x[t3[tindx]];
	float y1 = y[t1[tindx]], y2 = y[t2[tindx]], y3 = y[t3[tindx]];
	float z1 = z[t1[tindx]], z2 = z[t2[tindx]], z3 = z[t3[tindx]];
	float nx, ny, nz;
	nx = y1*(z2-z3) + y2*(z3-z1) + y3*(z1-z2);
	ny = z1*(x2-x3) + z2*(x3-x1) + z3*(x1-x2);
	nz = x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2);
	glNormal3f(nx, ny, nz);
}

//-----Computes surface normal vector for the ship----------------------
void normalShip(float x1, float y1, float z1,
            float x2, float y2, float z2,
            float x3, float y3, float z3 )
{
    float nx, ny, nz;
    nx = y1*(z2-z3)+ y2*(z3-z1)+ y3*(z1-z2);
    ny = z1*(x2-x3)+ z2*(x3-x1)+ z3*(x1-x2);
    nz = x1*(y2-y3)+ x2*(y3-y1)+ x3*(y1-y2);
    glNormal3f(nx, ny, nz);
}

// Plane Sweep implementation to draw a spaceship model and make it spin
void drawSpaceship()
{
    glPushMatrix();
        glRotatef(patrol_theta, 0, 1, 0); // make the spaceship spin
        float wx[N], wy[N], wz[N];
        float angStep = 5.0*3.1415926/180.0;  //Rotate in 5 deg steps (converted to radians)
        
        for (int j = 0; j < 72; j++){ // 72 times to complete 360 revolution with 5 degree steps
            glBegin(GL_QUAD_STRIP); 
            for(int i = 0; i < N; i++)
            {
                // Create a Quad strip
                wx[i] = vx[i] * cos(angStep) +  vz[i] * sin(angStep);
                wy[i] = vy[i];
                wz[i] = -vx[i] * sin(angStep) + vz[i] * cos(angStep);

                if(i > 0) normalShip(vx[i-1], vy[i-1], vz[i-1],
                                vx[i], vy[i], vz[i],
                                wx[i], wy[i], wz[i] );
                                
                glVertex3f(vx[i], vy[i], vz[i]);
                glVertex3f(wx[i], wy[i], wz[i]);
            }
            glEnd();
            for(int i = 0; i < N; i++)
            {
               // Update points to repeat surface into a 3D model
                vx[i] = wx[i];
                vy[i] = wy[i];
                vz[i] = wz[i]; 
            }
        }
    glPopMatrix();
}

//--------draws the mesh model of the cannon----------------------------
void drawCannon()
{
    glPushMatrix();
        glColor3f(0.3, 0.3, 0.3);

        //Construct the object model here using triangles read from OFF file
        glBegin(GL_TRIANGLES);
            for(int tindx = 0; tindx < ntri; tindx++)
            {
               normal(tindx);
               glVertex3d(x[t1[tindx]], y[t1[tindx]], z[t1[tindx]]);
               glVertex3d(x[t2[tindx]], y[t2[tindx]], z[t2[tindx]]);
               glVertex3d(x[t3[tindx]], y[t3[tindx]], z[t3[tindx]]);
            }
        glEnd();
    glPopMatrix();
}

//-------draws a castle wall model and maps a texture onto it-----------
void drawCastleWall()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[7]);
    
    // Base
    glPushMatrix();
        glBegin(GL_QUADS);
          glVertex3f(-150.0, 0.1, 10.0);
          glVertex3f(150.0, 0.1,  10.0);
          glVertex3f(150.0, 0.1, -10.0);
          glVertex3f(-150.0, 0.1, -10.0);
        glEnd();
    glPopMatrix();

  // Top
  glPushMatrix();
    glBegin(GL_QUADS);
      glVertex3f(150.0, 150.0, -10.0);
      glVertex3f(-150.0, 150.0,  -10.0);
      glVertex3f(-150.0, 150.0, 10.0);
      glVertex3f(150.0, 150.0, 10.0);
    glEnd();
  glPopMatrix();
  
  // Left
    glPushMatrix();
        glBegin(GL_QUADS);
          glVertex3f(-150.0, 0.0, 10.0);
          glVertex3f(-150.0, 0.0,  -10.0);
          glVertex3f(-150.0, 150.0, -10.0);
          glVertex3f(-150.0, 150.0, 10.0);
        glEnd();
    glPopMatrix();
    
    // Right
    glPushMatrix();
        glBegin(GL_QUADS);
          glVertex3f(150.0, 0.0, -10.0);
          glVertex3f(150.0, 0.0,  10.0);
          glVertex3f(150.0, 150.0, 10.0);
          glVertex3f(150.0, 150.0, -10.0);
        glEnd();
    glPopMatrix();
    
    // Back
    glPushMatrix();
        glBegin(GL_QUADS);
          glTexCoord2f(6.0, 0.0);   glVertex3f(150.0, 0.0, 10.0); // Repeat across 6 times 
          glTexCoord2f(0.0, 0.0);   glVertex3f(-150.0, 0.0,  10.0);
          glTexCoord2f(0.0, 4.0);   glVertex3f(-150.0, 150.0, 10.0); // Repeat up 4 times
          glTexCoord2f(6.0, 4.0);   glVertex3f(150.0, 150.0, 10.0);
        glEnd();
    glPopMatrix();
    
    // Front
    glPushMatrix();
        glBegin(GL_QUADS);
          glTexCoord2f(6.0, 0.0);   glVertex3f(-150.0, 0.0, -10.0);
          glTexCoord2f(0.0, 0.0);   glVertex3f(150.0, 0.0,  -10.0);
          glTexCoord2f(0.0, 4.0);   glVertex3f(150.0, 150.0, -10.0);
          glTexCoord2f(6.0, 4.0);   glVertex3f(-150.0, 150.0, -10.0);
        glEnd();
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
}

//----------draw a floor plane-------------------
void drawFloor()
{
	float black[4] = {0};
    glColor4f(0.96, 0.64, 0.38, 1.0);  //The floor is gray in colour
    glNormal3f(0, 1, 0);
    // Supressing specular reflections
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);

	glBegin(GL_QUADS);
	
	for(int x = -1000; x <= 1000; x += 20)
	{
		for(int z = -1000; z <= 1000; z += 20)
		{
			glVertex3f(x, 0, z);
			glVertex3f(x, 0, z+20);
			glVertex3f(x+20, 0, z+20);
			glVertex3f(x+20, 0, z);
		}
	}
	glEnd();
}

// Draws a cannon ball and translates it inside the cannon iron
void drawCanonBall() 
{
    glPushMatrix();
        glTranslatef(38.88 + canonball_x, 64 + canonball_y, 0);
        glutSolidSphere(5, 36, 18); // (radius, long, lat)
    glPopMatrix();
}

// Draws a full cannon model with the iron, stand and cannon ball
void drawFullCanon()
{
 // Canon ball
    glPushMatrix();
        glColor3f(0.0, 0.0, 0.0);
        drawCanonBall();
    glPopMatrix();
    
    // Canon iron
    glPushMatrix();
        glTranslatef(-20, 30, 0); // Move the center back to the original position
        glRotatef(30, 0, 0, 1); // Rotate the canon
        glTranslatef(20, -30, 0); // Move the center
        drawCannon();
    glPopMatrix();
    
    glPushMatrix(); // Stand parts
        glColor3f(0.8, 0.5, 0.25);
    
        // First cube - right leg
        glPushMatrix();
            glTranslatef(-10, 5, 17);
            glScalef(80, 10, 6);
            glutSolidCube(1);
        glPopMatrix();
        
        // Second cube - left stand
        glPushMatrix();
            glTranslatef(-20, 25, 17);
            glScalef(40, 30, 6);
            glutSolidCube(1);
        glPopMatrix();
        
        // Third cube - left leg
        glPushMatrix();
            glTranslatef(-10, 5, -17);
            glScalef(80, 10, 6);
            glutSolidCube(1);
        glPopMatrix();
        
        // Fourth cube - right stand
        glPushMatrix();
            glTranslatef(-20, 25, -17);
            glScalef(40, 30, 6);
            glutSolidCube(1);
        glPopMatrix();
    glPopMatrix();
}

// Draws a crate and textures them
void drawCrate()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId[5]);
    
    // box base
    glBegin(GL_QUADS);
          glTexCoord2f(0.0, 0.0);   glVertex3f(-20.0, 0.0, 20.0);
          glTexCoord2f(1.0, 0.0);   glVertex3f(20.0, 0.0, 20.0);
          glTexCoord2f(1.0, 1.0);   glVertex3f(20.0, 0.1, -20.0); // 0.1 to not clip with the floor
          glTexCoord2f(0.0, 1.0);   glVertex3f(-20.0, 0.1, -20.0);
    glEnd();
    
    // box left
    glPushMatrix();
        glBegin(GL_QUADS);
          glTexCoord2f(0.0, 0.0);   glVertex3f(-20.0, 0.0, 20.0);
          glTexCoord2f(1.0, 0.0);   glVertex3f(-20.0, 0.0, -20.0);
          glTexCoord2f(1.0, 1.0);   glVertex3f(-20.0, 40.0, -20.0);
          glTexCoord2f(0.0, 1.0);   glVertex3f(-20.0, 40.0, 20.0);
        glEnd();
    glPopMatrix();
    
    // box right
    glPushMatrix();
        glBegin(GL_QUADS);
          glTexCoord2f(0.0, 0.0);   glVertex3f(20.0, 0.0, -20.0);
          glTexCoord2f(1.0, 0.0);   glVertex3f(20.0, 0.0, 20.0);
          glTexCoord2f(1.0, 1.0);   glVertex3f(20.0, 40.0, 20.0);
          glTexCoord2f(0.0, 1.0);   glVertex3f(20.0, 40.0, -20.0);
        glEnd();
    glPopMatrix();
    
    // box back
    glPushMatrix();
        glBegin(GL_QUADS);
          glTexCoord2f(0.0, 0.0);   glVertex3f(20.0, 0.0, 20.0);
          glTexCoord2f(1.0, 0.0);   glVertex3f(-20.0, 0.0, 20.0);
          glTexCoord2f(1.0, 1.0);   glVertex3f(-20.0, 40.0, 20.0);
          glTexCoord2f(0.0, 1.0);   glVertex3f(20.0, 40.0, 20.0);
        glEnd();
    glPopMatrix();
    
    // box front
    glPushMatrix();
        glBegin(GL_QUADS);
          glTexCoord2f(0.0, 0.0);   glVertex3f(-20.0, 0.0, -20.0);
          glTexCoord2f(1.0, 0.0);   glVertex3f(20.0, 0.0, -20.0);
          glTexCoord2f(1.0, 1.0);   glVertex3f(20.0, 40.0, -20.0);
          glTexCoord2f(0.0, 1.0);   glVertex3f(-20.0, 40.0, -20.0);
        glEnd();
    glPopMatrix();
    
    glDisable(GL_TEXTURE_2D);
}

void skybox(){
  glEnable(GL_TEXTURE_2D);

  ////////////////////// LEFT WALL (right) /////////////////////// 
  glBindTexture(GL_TEXTURE_2D, texId[0]);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(-1000,  0, 1000);
  glTexCoord2f(1.0, 0.0); glVertex3f(-1000, 0., -1000);
  glTexCoord2f(1.0, 1.0); glVertex3f(-1000, 1000., -1000);
  glTexCoord2f(0.0, 1.0); glVertex3f(-1000, 1000, 1000);
  glEnd();

  ////////////////////// FRONT WALL ///////////////////////
  glBindTexture(GL_TEXTURE_2D, texId[1]);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(-1000,  0, -1000);
  glTexCoord2f(1.0, 0.0); glVertex3f(1000, 0., -1000);
  glTexCoord2f(1.0, 1.0); glVertex3f(1000, 1000, -1000);
  glTexCoord2f(0.0, 1.0); glVertex3f(-1000,  1000, -1000);
  glEnd();

 ////////////////////// RIGHT WALL (left) ///////////////////////
  glBindTexture(GL_TEXTURE_2D, texId[2]);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(1000,  0, -1000);
  glTexCoord2f(1.0, 0.0); glVertex3f(1000, 0, 1000);
  glTexCoord2f(1.0, 1.0); glVertex3f(1000, 1000,  1000);
  glTexCoord2f(0.0, 1.0); glVertex3f(1000,  1000,  -1000);
  glEnd();

  ////////////////////// REAR WALL ////////////////////////
  glBindTexture(GL_TEXTURE_2D, texId[3]);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f( 1000, 0, 1000);
  glTexCoord2f(1.0, 0.0); glVertex3f(-1000, 0,  1000);
  glTexCoord2f(1.0, 1.0); glVertex3f(-1000, 1000,  1000);
  glTexCoord2f(0.0, 1.0); glVertex3f( 1000, 1000, 1000);
  glEnd();
  
  /////////////////////// TOP //////////////////////////
  glBindTexture(GL_TEXTURE_2D, texId[4]);
  glBegin(GL_QUADS);
  glTexCoord2f(1.0, 0.0); glVertex3f(-1000, 1000, -1000);
  glTexCoord2f(1.0, 1.0); glVertex3f(1000, 1000,  -1000);
  glTexCoord2f(0.0, 1.0); glVertex3f(1000, 1000,  1000);
  glTexCoord2f(0.0, 0.0); glVertex3f(-1000, 1000, 1000);
  glEnd();
  
  ///////////////// FLOOR (Not Visible)//////////////////
  glBindTexture(GL_TEXTURE_2D, texId[6]);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(-1000, 0., 1000);
  glTexCoord2f(1.0, 0.0); glVertex3f(1000, 0.,  1000);
  glTexCoord2f(1.0, 1.0); glVertex3f(1000, 0., -1000);
  glTexCoord2f(0.0, 1.0); glVertex3f(-1000, 0., -1000);
  glEnd();
  
  glDisable(GL_TEXTURE_2D);
}

//--Display: ----------------------------------------------------------------------
//--This is the main display module containing function calls for generating
//--the scene.
void display()  
{
	float lpos[4] = {0., 300., 950., 1.0};  //light's position
    float shadowMat[16] = { 300,0,0,0, -0,0,-950,-1,
                        0,0,300,0, 0,0,0,300 };
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    //GL_LINE = Wireframe;   GL_FILL = Solid
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
    
    // View mode 1 & 2
	if (!spaceship_view) gluLookAt(eye_x, 100, eye_z,  look_x, 100, look_z,   0, 1, 0);
    else gluLookAt(0, 100 + spaceship_pos_y, 225,  0, 100, 0,   0, 1, 0);
    
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);   //set light position
    
    glRotatef(angle, 0.0, 1.0, 0.0);		//rotate the whole scene
    drawFloor();

    glPushMatrix();
        glTranslatef(0, -200, 0); // Move skybox down to avoid floor texture clipping
        skybox(); // TO ENABLE SKYBOX TEXTURES
    glPopMatrix();
    
    // Canon robot
    glPushMatrix();
        glTranslatef(100, 0, 65);
        drawRobot();
    glPopMatrix();
    
    // Draws spaceship
    glPushMatrix();
        glTranslatef(0, 50 + spaceship_pos_y, 300); // moves it into castle
        glScalef(4, 4, 4); // Scale whole ship
        //Light
        glPushMatrix();
            // Blinking colour
            if (blinked) glColor3f(1.0, 0.0, 0.0);
            else glColor3f(0.5, 0.0, 0.0);
            glRotatef(-light_theta, 0, 1, 0); // make the light spin
            glTranslatef(10, 20, 0);
            glutSolidSphere(1.5, 10, 10);
        glPopMatrix();
        glColor4f(0.235, 0.816, 0.439, 1);
        drawSpaceship();
    glPopMatrix();
    
    // Draws spaceship shadow
    glDisable(GL_LIGHTING);
    glPushMatrix();
        glTranslatef(0, 1, 0);
        glMultMatrixf(shadowMat);
        glTranslatef(0, 0 + spaceship_pos_y, 300);
        glScalef(4, 4, 4); // Scale whole ship
        //Light
        glPushMatrix();
            glColor4f(0.2, 0.2, 0.2, 1);
            glRotatef(-light_theta, 0, 1, 0); // make the light spin
            glTranslatef(10, 20, 0);
            glutSolidSphere(1, 10, 10);
        glPopMatrix();
        glColor4f(0.2, 0.2, 0.2, 1);
        drawSpaceship();
    glPopMatrix();
    glEnable(GL_LIGHTING);
    
    // Crate being moved by robot.
    glPushMatrix();
        glTranslatef(-300, 0, 200 + crate_z_pos);
        drawCrate();
         glPushMatrix();
            glTranslatef(0, 0, 40); // Behind crate
            drawRobot();
        glPopMatrix();
    glPopMatrix();
    
    // Patrolling Robot
    glPushMatrix();
        glTranslatef(0, 0, 300); // Translate to starting position to rotate around
        glRotatef(0 + patrol_theta, 0, 1, 0);
        glTranslatef(0, 0, 450); // Rotation circumference
        glRotatef(-90, 0, 1, 0);
        drawRobot();
    glPopMatrix();

    // Canon Left
    glPushMatrix();
        glTranslatef(-100, 0, 0);
        glRotatef(90, 0, 1, 0);
        drawFullCanon();
    glPopMatrix();
    
    // Canon Right
    glPushMatrix();
        glTranslatef(100, 0, 0);
        glRotatef(90, 0, 1, 0);
        drawFullCanon();
    glPopMatrix();

    //-----------Castle starts here------------//
    glPushMatrix();
        // Back Part of Castle
        glPushMatrix();
            glTranslatef(0, 0, 500);
            drawCastleWall();
        glPopMatrix();
        
        // Castle front wall #1
        glPushMatrix();
            glTranslatef(100, 0, 100);
            glScalef(0.35, 1, 1);
            drawCastleWall();
        glPopMatrix();
        
        // Castle front wall #2
        glPushMatrix();
            glTranslatef(-100, 0, 100);
            glScalef(0.35, 1, 1);
            drawCastleWall();
        glPopMatrix();
        
        // Castle side wall middle right
        glPushMatrix();
            glTranslatef(-200, 0, 300);
            glRotatef(90, 0, 1, 0);
            drawCastleWall();
        glPopMatrix();
        
        // Castle side wall middle left
        glPushMatrix();
            glTranslatef(200, 0, 300);
            glRotatef(90, 0, 1, 0);
            drawCastleWall();
        glPopMatrix();
        
        // Castle Cylinder pillars
        glPushMatrix();
            glColor3f(0.6, 0.6, 0.6);
            // Castle side Cylinder right
            glPushMatrix();
                glTranslatef(-200, 1, 500);
                glScalef(1, -4, 1);
                glRotatef(90, 1, 0, 0);
                glutSolidCylinder(50, 50, 100, 100);
            glPopMatrix();
            
            // Castle side Cylinder left
            glPushMatrix();
                glTranslatef(200, 1, 500);
                glScalef(1, -4, 1);
                glRotatef(90, 1, 0, 0);
                glutSolidCylinder(50, 50, 100, 100);
            glPopMatrix();
            
            // Castle side Cylinder right front
            glPushMatrix();
                glTranslatef(-200, 1, 100);
                glScalef(1, -4, 1);
                glRotatef(90, 1, 0, 0);
                glutSolidCylinder(50, 50, 100, 100);
            glPopMatrix();
            
            // Castle side Cylinder left front
            glPushMatrix();
                glTranslatef(200, 1, 100);
                glScalef(1, -4, 1);
                glRotatef(90, 1, 0, 0);
                glutSolidCylinder(50, 50, 100, 100);
            glPopMatrix();
        glPopMatrix();
        
        // Cylinder Cones
        glPushMatrix();
            glColor3f(0.7, 0.7, 0.7);
            // Castle Cone left back
            glPushMatrix();
                glTranslatef(200, 200, 500);
                glScalef(1, -1, 1);
                glRotatef(90, 1, 0, 0);
                glutSolidCone(50, 50, 100, 100);
            glPopMatrix();
            
            // Castle Cone right back
            glPushMatrix();
                glTranslatef(-200, 200, 500);
                glScalef(1, -1, 1);
                glRotatef(90, 1, 0, 0);
                glutSolidCone(50, 50, 100, 100);
            glPopMatrix();
            
            // Castle Cone left front
            glPushMatrix();
                glTranslatef(200, 200, 100);
                glScalef(1, -1, 1);
                glRotatef(90, 1, 0, 0);
                glutSolidCone(50, 50, 100, 100);
            glPopMatrix();
            
            // Castle Cone right front
            glPushMatrix();
                glTranslatef(-200, 200, 100);
                glScalef(1, -1, 1);
                glRotatef(90, 1, 0, 0);
                glutSolidCone(50, 50, 100, 100);
            glPopMatrix();
        glPopMatrix();
    glPopMatrix();
    
    glutSwapBuffers();
	glFlush();
}

//------- Initialize OpenGL parameters -----------------------------------
void initialize()
{
    float grey[4] = {0.2, 0.2, 0.2, 1.0};
    float white[4]  = {1.0, 1.0, 1.0, 1.0};
    
    q = gluNewQuadric();
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    
    //	Define spotlight ambient, diffuse, specular properties
    glLightfv(GL_LIGHT1, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT1, GL_SPECULAR, white);
    
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 40.0);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 5.0);
    
    //	Define main scene's ambient, diffuse, specular properties
    glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
    
    loadGLTextures();
    
    loadMeshFile("Cannon.off");				//Specify mesh file name here
	glClearColor (0.0, 0.0, 0.0, 0.0);

	glEnable(GL_LIGHTING);					//Enable OpenGL states
	glEnable(GL_LIGHT0);
 	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    gluQuadricDrawStyle(q, GLU_FILL);
    
    // Make objects in the world shiny to the white value
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_SPECULAR, white);
    glMaterialf(GL_FRONT, GL_SHININESS, 50);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120, 1, 50, 3000);  //The camera view volume  
}

// Timer for feet movement of the robots
void moveTimer(int value)
{
    if(theta >= 20){
        rotate_direction = -5;
        arm_theta ++;
    }
    if(theta <= -20){
        rotate_direction = 5;
        arm_theta --;
    }

    theta = theta + rotate_direction;
    glutPostRedisplay();
    glutTimerFunc(50, moveTimer, 0);
}

// Timer for the head rotation of the robots
void headTimer(int value)
{
    if(head_theta >= 40){
        rotate_direction_head = -3;
    }
    if(head_theta <= -40){
        rotate_direction_head = 3;
    }

    head_theta = head_theta + rotate_direction_head;
    glutPostRedisplay();
    glutTimerFunc(50, headTimer, 0);
}

// Timer for the projectile motion calculation and canon fire
// Resets before it can be fired again
void canonFireTimer(int value)
{
    if (canon_fired) {
        if(!peak_reached){
            canonball_x += 4;
            if (canonball_y == 80) { // max height using 30 degrees and 30 initial speed
                peak_reached = true;
            } else canonball_y ++;
        } else {
            // Descent
            if (canonball_y != -64) { // -64 to account for the starting height of the cannonball
                canonball_y --;
                canonball_x += 4;
            }
        } 
    } else {
        // When cannon is fired while still active
        canonball_x = 0;
        canonball_y = 0;
        return;   
    }
    
    glutPostRedisplay();
    glutTimerFunc(1, canonFireTimer, 0);
}

// Timer for the movement of the crate
void moveBoxTimer(int value)
{
    if (crate_z_pos >= 0) {
        crate_z_move = -2;
    }
    if (crate_z_pos == -300) {
        crate_z_move = 2;
    }
    
    crate_z_pos += crate_z_move;
    glutPostRedisplay();
    glutTimerFunc(50, moveBoxTimer, 0);
}

// Timer for the movement of the patrolling robot around the castle
void patrolTimer(int value)
{
    patrol_theta++;
    glutPostRedisplay();
    glutTimerFunc(50, patrolTimer, 0);
}

// Timer for the speed of rotation of the spaceship light
void lightThetaTimer(int value)
{
    light_theta+= 4;
    glutPostRedisplay();
    glutTimerFunc(50, lightThetaTimer, 0);
}

// Timer for all blinking animations
void blinkTimer(int value)
{
    if (blinked) blinked = false;
    else blinked = true;
    
    glutPostRedisplay();
    glutTimerFunc(100, blinkTimer, 0);
}

// Used to Launch spaceship into the air by increasing y value.
// Resets when it leaves the skybox
void launchSpaceshipTimer(int value)
{
    if (launched && spaceship_pos_y != 950) {
        spaceship_pos_y += 5;
    } else {
        spaceship_pos_y = 0;
        launched = false;
        return;
    }
    
    glutPostRedisplay();
    glutTimerFunc(50, launchSpaceshipTimer, 0);
}

//------------ Special key event callback ---------------------------------
// To enable the use of left and right arrow keys to rotate the scene
// Enables HOME to swap camera view
void special(int key, int x, int y)
{
    if(!spaceship_view) { //TODO: Double Check this potential fix
        if(key == GLUT_KEY_LEFT) angle -= 0.08611111;  //Change direction by 5 degrees
        else if(key == GLUT_KEY_RIGHT) angle += 0.08611111;
        else if(key == GLUT_KEY_DOWN)
        {  //Move backward
            eye_x += 5*sin(angle);
            eye_z -= 5*cos(angle);
        }
        else if(key == GLUT_KEY_UP)
        { //Move forward
            eye_x -= 5*sin(angle);
            eye_z += 5*cos(angle);
        }
    }

	look_x = eye_x - 100*sin(angle);
	look_z = eye_z + 100*cos(angle); // Scene is facing the + Z axis
    
    if(key == GLUT_KEY_HOME) {
        if(!spaceship_view) spaceship_view = true;
        else spaceship_view = false;
    }
    
	glutPostRedisplay();
}

//------------ Keyboard key event callback ---------------------------------
// To enable the use of 'c' to shoot a cannon ball and 's' to launch the spaceship
void keyboard(unsigned char key, int x, int y)
{
    // To shoot canonball
    if(!canon_fired && key == 99) {
        glutTimerFunc(50, canonFireTimer, 0);
        canon_fired = true;
    } else if (canon_fired == true && key == 99){
        canon_fired = false;
        peak_reached = false;
        drawCanonBall();
    }
    
    // To launch spaceship
    if(key == 115 && !launched) {
        glutTimerFunc(50, launchSpaceshipTimer, 0);
        launched = true;
    } else if (key == 115 && launched) {
        launched = false;
    }

	glutPostRedisplay();
}

//  ------- Main: Initialize glut window and register call backs -----------
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH);
   glutInitWindowSize (800, 800); 
   glutInitWindowPosition (10, 10);
   glutCreateWindow ("Alien Invasion!");
   initialize();

   glutDisplayFunc(display);
   glutSpecialFunc(special);
   glutKeyboardFunc(keyboard); 
   glutTimerFunc(50, moveTimer, 0);
   glutTimerFunc(50, headTimer, 0);
   glutTimerFunc(50, moveBoxTimer, 0);
   glutTimerFunc(50, patrolTimer, 0);
   glutTimerFunc(50, blinkTimer, 0);
   glutTimerFunc(50, lightThetaTimer, 0);
   glutTimerFunc(50, launchSpaceshipTimer, 0);
   glutMainLoop();
   return 0;
}
