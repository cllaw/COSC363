/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal(). (NEEDS TO CHANGE)
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir). 
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
    glm::vec3 vdif = posn - center;
    float a = (dir.x * dir.x + dir.z * dir.z);
    float b = 2 * (dir.x * vdif.x + dir.z * vdif.z);
    float c = vdif.x * vdif.x + vdif.z * vdif.z - (radius * radius);
   
    float delta = b*b - (4*a*c);
    
    if (fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;

    float t1 = (-b - sqrt(delta)) / (2 * a);
    float t2 = (-b + sqrt(delta)) / (2 * a);
    
    if(t1 < 0.01) {
		t1 = -1;
	}
    
	if(t2 < 0.01) {
		t2 = -1;
	}

	if (t1 > t2){
		t1 = t2;
		t2 = t1;
    }
    					
    float ypos = posn.y + dir.y*t1;
    if((ypos >= center.y) && (ypos <= center.y + height)){
		return t1;
	}	
    else{
		float ypos = posn.y + dir.y*t2;
		if((ypos >= center.y) && (ypos <= center.y + height)){
			return t2;
		}else{
			return -1.0;
		}
	}

}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    glm::vec3 vdif = p - center;
    glm::vec3 n = glm::vec3 (vdif.x, 0, vdif.z);
    n = glm::normalize(n);
    return n;
}
