#include "box.hpp"

Box::Box()
{
  pmin = Origin();
  pmax = Origin();
  T = Identity();
}

Box::Box(const Point &i_pmin, const Point &i_pmax)
{
  pmin = i_pmin;
  pmax = i_pmax;
  T = Identity();
}

bool Box::collides(const Box &rhs)
{
  std::cout << "my pmin = " << this->pmin << "my_pmax = " << this->pmax << std::endl;
  std::cout << "my pmin = " << this->T[3].x << std::endl;
  std::cout << "my pmin = " << this->T[3].y << std::endl;

  std::cout << "pmin de rhs : " << rhs.pmin << "pmax de rhs :" << rhs.pmax << std::endl;

  /* EXO 1 en 2D
  //bord b1 bas gauche
  if (this->pmin.y + T[3].y<rhs.pmax.y + rhs.T[3].y &&this->pmin.y + T[3].y> rhs.pmin.y + rhs.T[3].y)
  {
    if (this->pmax.x + T[3].x > rhs.pmin.x + rhs.T[3].x && this->pmax.x + T[3].x < rhs.pmax.x + rhs.T[3].x)
    {
      return true;
    }
  }

  //bord b1 bas droite
  if (this->pmin.y + T[3].y<rhs.pmax.y + rhs.T[3].y &&this->pmin.y + T[3].y> rhs.pmin.y + rhs.T[3].y)
  {
    if (this->pmin.x + T[3].x > rhs.pmin.x + rhs.T[3].x && this->pmin.x + T[3].x < rhs.pmax.x + rhs.T[3].x)
    {
      return true;
    }
  }

  //bord b1 haut droite
  if (this->pmax.y + T[3].y<rhs.pmax.y + rhs.T[3].y &&this->pmax.y + T[3].y> rhs.pmin.y + rhs.T[3].y)
  {
    if (this->pmin.x + T[3].x > rhs.pmin.x + rhs.T[3].x && this->pmin.x + T[3].x < rhs.pmax.x + rhs.T[3].x)
    {
      return true;
    }
  }

  //bord b1 haut gauche
  if (this->pmax.y + T[3].y<rhs.pmax.y + rhs.T[3].y &&this->pmax.y + T[3].y> rhs.pmin.y + rhs.T[3].y)
  {
    if (this->pmax.x + T[3].x > rhs.pmin.x + rhs.T[3].x && this->pmax.x + T[3].x < rhs.pmax.x + rhs.T[3].x)
    {
      return true;
    }
  }
  */

  return false;
}
