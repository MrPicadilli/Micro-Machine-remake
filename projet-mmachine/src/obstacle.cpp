#include "obstacle.hpp"

Obstacle::Obstacle()
{
}

Obstacle::Obstacle(OBSTACLE_TYPE type, Point pos_center)
{

	this->collider.type = type;
	this->m_object = read_mesh(smart_path("../assets/sphere.obj"));
	this->collider.center = pos_center;
	this->collider.rayon = 1;

	if (this->collider.type == BUISSON)
	{
		this->m_object.default_color(Color(0.0f, 1.f, 0.f));
	}
	if (this->collider.type == HUILE)
	{
		this->m_object.default_color(Color(0.0f, 0.f, 0.f));
	}
	if (this->collider.type == CAISSE)
	{
		this->m_object.default_color(Color(.4f, .25f, .13f));
	}
}