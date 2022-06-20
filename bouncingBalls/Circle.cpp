#include "Circle.h"
#include <cstdlib>

Circle::Circle() {
    this->color = HMM_Vec3(0, 0, 0);

    this->direction = HMM_Vec2(0, 0);
    this->position = HMM_Vec2(0, 0);

    this->size = 0;

    this->speed = 0;

    this->gravityVector = HMM_Vec2(0, 0);
}

Circle::Circle(int minRadius, int maxRadius, int rectangleY, int rectangleX, int minVelocity, int maxVelocity) {
    this->color.R = rand() % 255;
    this->color.G = rand() % 255;
    this->color.B = rand() % 255;

    // make a rand between 0.0 and 1.0, multiple it by 2 and substract 1
    // this should give me -1 to 1 randoms for X and Y for direction
    hmm_vec2 temp = HMM_Vec2(static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1, static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2 - 1);
    // and make it a unit vector
    this->direction = HMM_Normalize(temp);
    
    // Get a random speed
    this->speed = (rand() % (maxVelocity - minVelocity)) + minVelocity;;

    // Make the circle a random size
    this->RandomSize(minRadius, maxRadius);

    // Calculate the initial momentum
    this->velocity = this->direction * speed;

    // place the circle, keep it away from the edges
    // this should work with this->size*2, but I still get circle on bottom and right edge
    this->position.X = rand() % (rectangleX - this->size) + this->size;
    this->position.Y = rand() % (rectangleY - this->size) + this->size;
    
    this->gravityVector = HMM_Vec2(0, 0);

}

Circle::~Circle() {

}

int Circle::GetSize() {
    return this->size;
}

hmm_vec2 Circle::GetPosition() {
    return this->position;
}

hmm_vec3 Circle::GetColor() {
    return this->color;
}

unsigned int Circle::GetSpeed() {
    return this->speed;
}

hmm_vec2 Circle::GetDirection() {
    return this->direction;
}

hmm_vec2 Circle::GetGravityVector() {
    return this->gravityVector;
}

hmm_vec2 Circle::GetVelocity() {
    return this->velocity;
}

bool Circle::SetPosition(float X, float Y) {
    this->position.X = X;
    this->position.Y = Y;
    return true;
}

bool Circle::SetPosition(hmm_vec2 in) {
    this->position = in;
    return true;
}

bool Circle::SetDirection(hmm_vec2 in) {
    this->direction = HMM_Normalize(in);
    return true;
}

bool Circle::ApplyGravity(hmm_vec2 currentGravity) {
    this->gravityVector += currentGravity;
    return true;
}

bool Circle::SetGravityVector(hmm_vec2 currentGravity) {
    this->gravityVector = currentGravity;
    return true;
}

bool Circle::ResetGravitySpeed() {
    this->gravityVector = HMM_Vec2(0, 0);
    return true;
}

bool Circle::SetVelocity (hmm_vec2 in) {
    this->velocity = in;
    return true;
}

bool Circle::RandomSize(unsigned int min, unsigned int max) {
    // Make the circle a random size
    this->size = (rand() % (max - min)) + min;
    return true;
}