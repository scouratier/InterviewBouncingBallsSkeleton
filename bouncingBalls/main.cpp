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

std::string windowTitle = "Hypixel - Bouncing Balls";
int windowWidth = 1000;
int windowHeight = 400;
int numBalls = 20;
int minBallRadiusPx = 20;
int maxBallRadiusPx = 30;
float minBallVelocityPx = 70;
float maxBallVelocityPx = 80;

hmm_vec2 gravity = HMM_Vec2(0, 0);

void input(sf::RenderWindow &window) {
    sf::Event event;

    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) window.close();

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code)
            {
                case sf::Keyboard::F1:
                    // update UI
                    // reset gravity
                    // This doesn't do anything useful, look into fixing that
                    gravity = HMM_Vec2(0, 0);
                    std::cout << "Resetting gravity" << std::endl;
                    break;
                case sf::Keyboard::Left:
                    // update UI
                    gravity.X = gravity.X - 0.001;
                    break;
                case sf::Keyboard::Right:
                    // update UI
                    gravity.X = gravity.X + 0.001;
                    break;
                case sf::Keyboard::Up:
                    // update UI
                    gravity.Y = gravity.Y - 0.001;
                    break;
                case sf::Keyboard::Down:
                    gravity.Y = gravity.Y + 0.001;
                    // update UI
                    break;
            default:
                break;
            }
        }
    }
}

hmm_vec2 bounceBall(hmm_vec2 oldDirection, hmm_vec2 reflectionPlane, hmm_vec2 reflectionNormal) {
    //return HMM_Normalize(oldDirection - 2 * (HMM_Multiply(oldDirection, normal)));
    //return oldDirection - HMM_Multiply(HMM_Multiply(normal, 2),HMM_Dot(normal,oldDirection));
    hmm_vec2 unitReflection = HMM_Normalize(reflectionPlane);
    hmm_vec2 unitReflectionNormal = HMM_Normalize(reflectionNormal);
    //return oldDirection - HMM_Multiply(HMM_Multiply(unitNormal,HMM_Dot(unitNormal,oldDirection)) ,2);
    return unitReflection * HMM_Dot(oldDirection, unitReflection) - unitReflectionNormal * HMM_Dot(oldDirection, unitReflectionNormal);
}

/*********************************************************************************************************************/
/* TODO: This has a lot of repetiion. Probable somewhere I can make a function a make it more readable               */
/*********************************************************************************************************************/
void edgeCollision(Circle& in) {
    hmm_vec2 currentPosition = in.GetPosition();
    // Edge collision detection
    // hitting left edge
    if (currentPosition.X - in.GetSize() < 0) {
        // bounce!
        hmm_vec2 edgeReflection = HMM_Vec2(0, 1);
        hmm_vec2 edgeNormal = HMM_Vec2(1, 0);
        in.SetMomentum(bounceBall(in.GetMomentum(), edgeReflection, edgeNormal));
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
        hmm_vec2 edgeReflection = HMM_Vec2(0, 1);
        hmm_vec2 edgeNormal = HMM_Vec2(-1, 0);
        in.SetMomentum(bounceBall(in.GetMomentum(), edgeReflection, edgeNormal));
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
        hmm_vec2 edgeReflection = HMM_Vec2(1, 0);
        hmm_vec2 edgeNormal = HMM_Vec2(0, 1);
        in.SetMomentum(bounceBall(in.GetMomentum(), edgeReflection, edgeNormal));
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
        hmm_vec2 edgeReflection = HMM_Vec2(1, 0);
        hmm_vec2 edgeNormal = HMM_Vec2(0, -1);
        in.SetMomentum(bounceBall(in.GetMomentum(), edgeReflection, edgeNormal));
        in.ResetGravitySpeed();
        // Push away from the wall
        // The overlap is the total radius minus the distance to the wall
        float overlap = in.GetSize() - (windowHeight - currentPosition.Y);
        // set new position
        in.SetPosition(in.GetPosition() + edgeNormal * overlap);
    }
}

hmm_vec2 makeOrthogonalVector(hmm_vec2 in) {
    hmm_vec2 out;
    out.X = in.Y;
    out.Y = -in.X;
    return out;
}

/*void fixOverlap(Circle& in, hmm_vec2 wall) {
    // The ball is overlapping with a wall. Push it away
    // Distance of center from the wall
    hmm_vec2 distance = in.GetSize() - 
    float overlap = (in.GetSize() + itr->GetSize()) - HMM_Length(vBetweenCircles);
    // set new positions
    currentCircle->SetPosition(currentCircle->GetPosition() + HMM_Multiply(HMM_Multiply(HMM_Normalize(vBetweenCircles), 1), overlap / 2));
    itr->SetPosition(itr->GetPosition() + HMM_Multiply(HMM_Multiply(HMM_Normalize(vBetweenCircles), -1), overlap / 2));
}*/

void circleCollision(std::vector<Circle>::iterator currentCircle, std::vector<Circle>& testCircles) {
    // Collision happens when |C1.center - C2.center| < C1.radius+C2.radius
    std::vector<Circle>::iterator itr;
    for (itr = currentCircle+1; itr != testCircles.end(); itr++) {
        // Collision happens when |C1.center - C2.center| < C1.radius+C2.radius
        hmm_vec2 vBetweenCircles = HMM_Subtract(currentCircle->GetPosition(), itr->GetPosition());
        if (HMM_Length(vBetweenCircles) < currentCircle->GetSize() + itr->GetSize()) {


            // 
            hmm_vec2 unitBounceNormal = HMM_Normalize(vBetweenCircles);
            hmm_vec2 unitReflectionPlane = HMM_Normalize(makeOrthogonalVector(vBetweenCircles));
            // Set the new direction for the balls
            currentCircle->SetMomentum(bounceBall(currentCircle->GetMomentum(), HMM_Multiply(unitReflectionPlane, -1), HMM_Multiply(unitBounceNormal, 1)));
            itr->SetMomentum(bounceBall(itr->GetMomentum(), HMM_Multiply(unitReflectionPlane, 1), HMM_Multiply(unitBounceNormal, 1)));

            // The balls are now overlapping and should be moved apart
            // If we don't do this, the 2 balls will still be in a collision at the next test
            // and it makes all sorts of weird
            float overlap = (currentCircle->GetSize() + itr->GetSize()) - HMM_Length(vBetweenCircles);
            // set new positions
            currentCircle->SetPosition(currentCircle->GetPosition() + HMM_Multiply(HMM_Multiply(HMM_Normalize(vBetweenCircles),1), overlap / 2));
            itr->SetPosition(itr->GetPosition() + HMM_Multiply(HMM_Multiply(HMM_Normalize(vBetweenCircles), -1), overlap / 2));
        }
    }
}

void applyGravity(std::vector<Circle>& allCircles) {
    // Loop all circles
    std::vector<Circle>::iterator itr;
    for (itr = allCircles.begin(); itr != allCircles.end(); itr++) {
        // Add gravity to the current direction vector
        itr->ApplyGravity(gravity);
    }
}

void resetGravity(std::vector<Circle>& allCircles) {
    std::vector<Circle>::iterator itr;
    for (itr = allCircles.begin(); itr != allCircles.end(); itr++) {
        // Add gravity to the current direction vector
        itr->SetGravityVector(gravity);
    }
}

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
        itr->SetPosition(itr->GetPosition() + itr->GetGravityVector() + HMM_Multiply(itr->GetMomentum(), elapsed.asSeconds()));
    }
}

void draw(sf::RenderWindow& window,std::vector<Circle> allCircles) {
    window.clear();

    //https://www.sfml-dev.org/tutorials/2.5/graphics-shape.php
    // How to draw a green circle
    //sf::CircleShape shape(50.f);
    //shape.setFillColor(sf::Color(100, 250, 50));
    //window.draw(shape);
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
    for (i = 0; i < numBalls; i++) {
        // I'm very rusty on my C++ here. I believe this is correct, but I'm worried this causes a memory leak
        Circle tempCircle = Circle(minBallRadiusPx, maxBallRadiusPx, windowHeight, windowWidth, minBallVelocityPx, maxBallVelocityPx);
        allCircles.push_back(tempCircle);
    }

    while (window.isOpen()) {
        input(window);
        sf::Time elapsed = clock.getElapsedTime();
        if (elapsed.asSeconds() > 0.0016) {
            update(elapsed, allCircles);
            elapsed = clock.restart();
        }
        draw(window, allCircles);
    }

    return EXIT_SUCCESS;
}