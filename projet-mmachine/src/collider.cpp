#include "collider.hpp"


bool Collider::collision(Collider obj2) {
	float dist = distance(this->center, obj2.center);
	return (dist < (this->rayon + obj2.rayon));
}
