/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (pos, dir). 
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float A = posn.x - center.x;
    float B = posn.z - center.z;
    float D = height - posn.y + center.y;
    float tangent = (radius / height) * (radius / height);
    
    float a = (dir.x * dir.x) + (dir.z * dir.z) - (tangent*(dir.y * dir.y));
    float b = (2 * A * dir.x) + (2 * B * dir.z) + (2 * tangent * D * dir.y);
    float c = (A * A) + (B * B) - (tangent * (D * D));
    
    // Quadratic equation
    float delta = b*b - (4*a*c);
    
    if (fabs(delta) < 0.001) return -1.0;
    if(delta < 0.0) return -1.0;
   
    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);
    float t;
    
    if (t1>t2) {
        t = t2;
    } else {
        t = t1;
    }
        
    float r = posn.y + t*dir.y;
    
    if ((r > center.y) && (r < center.y + height)) {
        return t;
    } else {
        return -1;
    }

}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    float r = sqrt((p.x-center.x) * (p.x-center.x) + (p.z-center.z) * (p.z-center.z));
    glm::vec3 n = glm::vec3(p.x-center.x, r * (radius/height), p.z-center.z);
    n = glm::normalize(n);
    return n;
}
