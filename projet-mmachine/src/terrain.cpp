#include "terrain.hpp"
#include "draw.h"
#include "image_io.h"

// Flat Terrain

FlatTerrain::FlatTerrain(const Point &pmin, const Point &pmax) : mesh_(GL_TRIANGLES)
{
  // terrain mesh
  unsigned int a = mesh_.vertex(pmin.x, pmin.y, 0.f);
  unsigned int b = mesh_.vertex(pmax.x, pmin.y, 0.f);
  unsigned int c = mesh_.vertex(pmax.x, pmax.y, 0.f);
  unsigned int d = mesh_.vertex(pmin.x, pmax.y, 0.f);

  mesh_.triangle(a, b, c);
  mesh_.triangle(a, c, d);
}

void FlatTerrain::project(const Point &from, Point &to, Vector &n) const
{
  to.z = 0.f;
  n = Vector(0.f, 0.f, 1.f);
}

void FlatTerrain::draw(const Transform &v, const Transform &p)
{
  ::draw(mesh_, Identity(), v, p);
}

// Image Terrain

ImgTerrain::ImgTerrain(const Point &pmin, const Point &pmax, const char *filename) : mesh_(GL_TRIANGLES), mesh_checkpoints(GL_LINES)
{
  image_ = read_image(filename);

  origin_ = pmin;

  scale_.x = (pmax.x - pmin.x) / image_.width();
  scale_.y = (pmax.y - pmin.y) / image_.height();
  scale_.z = (pmax.z - pmin.z);

  float w = image_.width();
  float h = image_.height();

  srand (time(NULL));

  for (int i = 0; i < h; ++i)
  {
    for (int j = 0; j < w; ++j)
    {
      Color c = image_(i, j);
      mesh_.vertex(pmin.x + i * scale_.x, pmin.y + j * scale_.y, pmin.z + scale_.z * c.r);
      //randomly places obstacles 
      if (scale_.z * c.r >0 && rand() % 100000 <= 1)
      {
        obstacles.push_back(Obstacle((OBSTACLE_TYPE)(rand() % NB_TYPES_OBSTACLES), Point(pmin.x + i * scale_.x, pmin.y + j * scale_.y, pmin.z + scale_.z * c.r)));
      }
    }
  }

  for (int i = 0; i < h - 1; ++i)
  {
    for (int j = 0; j < w - 1; ++j)
    {
      unsigned int p0 = i * w + j;
      unsigned int p1 = i * w + j + 1;
      unsigned int p2 = (i + 1) * w + j;
      unsigned int p3 = (i + 1) * w + j + 1;

      mesh_.triangle(p0, p2, p1);
      mesh_.triangle(p2, p3, p1);
    }
  }

  // read the file containing the coordinates of the checkpoints
  std::ifstream checkpoints_file("projet-mmachine/assets/circuit_simple_checkpoints");
  if (!checkpoints_file)
    std::cerr << "Could not open the checkpoints" << std::endl;

  std::string reader;
  std::string delimiter = ":";
  int count = 0;
  while (getline(checkpoints_file, reader))
  {
    if (count < 2)
    {
      Point playerSpawn;

      size_t pos = 0;
      std::string token;
      while ((pos = reader.find(delimiter)) != std::string::npos)
      {
        token = reader.substr(0, pos);
        reader.erase(0, pos + delimiter.length());
        playerSpawn(0) = pmin.x + std::stoi(token) * scale_.x;
      }
      playerSpawn(1) = pmin.y + (image_.height() - std::stoi(reader)) * scale_.y;
      p_spawn[count] = playerSpawn;
    }
    else
    {
      Point checkpoint;

      size_t pos = 0;
      std::string token;
      while ((pos = reader.find(delimiter)) != std::string::npos)
      {
        token = reader.substr(0, pos);
        reader.erase(0, pos + delimiter.length());
        checkpoint(0) = pmin.x + std::stoi(token) * scale_.x;
      }
      checkpoint(1) = pmin.y + (image_.height() - std::stoi(reader)) * scale_.y;
      checkpoints.push_back(checkpoint);
    }
    count += 1;
  }

  checkpoints_file.close();

  int subDivision = 2;

  // subdivide the checkpoints for better precision
  for (int n = 0; n < subDivision; n++)
  {
    std::vector<Point> tabPointsTMP;
    tabPointsTMP.push_back(checkpoints[0]);

    for (int i = 0; i < (int)checkpoints.size() - 1; i++)
    {
      Point tmp = checkpoints[i] + 0.25 * (checkpoints[i + 1] - checkpoints[i]);
      Point tmp2 = checkpoints[i] + 0.75 * (checkpoints[i + 1] - checkpoints[i]);

      tabPointsTMP.push_back(tmp);
      tabPointsTMP.push_back(tmp2);
    }
    tabPointsTMP.push_back(checkpoints.back());
    checkpoints = tabPointsTMP;
  }

  // draw un second mesh en GL lines pour afficher les checkpoints
  for (int i = 0; i < checkpoints.size() - 1; i++)
  {
    mesh_checkpoints.vertex(checkpoints[i]);
    mesh_checkpoints.vertex(checkpoints[i + i]);
  }
}

Color ImgTerrain::pixel(float x, float y) const
{
  float s_x = (x - origin_.x) / scale_.x;
  float s_y = (y - origin_.y) / scale_.y;

  s_x = s_x < image_.width() - 1 ? s_x : image_.width() - 1.5;
  s_x = s_x > 0 ? s_x : 0;
  s_y = s_y < image_.height() - 1 ? s_y : image_.height() - 1.5;
  s_y = s_y > 0 ? s_y : 0;

  int p_x = (int)s_x;
  int p_y = (int)s_y;

  float w_x = s_x - p_x;
  float w_y = s_y - p_y;

  Color c_0 = image_(p_x, p_y) * (1 - w_x) * (1 - w_y);
  Color c_1 = image_(p_x + 1, p_y) * w_x * (1 - w_y);
  Color c_2 = image_(p_x + 1, p_y + 1) * w_x * w_y;
  Color c_3 = image_(p_x, p_y + 1) * (1 - w_x) * w_y;

  return c_0 + c_1 + c_2 + c_3;
}

void ImgTerrain::project(const Point &from, Point &to, Vector &n) const
{

  Color pix = pixel(to.x, to.y);
  float init_length = length(to - from);
  to.z = (origin_.z + pix.r * scale_.z);
  if (init_length > 0 && to.z > from.z && length(to - from) / init_length > 2.)
  {
    to = from;
  }
  n = Vector(0.f, 0.f, 1.f);
}

void ImgTerrain::draw(const Transform &v, const Transform &p)
{
  ::draw(mesh_, Identity(), v, p);
}

void ImgTerrain::drawCheckpoints(const Transform &v, const Transform &p)
{
  ::draw(mesh_checkpoints, Identity(), v, p);
}

// returns the percentage of the circuit the player travelled
float ImgTerrain::getCircuitpercentage(const Point &p)
{
  int closest = getClosestCheckpoint(p);
  int next = (closest + 1) % checkpoints.size();

  /* as we always have the closest - 1, we must ensure that our result does not 
   * goes above 1.
   */
  float offset = fmod(projectVector(p, checkpoints[closest], checkpoints[next]), 1.f);
  if (offset > 1)
  {
    closest += 1;
    offset -= 1;
  }

  return (closest + offset) / (checkpoints.size() - 1) * 100;
}

// returns the index of the closest checkpoint
int ImgTerrain::getClosestCheckpoint(const Point &p)
{
  int closest = 0;
  float lastDist = INFINITY;
  for (int i = 1; i < checkpoints.size(); i++)
  {
    float dist = distance2(checkpoints[i], p);
    if (dist < lastDist)
    {
      closest = i;
      lastDist = dist;
    }
  }

  /*
    we always return the closest - 1 because if we do not, being at 51% between 
    2 checkpoints will return a checkpoitn we haven't passed yet, leading to bad results.
    so we return the closest - 1
  */
  return closest - 1;
}

Point ImgTerrain::getFirstCheckpoint()
{
  return checkpoints[0];
}

// project p in the direction of c2 - c1
// then gets the % of distance between c1 and c2
float ImgTerrain::projectVector(const Point &p, const Point &c1, const Point &c2)
{
  Vector playerPos = Vector(c1 - p);
  Vector dir = Vector(c2 - c1);

  Vector proj = dot(playerPos, dir) / length2(dir) * dir;

  return length(proj) / length(dir);
}