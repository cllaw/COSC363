/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer 
* Chuan Law (81677469)
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "Cylinder.h"
#include "Cone.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "TextureBMP.h"
#include <GL/glut.h>
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float PHONGS_CONSTANT = 20.0;
const float ETA = 1.01;
const float transparency = 0.5;

TextureBMP texture;                                                     // Define first texture for sphere
TextureBMP texture2;                                                     // Define first texture for sphere
vector<SceneObject*> sceneObjects;                                      //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(10, 40, -3); // First light source
	glm::vec3 light2(-10, 40, -3); // Second light source
	glm::vec3 ambientCol(0.2);   //Ambient color of light
    glm::vec3 specCol(0);
    glm::vec3 specCol2(0);
    glm::vec3 white(1);
    glm::vec3 colorSum(0);
    float LIGHT_INTENSITY = 0.6; // For light source 1
    float LIGHT_INTENSITY2 = 0.4; // For light source 2
    
    //-----------------------Setup------------------------//
    ray.closestPt(sceneObjects);		                                //Compute the closest point of intersetion of objects with the ray
    if(ray.xindex == -1) return backgroundCol;                          //If there is no intersection return background colour
    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor();       //else an object is there, get colour of object
    
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); //Compute the normal vector on the sphere at the point of intersection
    
    //----------------------Lighting----------------------//
    //--- Computing lighting using Phong's illumination---//
    
    glm::vec3 lightVector = light - ray.xpt;                            //The light vector from the point of intersectin towards the light source
    glm::vec3 lightVector2 = light2 - ray.xpt; 
    
    float distanceToLight = glm::length(lightVector);
    float distanceToLight2 = glm::length(lightVector2);
    
    lightVector = normalize(lightVector);                               //Converts the light vector into a unit vector
    lightVector2 = normalize(lightVector2); 
    
    float lDotn = dot(lightVector, normalVector);
    float lDotn2 = dot(lightVector2, normalVector);
    
    // Computing specular reflections
    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);    // using reflect to generate specular reflection vector r
    glm::vec3 reflVector2 = glm::reflect(-lightVector2, normalVector);
    
    float rDotv = dot(reflVector, -ray.dir);
    float rDotv2 = dot(reflVector2, -ray.dir);
    
    rDotv = max(rDotv, 0.0f); // if rDotv is < 0
    rDotv2 = max(rDotv, 0.0f); // if rDotv2 is < 0
    
    //----------------------Shadows----------------------//
    //----------- Adding shadows to the scene------------//
    
    
    Ray shadow(ray.xpt, lightVector);
    Ray shadow2(ray.xpt, lightVector2);
    
    shadow.closestPt(sceneObjects);                                     // Compute the closest point of intersection of objects with the ray
    shadow2.closestPt(sceneObjects);                                    // Enable second shadow computations (DOUBLE CHECK)
    
    if(rDotv >= 0) {specCol = pow(rDotv, PHONGS_CONSTANT) * white;}
    if(rDotv2 >= 0) {specCol2 = pow(rDotv2, PHONGS_CONSTANT) * white;}

    // Compute final lighting and shadows in the scene
    if (lDotn <= 0 || (shadow.xindex > -1 && (shadow.xdist < distanceToLight))) {
        colorSum += ambientCol * materialCol;
    } else {
        colorSum = (ambientCol * materialCol) + (lDotn * materialCol + specCol) * LIGHT_INTENSITY;
    }
    
    if (lDotn2 <= 0 || (shadow2.xindex > -1 && (shadow2.xdist < distanceToLight2))) {
        colorSum += ambientCol * materialCol;
    } else {
        colorSum += (ambientCol * materialCol) + (lDotn * materialCol + specCol2) * LIGHT_INTENSITY2;
    }
    
    //------------------Reflections---------------------//
    //---------Specify the index of the object----------// 
    //----------that should be made reflective----------//
    
    if(ray.xindex == 0 && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step + 1);         //Recursion!
        colorSum = colorSum + (0.8f * reflectedCol);    
    }
    
    //----------Extra Features-------------//
    
        // Checkered floor texture
    if (ray.xindex == 4) {
        int modx = (int)((ray.xpt.x + 50) / 15) % 2;
		int modz = (int)((ray.xpt.z + 50) / 15) % 2;
        
        if((modx && modz) || (!modx && !modz)){
            colorSum += glm::vec3(1, 0.45, 0.70);
        } else { 
            colorSum = glm::vec3(0, 0, 0);
        }
    }
        	
    //-- Background wallpaper
    if(ray.xindex == 7) {
		float s = (ray.xpt.x+150)/300;
		float t = (ray.xpt.y+40)/140;
		colorSum = texture2.getColorAt(s,t);
	}
    
    // Refraction for the sphere (NEEDS TO CHANGE)
    if(ray.xindex == 3 && step < MAX_STEPS){
        
        // Ray going into the sphere
        glm::vec3 refrDir = glm::refract(ray.dir,normalVector, 1.0f / ETA);
        Ray refrRay(ray.xpt,refrDir);
        refrRay.closestPt(sceneObjects);
        if(refrRay.xindex == -1){
            return backgroundCol;
        }
        
        // Compute normal vector inside the sphere
        glm::vec3 normalVec = sceneObjects[refrRay.xindex]->normal(refrRay.xpt);
        glm::vec3 refrDir2 = glm::refract(refrDir, -normalVec, ETA);
        Ray refrRay2(refrRay.xpt, refrDir2);
        refrRay2.closestPt(sceneObjects);
        if(refrRay2.xindex == -1){
            return backgroundCol;
        }
        glm::vec3 refracCol = trace(refrRay2, step + 1);
        colorSum = colorSum * transparency + refracCol * transparency;
        return colorSum;
	}
    
    // Sphere texture for xindex = 1
    if (ray.xindex == 1)
	{
		glm::vec3 center(5.0, 5.0, -90.0);
		glm::vec3 d = normalize(ray.xpt - center);
		float u = (0.5 - atan2(d.z, d.x) + M_PI) / (2 * M_PI);
		float v = 0.5 + asin(d.y) / M_PI;
		colorSum = texture.getColorAt(min (u, (float) 0.99), min(v, (float) 0.99));
   }
        
    // Procedural pattern texture for sphere
    if(ray.xindex == 2) { // 2 Stripe patterns
       int val = ((int) ray.xpt.y) % 2; // Around y-axis (horizontal)
       if (val == 0) {
           colorSum += glm::vec3(0.87, 0.56, 0.56);
       } else {
           colorSum += glm::vec3(0.6, 0.4, 0.2);
       }
    }
    
    return colorSum;
}

//---------------------------Anti-Aliasing------------------------------------------ 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 antiAliasing(glm::vec3 eye, float pixel_size, float xp, float yp){
	
    float quarterX = xp + pixel_size * 0.25;
    float threeQuartersX = xp + pixel_size * 0.75;
    
    float quarterY = yp + pixel_size * 0.25;
    float threeQuarterY = yp + pixel_size * 0.75;
    
	glm::vec3 colorSum(0);
	glm::vec3 avg(0.25);
	
	Ray ray = Ray(eye, glm::vec3(quarterX, quarterY, -EDIST));		
    ray.normalize();				
	colorSum += trace(ray,1);
	
	ray = Ray(eye, glm::vec3(quarterX, threeQuarterY, -EDIST));
	ray.normalize();				
	colorSum += trace(ray,1);
	
	ray = Ray(eye, glm::vec3(threeQuartersX, quarterY, -EDIST));
	ray.normalize();				
	colorSum += trace(ray,1);
	
	ray = Ray(eye, glm::vec3(threeQuartersX, threeQuarterY, -EDIST));
	ray.normalize();				
	colorSum += trace(ray,1);
	
	colorSum *= avg;
	return colorSum;
}

// Function that draws 6 planes to form a box
void drawBox()
{
    // Base
    Plane *plane1 = new Plane (glm::vec3(7., -9, -55),                  //Point A
                              glm::vec3(13., -9, -55),                  //Point B
                              glm::vec3(13., -9, -70),                  //Point C
                              glm::vec3(7., -9, -70),                   //Point D
                              glm::vec3(0, 0.5, 1)                      //Colour
    );
    
    // Left
    Plane *plane2 = new Plane (glm::vec3(7., -9, -55),                  //Point A
                              glm::vec3(7., -3, -55),                   //Point B
                              glm::vec3(7., -3, -70),                   //Point C
                              glm::vec3(7., -9, -70),                   //Point D
                              glm::vec3(0.0, 0.5, 1)                    //Colour
    );
    // Right
    Plane *plane3 = new Plane (glm::vec3(7, -3, -55),                   //Point A
                              glm::vec3(7, -9, -55),                    //Point B
                              glm::vec3(7, -9, -70),                    //Point C
                              glm::vec3(7, -3, -70),                    //Point D
                              glm::vec3(0.0, 0.5, 1)                    //Colour
    );
    
    // BackPlane
    Plane *plane4 = new Plane (glm::vec3(7., -3, -70),                  //Point A
                              glm::vec3(13, -3, -70),                   //Point B
                              glm::vec3(13, -9, -70),                   //Point C
                              glm::vec3(7., -9, -70),                   //Point D
                              glm::vec3(0, 0.5, 1)                      //Colour
    );
    
    // Front
    Plane *plane5 = new Plane (glm::vec3(7., -9, -55),                  //Point A
                              glm::vec3(13, -9, -55),                   //Point B
                              glm::vec3(13, -3, -55),                   //Point C
                              glm::vec3(7., -3, -55),                   //Point D
                              glm::vec3(0, 0.5, 1)                      //Colour
    );
    
    // Top
    Plane *plane6 = new Plane (glm::vec3(7., -3, -55),                  //Point A
                              glm::vec3(13, -3, -55),                   //Point B
                              glm::vec3(13, -3, -70),                   //Point C
                              glm::vec3(7., -3, -70),                   //Point D
                              glm::vec3(0, 0.5, 1)                      //Colour
    );
    
    sceneObjects.push_back(plane1); 
	sceneObjects.push_back(plane2); 
	sceneObjects.push_back(plane3);
	sceneObjects.push_back(plane4);
	sceneObjects.push_back(plane5);
	sceneObjects.push_back(plane6);
    
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);                                          //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);                                                  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	                                //For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	        //direction of the primary ray

		    Ray ray = Ray(eye, dir);		                            //Create a ray originating from the camera in the direction 'dir'
			ray.normalize();				                            //Normalize the direction of the ray to a unit vector
            
		    //glm::vec3 col = trace (ray, 1);                           //Trace the primary ray and get the colour value
            glm::vec3 col = antiAliasing(eye, cellX, xp, yp);           // Enable anti aliasing

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				                            //Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
    
    texture = TextureBMP((char*)"6-star-dragonball.bmp");               // initialize bmp texture
    texture2 = TextureBMP((char*)"Kame_House.bmp");               // initialize bmp texture

	//-- Create a pointer to sphere objects
	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -120.0), 15.0, glm::vec3(0, 0, 1));
	Sphere *sphere2 = new Sphere(glm::vec3(5.0, 5.0, -90.0), 3.0, glm::vec3(1, 0, 0));
	Sphere *sphere3 = new Sphere(glm::vec3(-12.0, -7.0, -85.0), 4.0, glm::vec3(0, 1, 0));
	Sphere *sphere4 = new Sphere(glm::vec3(3.0, -10.0, -70.0), 4.0, glm::vec3(1, 1, 1));

    //--Create a pointer to a plane object
    Plane *plane = new Plane (glm::vec3(-220., -20, 140),               //Point A
                              glm::vec3(220., -20, 140),                //Point B
                              glm::vec3(220., -20, -300),               //Point C
                              glm::vec3(-220., -20, -300),              //Point D
                              glm::vec3(0.5, 0.5, 0)                    //Colour
    );
    
    //--The background plane
    Plane *plane1 = new Plane (glm::vec3(-220., 280, -350),               //Point A
                              glm::vec3(220., 280, -350),                //Point B
                              glm::vec3(220., -40, -350),               //Point C
                              glm::vec3(-220., -40, -350),              //Point D
                              glm::vec3(1, 0, 0)                    //Colour
    );
    
    //--Create a pointer to a cylinder object
    Cylinder *cylinder = new Cylinder(glm::vec3(9, -10.0, -45.0), 1.5, 5, glm::vec3(1, 1, 0));
    
    //--Create a pointer to a cone object
    Cone *cone = new Cone(glm::vec3(-3, -18.0, -80.0), 3, 7, glm::vec3(1, 1, 0));
    

	//--Add the above to the list of scene objects.
	sceneObjects.push_back(sphere1);        // 0
	sceneObjects.push_back(sphere2);        // 1
	sceneObjects.push_back(sphere3);        // 2
	sceneObjects.push_back(sphere4);        // 3
    sceneObjects.push_back(plane);          // 4
    sceneObjects.push_back(cylinder);       // 5
    sceneObjects.push_back(cone);           // 6
    sceneObjects.push_back(plane1);         // 7
    drawBox();
    
}


int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
