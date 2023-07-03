#pragma once
#include <climits>

struct Position
{
   int x;
   int y;
};

class PongGame
{
private:
   Position ballPosition;
   Position platformPosition;
   int screen_width;
   int screen_height;
   int ball_dx;
   int ball_dy;
   int platform_width;

public:
   PongGame(int width = 128, int height = 64) : screen_width(width), screen_height(height), platform_width(20)
   {
      ballPosition = {width / 2, height / 2};                  // start ball at center
      platformPosition = {0, height / 2 - platform_width / 2}; // start platform at center left
      ball_dx = 1;                                             // ball speed in x direction
      ball_dy = 1;                                             // ball speed in y direction
   }

   int handle(unsigned char key)
   {
      if (key == 11)
      {
         // move platform up
         if (platformPosition.y > 0)
            platformPosition.y--;
      }
      else if (key == 12)
      {
         // move platform down
         if (platformPosition.y < screen_height - platform_width)
            platformPosition.y++;
      }

      // move the ball
      ballPosition.x += ball_dx;
      ballPosition.y += ball_dy;

      // check for ball collisions with the wall and platform
      if (ballPosition.x <= 0)
      {
         if (ballPosition.y >= platformPosition.y && ballPosition.y <= platformPosition.y + platform_width)
         {
            ball_dx *= -1; // reverse x direction
         }
         else
         {
            // the ball fell off the platform, return INT_MAX as game over signal
            return INT_MAX;
         }
      }
      if (ballPosition.x >= screen_width)
         ball_dx *= -1; // reverse x direction

      if (ballPosition.y <= 0 || ballPosition.y >= screen_height)
         ball_dy *= -1; // reverse y direction

      // game continues
      return 0;
   }

   Position getBallPosition()
   {
      return ballPosition;
   }

   Position getPlatformPosition()
   {
      return platformPosition;
   }
};