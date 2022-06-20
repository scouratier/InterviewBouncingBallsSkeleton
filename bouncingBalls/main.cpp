#include <string>
#include <stdio.h>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <cstdlib>
#include <time.h>

#include "HandmadeMath.h"
#include "Circle.h"
typedef hmm_vec2 vec2;
typedef hmm_vec3 vec3;
typedef hmm_vec4 vec4;
typedef hmm_quaternion quat;
typedef hmm_mat4 mat4;

static const std::string windowTitle = "Hypixel - Bouncing Balls";
static const int windowWidth = 1000;
static const int windowHeight = 400;
static const int numBalls = 20;
static const int minBallRadiusPx = 20;
static const int maxBallRadiusPx = 30;
static const unsigned int minBallVelocityPx = 70;
static const unsigned int maxBallVelocityPx = 80;

hmm_vec2 gravity = HMM_Vec2(0, 0);

/*******************************************************************************************************************/
/* Iterate all circles and re-randomize their size                                                                 */
/*******************************************************************************************************************/
void randomizeSizes(std::vector<Circle>& allCircles)
{
    std::vector<Circle>::iterator itr;
    for (itr = allCircles.begin(); itr != allCircles.end(); itr++) {
        itr->RandomSize(minBallRadiusPx, maxBallRadiusPx);
    }
}

void input(sf::RenderWindow &window, std::vector<Circle>& allCircles) {
    sf::Event event;

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) window.close();

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code)
            {
                case sf::Keyboard::F1:
                    // Randomize mass
                    // Not really sure what this means.
                    // Loop all circles
                    randomizeSizes(allCircles);
                    break;
                case sf::Keyboard::Left:
                    // update UI
                    gravity.X = gravity.X - 0.001f;
                    break;
                case sf::Keyboard::Right:
                    // update UI
                    gravity.X = gravity.X + 0.001f;
                    break;
                case sf::Keyboard::Up:
                    // update UI
                    gravity.Y = gravity.Y - 0.001f;
                    break;
                case sf::Keyboard::Down:
                    gravity.Y = gravity.Y + 0.001f;
                    // update UI
                    break;
            default:
                break;
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------------------------------*/
/* This is used to bounce a ball on a wall                                                                         */
/*-----------------------------------------------------------------------------------------------------------------*/
hmm_vec2 bounceBall(hmm_vec2 oldDirection, hmm_vec2 reflectionPlane, hmm_vec2 reflectionNormal) {
    hmm_vec2 unitReflection = HMM_Normalize(reflectionPlane);
    hmm_vec2 unitReflectionNormal = HMM_Normalize(reflectionNormal);
    return unitReflection * HMM_Dot(oldDirection, unitReflection) - unitReflectionNormal * HMM_Dot(oldDirection, unitReflectionNormal);
}

/*-----------------------------------------------------------------------------------------------------------------*/
/* TODO: This has a lot of repetiion. Probably somewhere I can make a function a make it more readable             */
/*-----------------------------------------------------------------------------------------------------------------*/
void edgeCollision(Circle& in) {
    hmm_vec2 currentPosition = in.GetPosition();
    // Edge collision detection
    // hitting left edge
    if (currentPosition.X - in.GetSize() < 0) {
        // bounce!
        // Make the unit vectors
        hmm_vec2 edgeReflection = HMM_Vec2(0, 1);
        hmm_vec2 edgeNormal = HMM_Vec2(1, 0);
        in.SetVelocity(bounceBall(in.GetVelocity(), edgeReflection, edgeNormal));
        // Push away from the wall
        // The overlap is the total radius minus the distance to the wall
        float overlap = in.GetSize() - currentPosition.X;
        // set new position
        in.SetPosition(in.GetPosition() + edgeNormal * overlap);
        in.ResetGravitySpeed();
    }
    // hitting right edge
    if (currentPosition.X + in.GetSize() > windowWidth) {
        // bounce!
        // Make the unit vectors
        hmm_vec2 edgeReflection = HMM_Vec2(0, 1);
        hmm_vec2 edgeNormal = HMM_Vec2(-1, 0);
        in.SetVelocity(bounceBall(in.GetVelocity(), edgeReflection, edgeNormal));
        in.ResetGravitySpeed();
        // Push away from the wall
        // The overlap is the total radius minus the distance to the wall
        float overlap = in.GetSize() - (windowWidth - currentPosition.X);
        // set new position
        in.SetPosition(in.GetPosition() + edgeNormal * overlap);
    }
    // hitting top edge
    if (currentPosition.Y - in.GetSize() < 0) {
        // bounce!
        // Make the unit vectors
        hmm_vec2 edgeReflection = HMM_Vec2(1, 0);
        hmm_vec2 edgeNormal = HMM_Vec2(0, 1);
        in.SetVelocity(bounceBall(in.GetVelocity(), edgeReflection, edgeNormal));
        in.ResetGravitySpeed();
        // Push away from the wall
        // The overlap is the total radius minus the distance to the wall
        float overlap = in.GetSize() - currentPosition.Y;
        // set new position
        in.SetPosition(in.GetPosition() + edgeNormal * overlap);
    }
    // hitting bottom edge
    if (currentPosition.Y + in.GetSize() > windowHeight) {
        // bounce!
        // Make the unit vectors
        hmm_vec2 edgeReflection = HMM_Vec2(1, 0);
        hmm_vec2 edgeNormal = HMM_Vec2(0, -1);
        in.SetVelocity(bounceBall(in.GetVelocity(), edgeReflection, edgeNormal));
        in.ResetGravitySpeed();
        // Push away from the wall
        // The overlap is the total radius minus the distance to the wall
        float overlap = in.GetSize() - (windowHeight - currentPosition.Y);
        // set new position
        in.SetPosition(in.GetPosition() + edgeNormal * overlap);
    }
}

/*----------------------------------------------------------------------------------------------------------*/
/* Utility function to make a new vector that 90 degrees, clockwise from the input vector                   */
/*----------------------------------------------------------------------------------------------------------*/
hmm_vec2 makeOrthogonalVector(hmm_vec2 in) {
    hmm_vec2 out;
    out.X = in.Y;
    out.Y = -in.X;
    return out;
}

/*----------------------------------------------------------------------------------------------------------*/
/* Function to bounce a ball off of another ball, using velocity and momentum                               */
/* First step is to detect for a collision (On^2)                                                           */
/* Then calculate the new velocities                                                                        */
/* Then set the new velocity                                                                                */
/* Then pull the circles away (the overlap)                                                                 */
/*----------------------------------------------------------------------------------------------------------*/
void circleCollision(std::vector<Circle>::iterator currentCircle, std::vector<Circle>& testCircles) {
    // Loop balls from the current position + 1 (to not test against self
    std::vector<Circle>::iterator itr;
    for (itr = currentCircle+1; itr != testCircles.end(); itr++) {
        // Collision happens when |C1.center - C2.center| < C1.radius+C2.radius
        // Get the vector between the center of C1 and C2 (delta)
        hmm_vec2 deltaBetweenCircles = HMM_Subtract(currentCircle->GetPosition(), itr->GetPosition());
        if (HMM_Length(deltaBetweenCircles) < currentCircle->GetSize() + itr->GetSize()) {
            // Make a unit vector from delta
            hmm_vec2 unitBounceNormal = HMM_Normalize(deltaBetweenCircles);
            // Make a unit vector that's 90 degrees from delta. This is the reflection line for the circles
            hmm_vec2 unitReflectionPlane = HMM_Normalize(makeOrthogonalVector(deltaBetweenCircles));
            // Bounce the balls using momentum
            hmm_vec2 C1newVelocity = HMM_Vec2(0, 0);
            hmm_vec2 C2newVelocity = HMM_Vec2(0, 0);

            // This will calculate the new velocities
            C1newVelocity = (currentCircle->GetVelocity() * (currentCircle->GetSize() - itr->GetSize()) + 2 * (itr->GetSize() * itr->GetVelocity())) / (currentCircle->GetSize() + itr->GetSize());
            C2newVelocity = (itr->GetVelocity() * (itr->GetSize() - currentCircle->GetSize()) + 2 * (currentCircle->GetSize() * currentCircle->GetVelocity())) / (currentCircle->GetSize() + itr->GetSize());

            // Apply the velocities
            currentCircle->SetVelocity(C1newVelocity);
            itr->SetVelocity(C2newVelocity);

            // The balls are now overlapping and should be moved apart
            // If we don't do this, the 2 balls will still be in a collision at the next test
            // and it makes all sorts of weird
            // and it makes all sorts of weird
            float overlap = (currentCircle->GetSize() + itr->GetSize()) - HMM_Length(deltaBetweenCircles);
            // set new positions
            currentCircle->SetPosition(currentCircle->GetPosition() + HMM_Multiply(HMM_Multiply(HMM_Normalize(deltaBetweenCircles),1), overlap / 2));
            itr->SetPosition(itr->GetPosition() + HMM_Multiply(HMM_Multiply(HMM_Normalize(deltaBetweenCircles), -1), overlap / 2));
        }
    }
}

/*--------------------------------------------------------------------------------------------------*/
/* This will increase the velocity in the direction of the gravity every time it's called           */
/*--------------------------------------------------------------------------------------------------*/
void applyGravity(std::vector<Circle>& allCircles) {
    // Loop all circles
    std::vector<Circle>::iterator itr;
    for (itr = allCircles.begin(); itr != allCircles.end(); itr++) {
        // Add gravity to the current direction vector
        itr->ApplyGravity(gravity);
    }
}

/*--------------------------------------------------------------------------------------------------*/
/* This updates the data structures                                                                 */
/* It will manage:                                                                                  */              
/* - collisions with edges                                                                          */
/* - applying gravity each loop                                                                     */
/* - 2 ball colliding with each other                                                               */
/* - moving the balls based on velocity                                                             */
/*--------------------------------------------------------------------------------------------------*/
void update(sf::Time elapsed, std::vector<Circle>& allCircles) {
    std::vector<Circle>::iterator itr;
    for (itr = allCircles.begin(); itr != allCircles.end(); itr++) {
        // Edge stuff
        edgeCollision(*itr);
        // Circle stuff
        circleCollision(itr, allCircles);
        // Apply Gravity
        applyGravity(allCircles);
        // Movement
        itr->SetPosition(itr->GetPosition() + itr->GetGravityVector() + HMM_Multiply(itr->GetVelocity(), elapsed.asSeconds()));
    }
}

/*-----------------------------------------------------------------------------------------*/
/* This will read from the data structures and draw the shapes we want on screen           */
/* https://www.sfml-dev.org/tutorials/2.5/graphics-shape.php                               */
/* The movement of shapes is done by doing a screen wipe and redrawing everything          */
/* (as opposed to using sfml "move" functions                                              */
/*-----------------------------------------------------------------------------------------*/
void draw(sf::RenderWindow& window,std::vector<Circle> allCircles) {
    window.clear();
    std::vector<Circle>::iterator itr;
    for (itr = allCircles.begin(); itr != allCircles.end(); itr++) {
        // move this to a utility function
        sf::CircleShape shape(itr->GetSize());
        shape.setFillColor(sf::Color(itr->GetColor().R, itr->GetColor().G, itr->GetColor().B));
        shape.setOrigin(shape.getRadius(), shape.getRadius());
        shape.setPosition(itr->GetPosition().X, itr->GetPosition().Y);
        window.draw(shape);
    }
    window.display();
}

int main() {	
    sf::ContextSettings settings;
    // just because I can. Not even sure it does anything with Circles
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle);
    // Seed here for random number in Circle. 
    srand(time(0));

    // Start the time
    sf::Clock clock;

    // make our vector that will hold the circles
    std::vector<Circle> allCircles;
    int i;
    // Fill in the array with Circles
    for (i = 0; i < numBalls; i++) {
        // I'm very rusty on my C++ here. I believe this is correct, but I'm worried this causes a memory leak
        // UPDATE: based on VS memory graphs, it does not
        Circle tempCircle = Circle(minBallRadiusPx, maxBallRadiusPx, windowHeight, windowWidth, minBallVelocityPx, maxBallVelocityPx);
        allCircles.push_back(tempCircle);
    }

    while (window.isOpen()) {
        input(window, allCircles);
        sf::Time elapsed = clock.getElapsedTime();
        // Added this to have a managed tic rate if I needed it
        // Came in handy to troubleshoot issues. Left it at 0 to run as fast as possible
        if (elapsed.asSeconds() > 0.00) {
            update(elapsed, allCircles);
            elapsed = clock.restart();
        }
        draw(window, allCircles);
    }

    return EXIT_SUCCESS;
}