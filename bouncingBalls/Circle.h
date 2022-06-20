#include "HandmadeMath.h"

class Circle {
public:
    Circle();
    Circle(int minRadius, int maxRadius, int rectangleY, int rectangleX, int minVelocity, int maxVelocity);
    ~Circle();

    int             GetSize();
    hmm_vec2        GetPosition();
    hmm_vec3        GetColor();
    unsigned int    GetSpeed();
    hmm_vec2        GetDirection();
    hmm_vec2        GetGravityVector();
    hmm_vec2        GetVelocity();

    bool            SetPosition(float X, float Y);
    bool            SetPosition(hmm_vec2 in);
    bool            SetDirection(hmm_vec2 in);
    bool            ApplyGravity(hmm_vec2 currentGravity);
    bool            SetGravityVector(hmm_vec2 currentGravity);
    bool            ResetGravitySpeed();
    bool            SetVelocity(hmm_vec2 in);
    bool            RandomSize(unsigned int min, unsigned int max);

private:
    unsigned int    size;
    hmm_vec2        position;
    hmm_vec3        color;
    unsigned int    speed;
    hmm_vec2        direction;
    hmm_vec2        velocity;
    hmm_vec2        gravityVector;
};