/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

// Config
#include "../core/config.hpp"

// C
#include <stdio.h>
#include <unistd.h> // sleep

// C++
#include <iomanip>
#include <iostream>
#include <memory>
#include <unordered_map>

/* SDL 2.0.12
 * License: MIT
 */
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

/* Crazy Sentences Online 1.0
 * License: MIT
 */
#include "../Module.hpp"

int main(int argc, char *argv[]) {
  std::shared_ptr<CSO::Client> client = CSO::CsoClientModuleExport.newClient();
  // Screen dimension constants
  const int SCREEN_WIDTH = 640;
  const int SCREEN_HEIGHT = 480;

  // The window we'll be rendering to
  SDL_Window *window;
  SDL_SysWMinfo systemInfo;

  // The surface contained by the window
  SDL_Surface *screenSurface = NULL;

  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
  } else {

    // Create window
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    } else {
      // Get window surface
      screenSurface = SDL_GetWindowSurface(window);

      // Fill the surface white
      SDL_FillRect(screenSurface, NULL,
                   SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));

      // Update the surface
      SDL_UpdateWindowSurface(window);

      // Pass to client
      client->initGraphics(window);

      // Main loop flag
      bool quit = false;
      client->setMainLoopFlag(&quit);

      // Event handler
      SDL_Event e;

      // While application is running
      while (!quit) {
        client->handleEventFromSdl(&e);

        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
          // User requests quit
          std::cout << "type" << e.type << std::endl;
          if (e.type == SDL_QUIT) {
            quit = true;
          } else if (e.type == SDL_WINDOWEVENT &&
                     e.window.event == SDL_WINDOWEVENT_CLOSE) {
            // multi-window close
            quit = true;
          }
        }
        // Render
        client->render();
        // Update the surface
        SDL_UpdateWindowSurface(window);
      }
    }
  }

  // Destroy window
  SDL_DestroyWindow(window);

  // Quit SDL subsystems
  SDL_Quit();

  return 0;
}
