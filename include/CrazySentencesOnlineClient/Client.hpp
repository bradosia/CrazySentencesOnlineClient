/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

#ifndef CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_CLIENT_H
#define CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_CLIENT_H

// Config
#include "../core/config.hpp"

// C++
#include <iostream>
#include <memory>

/* Dear ImGui 1.75
 * License: MIT
 */
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_sdl.h>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include "boost/dll/runtime_symbol_info.hpp"
#include "boost/filesystem.hpp"

/* SDL 2.0.12
 * License: MIT
 */
#include <SDL2/SDL_syswm.h>

/* Ogre3D 1.12.2
 * License: MIT
 */
#include <GL/glew.h>
#if CSO_OGRE_ENABLE
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#endif

// Local Project
#include "../Interface.hpp"
#if CSO_SCENE_MENU_ENABLE
#include "SceneMenu.hpp"
#endif

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

class ClientWidget : public Client {

public:
  ClientWidget();
  ~ClientWidget();
  // Interface
  bool handleEventFromSdl(SDL_Event *);
  void setMainLoopFlag(bool *);
  bool initGraphics(SDL_Window *);
  bool render();
  bool exit();

  // UI
  bool ShowExampleMenuFile();

  // Custom
  void setStyle1();
  void addDrawArrow();
  void mousePressEvent(SDL_Event *);
  void mouseReleaseEvent(SDL_Event *);
  void mouseMoveEvent(SDL_Event *);
  void wheelEvent(SDL_Event *);
  void resizeEvent(SDL_Event *);

  bool initOgre();
  bool resizeOgre();

  ImFont *fontArialTitle;
  ImFont *fontArialText;

protected:
  void initializeGL();

private:
  /* Does not take ownership of the window
   * No guarantee window pointer is valid
   */
  SDL_Window *window;
  unsigned int windowWidth, windowHeight;
  bool *mainLoopFlag;
  bool show_demo_window, show_another_window, m_wasLeftPressed,
      m_wasRightPressed, m_wasMiddlePressed;
  int m_mouseX, m_mouseY;
// OGRE
#if CSO_OGRE_ENABLE
  std::shared_ptr<Ogre::Root> ogreRoot;
  Ogre::RenderWindow* renderWindow;
#endif
// SCENE
#if CSO_SCENE_MENU_ENABLE
  std::shared_ptr<SceneMenu> menuScene;
#endif
  // Our state
  ImVec4 clear_color;
  // Decide GL+GLSL versions
#if __APPLE__
  // GL 3.2 + GLSL 150
  const char *glsl_version = "#version 150";
#else
  // GL 3.0 + GLSL 130
  const char *glsl_version = "#version 130";
  unsigned int textureName = 1;
  unsigned int glID = 0;
#endif
};

} // namespace CSO

#endif
// end CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_CLIENT_H
