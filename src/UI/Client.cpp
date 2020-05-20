/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

// Local Project
#include "Client.hpp"
#if CSO_ASSIMP_ENABLE
#include "../core/AssimpLoader.hpp"
#include "../core/AssimpOgreUtility.hpp"
#endif

/* Ogre3D 1.12.2
 * License: MIT
 */
#include <OgreCamera.h>
#include <OgreException.h>
#include <OgreTextureManager.h>
#include <OgreViewport.h>
#if OGRE_STATIC
#include <Plugins/STBICodec/OgreSTBICodec.h>
#include <RenderSystems/GL3Plus/OgreGL3PlusPlugin.h>
#include <RenderSystems/GL3Plus/OgreGLRenderSystemCommon.h>
#endif

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

ClientWidget::ClientWidget()
    : m_wasLeftPressed(0), m_wasRightPressed(0), m_wasMiddlePressed(0) {
  std::cout << "CSO::ClientWidget::ClientWidget()" << std::endl;
  show_demo_window = true;
  show_another_window = false;
};
ClientWidget::~ClientWidget() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  // ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
};

bool ClientWidget::initGraphics(SDL_Window *window_) {
  window = window_;
  int windowWidthInt, windowHeightInt;
  SDL_GetWindowSize(window, &windowWidthInt, &windowHeightInt);
  windowWidth = static_cast<unsigned int>(windowWidthInt);
  windowHeight = static_cast<unsigned int>(windowHeightInt);

  std::cout << "ClientWidget::initGraphics" << std::endl;

  // Ogre Initialize
  initOgre();

  // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
  bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
  bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
  bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING)
  bool err = false;
  glbinding::initialize([](const char *name) {
    return (glbinding::ProcAddress)glfwGetProcAddress(name);
  });
#else
  bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader
                    // is likely to requires some form of initialization.
#endif
  if (err) {
#if HOCR_EDIT_MODULE_MAIN_WIDGET_INIT_GRAPHICS_DEBUG
    std::cout << "ClientWidget::initGraphics initialization ERROR" << std::endl;
#endif
    return false;
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;

  // Setup Dear ImGui style
  setStyle1();

  // Setup Platform/Renderer bindings
  SDL_GLContext gl_context = SDL_GL_GetCurrentContext();
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored
  // into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
  // ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'docs/FONTS.txt' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  boost::filesystem::path programPath = boost::dll::program_location();
  std::cout << "ClientWidget::initGraphics programPath: " << programPath
            << std::endl;
  boost::filesystem::path fontPath =
      programPath.parent_path() / "fonts/arial.ttf";
  std::cout << "fontPath: " << fontPath << std::endl;
  if (boost::filesystem::is_regular_file(fontPath)) {
    std::cout << "fontPath FOUND!: " << fontPath << std::endl;
    fontArialTitle =
        io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 22);
    fontArialText = io.Fonts->AddFontFromFileTTF(fontPath.string().c_str(), 12);
  }

  // Our state
  clear_color = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);

  return true;
}

bool ClientWidget::initOgre() {

#if CSO_OGRE_ENABLE
  // Ogre Initialize
  ogreRoot = std::make_shared<Ogre::Root>(Ogre::BLANKSTRING, Ogre::BLANKSTRING,
                                          Ogre::BLANKSTRING);

  /* Note: if no .png and .jpg codec is found, then a yellow image results
   * Ogre::STBIPlugin handles these codecs
   */
#if OGRE_STATIC_RENDER
  ogreRoot->installPlugin(new Ogre::STBIPlugin);
  ogreRoot->installPlugin(new Ogre::GL3PlusPlugin);
#else
  ogreRoot->loadPlugin("Codec_STBI");
  ogreRoot->loadPlugin("RenderSystem_GL3Plus");
#endif

  const Ogre::RenderSystemList &rsList = ogreRoot->getAvailableRenderers();
  Ogre::RenderSystem *rs = rsList[0];

  Ogre::StringVector renderOrder;
  // renderOrder.push_back("OpenGL");
  renderOrder.push_back("OpenGL 3+");

  for (auto itr = renderOrder.begin(); itr != renderOrder.end(); itr++) {
    for (auto it = rsList.begin(); it != rsList.end(); it++) {
      if ((*it)->getName().find(*itr) != Ogre::String::npos) {
        rs = *it;
        break;
      }
    }
    if (rs != nullptr)
      break;
  }

  if (rs == nullptr) {
#if HOCR_EDIT_MODULE_MAIN_WIDGET_INIT_GRAPHICS_DEBUG
    std::cout << "ClientWidget::initGraphics ERROR RenderSystem is not found"
              << std::endl;
#endif
    return false;
  }

  rs->setConfigOption("Full Screen", "No");
  ogreRoot->setRenderSystem(rs);
  ogreRoot->initialise(false);

  Ogre::NameValuePairList params;

  if (rs->getName().find("GL") <= rs->getName().size()) {
    params["currentGLContext"] = Ogre::String("false");
  }

  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(window, &wmInfo);

#if defined(__WIN32__)
  size_t winHandle = reinterpret_cast<size_t>(wmInfo.info.win.window);
  params["externalWindowHandle"] = Ogre::StringConverter::toString(winHandle);
#elif defined(__APPLE__)
  size_t winHandle = reinterpret_cast<size_t>(wmInfo.info.cocoa.window);
  params["externalWindowHandle"] = Ogre::StringConverter::toString(winHandle);
#elif defined(__linux__)
  size_t winHandle = reinterpret_cast<size_t>(wmInfo.info.x11.window);
  params["externalWindowHandle"] = Ogre::StringConverter::toString(winHandle);
#endif
  params["currentGLContext"] = Ogre::String("True");
  /* Needed to stop Ogre from doing selectPixelFormat
   * selectPixelFormat should be done by the window creation library
   */
  params["externalGLControl"] = Ogre::String("True");

#if defined(__APPLE__)
  params["macAPI"] = "cocoa";
  params["macAPICocoaUseNSView"] = "true";
#endif

  Ogre::String name = "RenderWindow #" + std::to_string(winHandle);
  try {
    renderWindow = ogreRoot->createRenderWindow(name, windowWidth, windowHeight,
                                                false, &params);
    /* How to access the render window without saving the pointer?
     * use Ogre::Root::getRenderTarget and cast the pointer to a RenderWindow*
     */
  } catch (Ogre::RenderingAPIException e) {
    std::cout << e.getDescription() << std::endl;
    std::cout << e.what() << std::endl;
    std::cout << "file: " << e.getFile() << ", function: " << e.getSource()
              << ", line: " << e.getLine() << std::endl;
    return false;
  }

  renderWindow->setVisible(true);

  // chooseSceneManager
  Ogre::SceneManager *m_sceneManager =
      ogreRoot->createSceneManager("DefaultSceneManager", "menu");

  boost::filesystem::path programPath = boost::dll::program_location();
#if CSO_OGRE_REAL_TIME_SHADER_ENABLE
  // Shortly after initialising Ogre::Root
  rtShader = std::make_shared<RTShader>();
  // initialize real time shader system
  if (!rtShader->initialize(m_sceneManager)) {
    std::cout << "ClientWidget::initOgre RTShader initialize FAIL" << std::endl;
    return false;
  }
#endif

  // set up resources
  setupResources();

  try {
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  } catch (std::exception e) {
    std::cout << e.what() << std::endl;
    // Some material or may be even an unrelated scripts crashed the engine.
    // Load the model plain simple, no materials. Only skeleton and mesh.
  }

#endif
  return true;
}

bool ClientWidget::render() {
  if (renderDelayFrameCount == 60) {
#if CSO_SCENE_MENU_ENABLE
    // Select scene
    menuScene = std::make_shared<SceneMenu>();
    menuScene->initialize(ogreRoot, renderWindow);
#endif
  }
  ImGuiIO &io = ImGui::GetIO();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame(window);
  ImGui::NewFrame();

  // Demonstrate creating a "main" fullscreen menu bar and populating it.
  // Note the difference between BeginMainMenuBar() and BeginMenuBar():
  // - BeginMenuBar() = menu-bar inside current window we Begin()-ed into (the
  // window needs the ImGuiWindowFlags_MenuBar flag)
  // - BeginMainMenuBar() = helper to create menu-bar-sized window at the top of
  // the main viewport + call BeginMenuBar() into it.
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ShowExampleMenuFile();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Undo", "CTRL+Z")) {
      }
      if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
      } // Disabled item
      ImGui::Separator();
      if (ImGui::MenuItem("Cut", "CTRL+X")) {
      }
      if (ImGui::MenuItem("Copy", "CTRL+C")) {
      }
      if (ImGui::MenuItem("Paste", "CTRL+V")) {
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  {
    static float f = 0.0f;
    static int counter = 0;

    // ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Debug");

    ImGui::Text("This is some useful text."); // Display some text (you can use
                                              // a format strings too)
    ImGui::Checkbox(
        "Demo Window",
        &show_demo_window); // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f,
                       1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
    ImGui::ColorEdit3(
        "clear color",
        (float *)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button("Button")) // Buttons return true when clicked (most
                                 // widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
  }

  // Rendering
  // Internally converts the UI to a draw list
  ImGui::Render();

  glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);

  SDL_GL_MakeCurrent(window, SDL_GL_GetCurrentContext());

#if CSO_OGRE_ENABLE
  // Render ogre first
  /* terminate called after throwing an instance of
   * 'Ogre::InvalidStateException'
   *
   * Ogre has two build in default materials. BaseWhite (the default used when
   * the requested material is missing or when a new material is created) and
   * BaseWhiteNoLighting (same as BaseWhite but with the light turned off).
   */
  /* BUG 50% of the time
   *
   * ClientWidget::render() Exception: InvalidStateException: RenderSystem does
   * not support FixedFunction, but technique of 'ch_attacker_11_01.png' has no
   * Vertex Shader. Use the RTSS or write custom shaders. in
   * SceneManager::_setPass at
   * C:/source/ogre-1.12.6/OgreMain/src/OgreSceneManager.cpp (line 895)
   *
   * NORMALLY:
   *
   * ShaderGeneratorTechniqueResolverListener::handleSchemeNotFound, schemeName:
   * ShaderGeneratorDefaultScheme, Material: ch_attacker_11_01.png TP1
   */
  for (Ogre::Technique *tech : Ogre::Material::Techniques()) {
    std::cout << "Ogre::Technique: " << tech->getName() << std::endl;
  }
  try {
    ogreRoot->renderOneFrame();
  } catch (Ogre::Exception e) {
    std::cout << "ClientWidget::render() Exception: " << e.what() << std::endl;
    return false;
  } catch (...) {
    std::cout << "ClientWidget::render() Exception!" << std::endl;
    return false;
  }
#endif

  // Render imgui after
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // increment frames for delays
  renderDelayFrameCount++;
  return true;
}

bool ClientWidget::handleEventFromSdl(SDL_Event *event) {
  while (SDL_PollEvent(event)) {
    ImGui_ImplSDL2_ProcessEvent(event);
    if (event->type == SDL_QUIT) {
      exit();
    } else if (event->type == SDL_WINDOWEVENT) {
      if (event->window.event == SDL_WINDOWEVENT_CLOSE &&
          event->window.windowID == SDL_GetWindowID(window)) {
        exit();
      } else {
        resizeEvent(event);
      }
    } else if (event->type == SDL_MOUSEBUTTONDOWN) {
      mousePressEvent(event);
    } else if (event->type == SDL_MOUSEBUTTONUP) {
      mouseReleaseEvent(event);
    } else if (event->type == SDL_MOUSEMOTION) {
      mouseMoveEvent(event);
    } else if (event->type == SDL_MOUSEWHEEL) {
      wheelEvent(event);
    }
    // std::cout << "event: " << event->type << std::endl;
  }
  return true;
}

void ClientWidget::mousePressEvent(SDL_Event *event) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
    return;
  SDL_GetMouseState(&m_mouseX, &m_mouseY);
  if (event->button.button == SDL_BUTTON_LEFT) {
    m_wasLeftPressed = true;
  } else if (event->button.button == SDL_BUTTON_RIGHT) {
    m_wasRightPressed = true;
  } else if (event->button.button == SDL_BUTTON_MIDDLE) {
    m_wasMiddlePressed = true;
  }
}

void ClientWidget::mouseReleaseEvent(SDL_Event *event) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
    return;
  if (event->button.button == SDL_BUTTON_LEFT) {
    m_wasLeftPressed = false;
  } else if (event->button.button == SDL_BUTTON_RIGHT) {
    m_wasRightPressed = false;
  } else if (event->button.button == SDL_BUTTON_MIDDLE) {
    m_wasMiddlePressed = false;
  }
}
void ClientWidget::mouseMoveEvent(SDL_Event *event) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
    return;
  const int oldX = m_mouseX;
  const int oldY = m_mouseY;
  SDL_GetMouseState(&m_mouseX, &m_mouseY);

#if CSO_SCENE_MENU_ENABLE
  if (m_wasLeftPressed) {
    menuScene->rotateCamera(m_mouseX - oldX, m_mouseY - oldY);
  }
  if (m_wasRightPressed) {
    // menuScene->slideCamera(m_mouseX - oldX, m_mouseY - oldY);
  }
  if (m_wasMiddlePressed) {
    menuScene->moveCamera(m_mouseX - oldX, m_mouseY - oldY);
  }
#endif
}

void ClientWidget::wheelEvent(SDL_Event *event) {
  ImGuiIO &io = ImGui::GetIO();
  if (io.WantCaptureMouse)
    return;
#if CSO_SCENE_MENU_ENABLE
  menuScene->zoomInCamera(event->wheel.y);
#endif
}

void ClientWidget::resizeEvent(SDL_Event *event) {
  if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
    resizeOgre();
  } else if (event->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
    resizeOgre();
  }
}

bool ClientWidget::resizeOgre() {
#if CSO_OGRE_ENABLE
  if (renderWindow) {
    renderWindow->windowMovedOrResized();
#if OGRE_VERSION_MAJOR < 2
    Ogre::Viewport *vp = renderWindow->getViewport(0);
    if (vp) {
      vp->getCamera()->setAspectRatio(Ogre::Real(vp->getActualWidth()) /
                                      Ogre::Real(vp->getActualHeight()));
    }
  }
#endif
#endif
  return true;
}

bool ClientWidget::exit() {
  *mainLoopFlag = true;
  return true;
}

void ClientWidget::setMainLoopFlag(bool *mainLoopFlag_) {
  mainLoopFlag = mainLoopFlag_;
}

void ClientWidget::setStyle1() {
  ImGuiStyle &style = ImGui::GetStyle();

  // frame rounding
  style.WindowRounding = 0.0f;
  style.ChildRounding = 0.0f;
  style.FrameRounding = 0.0f;
  style.GrabRounding = 0.0f;
  style.PopupRounding = 0.0f;
  style.ScrollbarRounding = 0.0f;

  // frame padding
  style.WindowPadding = ImVec2(0, 0);
  style.FramePadding = ImVec2(0, 0);
  style.DisplayWindowPadding = ImVec2(0, 0);
  style.DisplaySafeAreaPadding = ImVec2(0, 0);

  style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
  // style.Colors[ImGuiCol_TextHovered]           = ImVec4(1.00f, 1.00f,
  // 1.00f, 1.00f); style.Colors[ImGuiCol_TextActive]            =
  // ImVec4(1.00f, 1.00f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
  // style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f,
  // 0.00f, 0.00f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] =
      ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] =
      ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
  // style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f,
  // 0.86f, 0.99f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  // style.Colors[ImGuiCol_Column]                = ImVec4(0.39f, 0.39f,
  // 0.39f, 1.00f); style.Colors[ImGuiCol_ColumnHovered]         =
  // ImVec4(0.26f, 0.59f, 0.98f, 0.78f); style.Colors[ImGuiCol_ColumnActive] =
  // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  // style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f,
  // 0.59f, 0.50f); style.Colors[ImGuiCol_CloseButtonHovered]    =
  // ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
  // style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.98f, 0.39f,
  // 0.36f, 1.00f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
      ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  // style.Colors[ImGuiCol_TooltipBg]             = ImVec4(1.00f,
  // 1.00f, 1.00f, 0.94f);
  style.Colors[ImGuiCol_ModalWindowDarkening] =
      ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void ClientWidget::addDrawArrow() {
  ImDrawList *list = ImGui::GetWindowDrawList();
  ImVec2 lineP1 = {150.0f, 100.0f};
  ImVec2 lineP2 = {150.0f, 300.0f};
  static float thickness = 4.0f;
  static float arrowWidth = 12.0f;
  static float arrowHeight = 18.0f;
  static float lineWidth = 4.0f;

  list->PathLineTo({lineP1.x - thickness, lineP1.y + arrowHeight});  // P1
  list->PathLineTo({lineP1.x - arrowWidth, lineP1.y + arrowHeight}); // P2
  list->PathLineTo({lineP1.x, lineP1.y});                            // P3
  list->PathLineTo({lineP1.x + arrowWidth, lineP1.y + arrowHeight}); // P4
  list->PathLineTo({lineP1.x + thickness, lineP1.y + arrowHeight});  // P5
  list->PathLineTo({lineP2.x + thickness, lineP2.y});                // P6
  list->PathLineTo({lineP2.x - thickness, lineP2.y});                // P7
  list->PathFillConvex(IM_COL32(255, 255, 0, 255));

  list->PathLineTo({lineP1.x - thickness, lineP1.y + arrowHeight});  // P1
  list->PathLineTo({lineP1.x - arrowWidth, lineP1.y + arrowHeight}); // P2
  list->PathLineTo({lineP1.x, lineP1.y});                            // P3
  list->PathLineTo({lineP1.x + arrowWidth, lineP1.y + arrowHeight}); // P4
  list->PathLineTo({lineP1.x + thickness, lineP1.y + arrowHeight});  // P5
  list->PathLineTo({lineP2.x + thickness, lineP2.y});                // P6
  list->PathLineTo({lineP2.x - thickness, lineP2.y});                // P7
  list->PathStroke(IM_COL32(0, 0, 0, 255), true, lineWidth);
}

// Note that shortcuts are currently provided for display only (future version
// will add flags to BeginMenu to process shortcuts)
bool ClientWidget::ShowExampleMenuFile() {
  ImGui::MenuItem("(dummy menu)", NULL, false, false);
  if (ImGui::MenuItem("New")) {
  }
  if (ImGui::MenuItem("Open", "Ctrl+O")) {
  }
  if (ImGui::BeginMenu("Open Recent")) {
    ImGui::MenuItem("fish_hat.c");
    ImGui::MenuItem("fish_hat.inl");
    ImGui::MenuItem("fish_hat.h");
    if (ImGui::BeginMenu("More..")) {
      ImGui::MenuItem("Hello");
      ImGui::MenuItem("Sailor");
      if (ImGui::BeginMenu("Recurse..")) {
        ShowExampleMenuFile();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenu();
  }
  if (ImGui::MenuItem("Save", "Ctrl+S")) {
  }
  if (ImGui::MenuItem("Save As..")) {
  }
  ImGui::Separator();
  if (ImGui::BeginMenu("Options")) {
    static bool enabled = true;
    ImGui::MenuItem("Enabled", "", &enabled);
    ImGui::BeginChild("child", ImVec2(0, 60), true);
    for (int i = 0; i < 10; i++)
      ImGui::Text("Scrolling Text %d", i);
    ImGui::EndChild();
    static float f = 0.5f;
    static int n = 0;
    static bool b = true;
    ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
    ImGui::InputFloat("Input", &f, 0.1f);
    ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
    ImGui::Checkbox("Check", &b);
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Colors")) {
    float sz = ImGui::GetTextLineHeight();
    for (int i = 0; i < ImGuiCol_COUNT; i++) {
      const char *name = ImGui::GetStyleColorName((ImGuiCol)i);
      ImVec2 p = ImGui::GetCursorScreenPos();
      ImGui::GetWindowDrawList()->AddRectFilled(
          p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
      ImGui::Dummy(ImVec2(sz, sz));
      ImGui::SameLine();
      ImGui::MenuItem(name);
    }
    ImGui::EndMenu();
  }
  if (ImGui::BeginMenu("Disabled", false)) // Disabled
  {
    IM_ASSERT(0);
  }
  if (ImGui::MenuItem("Checked", NULL, true)) {
  }
  if (ImGui::MenuItem("Quit", "Alt+F4")) {
  }
  return true;
}

bool ClientWidget::setupResources() {
  boost::filesystem::path programPath = boost::dll::program_location();
  boost::filesystem::path resourceConfigPath =
      programPath.parent_path() / "resources.cfg";
  std::cout << "SceneMenu::setupResources resourceConfigPath="
            << resourceConfigPath << std::endl;

  Ogre::ConfigFile cf;
  cf.load(resourceConfigPath.string());

  Ogre::String secName, typeName;
  Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();

  // Ogre::ConfigFile::SettingsBySection;

  while (secIt.hasMoreElements()) {
    secName = secIt.peekNextKey();
    Ogre::ConfigFile::SettingsMultiMap *settings = secIt.getNext();
    Ogre::ConfigFile::SettingsMultiMap::iterator setIt;

    for (setIt = settings->begin(); setIt != settings->end(); ++setIt) {
      typeName = setIt->first;
      // make paths relative to the executable
      boost::filesystem::path resourceDirPath =
          programPath.parent_path() / setIt->second;
      std::cout << "SceneMenu::setupResources archName=" << resourceDirPath
                << std::endl;
      Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
          resourceDirPath.string(), typeName, secName);
    }
  }
  return true;
}

} // namespace CSO
