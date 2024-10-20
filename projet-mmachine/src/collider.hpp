#ifndef _MMACHINE_COLLIDER_HPP_
#define _MMACHINE_COLLIDER_HPP_



#include "mat.h"
#include "vec.h"
#include <string.h>

using namespace std;

enum OBSTACLE_TYPE {
	BUISSON, 
	CAISSE,
	HUILE,
	NB_TYPES_OBSTACLES,
	PLAYER,
};

class Collider {
public:

	Point center;
	float rayon;
	OBSTACLE_TYPE type;

	bool collision(Collider obj2);

private:

};



#endif