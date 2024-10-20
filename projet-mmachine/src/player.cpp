#include "player.hpp"

#include <SDL2/SDL.h>

Player::Player() : 
  terrain_(nullptr),
  forward_(true),
  switchable_(true),
  controller_(nullptr),
  active_(false),
  speed_(0.f, 0.f, 0.f),
  last_time_(0),
  position_(0.f, 0.f, 0.f),
  direction_(1.f, 0.f, 0.f),
  normal_(0.f, 0.f, 0.f),
  acceleration_(0.0005f),
  turning_angle_(3.f),
  max_speed_(8.f),
  friction_(0.02f, 0.1f)
{
    collider_.center = position_;
    collider_.rayon = 0.75;
    collider_.type = PLAYER;
}

void Player::spawn_at(const Point& position, const Vector& direction) {
  //reset position on terrain
  position_ = position ;
  collider_.center = position_;
  direction_ = direction ;
  project(position_) ;

  //reset speed
  speed_ = Vector(0.f, 0.f, 0.f) ;

  //wait for activation
  deactivate() ;
}

void Player::step() {
  //do not move if inactive
  if(!active_) return ;

  //handle friction
  Vector linear = dot(speed_, direction_)*direction_ ;
  Vector lateral = speed_ - linear ;
  speed_ = speed_ - friction_.x * linear ;
  speed_ = speed_ - friction_.y * lateral ;

  //backward or forward
  if(forward_) {
    //handle accelerations
    if(controller_->up()) {
      speed_ = speed_ + direction_ * acceleration_ ;
    }

    if(controller_->down()) {
      speed_ = speed_ - direction_ * acceleration_ ;
      if(dot(speed_, direction_) < 0) {
        speed_ = Vector(0.f, 0.f, 0.f) ;
      }
    }

    //handle rotation
    if(controller_->left()) {
      direction_ = Rotation(normal_, turning_angle_)(direction_) ;
    }
    if(controller_->right()) {
      direction_ = Rotation(normal_, -turning_angle_)(direction_) ;
    }
  } else {
    //handle accelerations
    if(controller_->up()) {
      speed_ = speed_ + direction_ * acceleration_ ;
      if(dot(speed_, direction_) > 0) {
        speed_ = Vector(0.f, 0.f, 0.f) ;
      }
    }

    if(controller_->down()) {
      speed_ = speed_ - direction_ * acceleration_ ;
    }

    //handle rotation
    if(controller_->left()) {
      direction_ = Rotation(normal_, -turning_angle_)(direction_) ;
    }
    if(controller_->right()) {
      direction_ = Rotation(normal_, turning_angle_)(direction_) ;
    }
  }

  if(length(speed_) == 0.f) {
    if(switchable_) {
      if(controller_->down()) {
        forward_ = false ;
      } 
      if(controller_->up()) {
        forward_ = true ;
      }
    }
  }

  switchable_ = !(
      controller_->up() 
      || controller_->down() 
      || controller_->left() 
      || controller_->right()
      ) ;

  //compute new position position
  int time = SDL_GetTicks() ;
  Point new_position = position_ + (time - last_time_) * speed_ ;

  //project
  project(new_position) ;

  //update speed taking projection into account
  if(time != last_time_) {
    speed_ = (new_position - position_) / (time - last_time_) ;
    speed_ = speed_ - dot(speed_, normal_)*normal_ ;
  }

  //update the position
  position_ = new_position ;

  //update time
  last_time_ = time ;
}

void Player::activate() {
  active_ = true ;
}

void Player::deactivate() {
  active_ = false ;
}

Transform Player::transform() {
  step() ;
  return Transform(direction_, cross(normal_, direction_), normal_, position_ - Point()) ;
}

void Player::set_terrain(const Terrain* terrain) {
  terrain_ = terrain ;
}

void Player::set_controller(const Controller* controller) {
  controller_ = controller ;
}

void Player::project(Point& candidate) {
  //project position and get normal
  terrain_->project(position_, candidate, normal_) ;

  //project direction
  direction_ = normalize(direction_ - dot(direction_, normal_)*normal_) ;
}
