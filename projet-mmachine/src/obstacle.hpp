#ifndef _MMACHINE_OBSTACLE_HPP_
#define _MMACHINE_OBSTACLE_HPP_


#include "mat.h"
#include "wavefront.h"
#include "orbiter.h"
#include "draw.h"
#include "collider.hpp"
#include <string.h>

using namespace std;

class Obstacle {
public:
	Mesh m_object;
	Collider collider;
	Obstacle();
	//! pos_center z = hauteur
	Obstacle(OBSTACLE_TYPE type, Point pos_center);

private:

};



#endif