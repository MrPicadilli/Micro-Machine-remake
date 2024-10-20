#ifndef _MMACHINE_TERRAIN_HPP_
#define _MMACHINE_TERRAIN_HPP_

#include "vec.h"
#include "mat.h"
#include "mesh.h"
#include "image.h"
#include "orbiter.h"
#include "obstacle.hpp"

#include <fstream>

class Terrain {
  public :
    virtual void project(const Point& from, Point& to, Vector& n) const = 0 ;
    virtual void draw(const Transform& v, const Transform& p) = 0 ;
} ;


class FlatTerrain : public Terrain {
  public :
    FlatTerrain(const Point& pmin, const Point& pmax) ;
    void project(const Point& from, Point& to, Vector& n) const ;
    void draw(const Transform& v, const Transform& p) ;

  private :
    Mesh mesh_ ;
} ;


class ImgTerrain : public Terrain {
  public :
    ImgTerrain(const Point& pmin, const Point& pmax, const char* filename) ;
    void project(const Point& from, Point& to, Vector& n) const ;
    void draw(const Transform& v, const Transform& p) ;
    void drawCheckpoints(const Transform& v, const Transform& p) ;
    float getCircuitpercentage(const Point& p);
    float projectVector(const Point& p, const Point& c1, const Point& c2);
    Point getFirstCheckpoint();

    std::vector<Obstacle> obstacles;
    std::vector<Point> checkpoints;
    Point p_spawn[2];

  private :
    Color pixel(float x, float y) const ;
    int getClosestCheckpoint(const Point& p);

    Mesh mesh_ ;
    Mesh mesh_checkpoints;
    Image image_ ;

    Vector scale_ ;
    Point origin_ ;

    
} ;

#endif
