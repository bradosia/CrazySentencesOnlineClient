/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

// Local Project
#include "Client.hpp"

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

ClientWidget::ClientWidget() {
  std::cout << "CSO::ClientWidget::ClientWidget()" << std::endl;
};
ClientWidget::~ClientWidget() {
  // Cleanup
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
};

bool ClientWidget::initGraphics(SDL_Window *window_) {
  window = window_;
  std::cout << "ClientWidget::initGraphics" << std::endl;
  // Ogre Initialize
  root =
      new Ogre::Root(Ogre::BLANKSTRING, Ogre::BLANKSTRING, Ogre::BLANKSTRING);
#if defined(OGRE_STATIC)
  root->installPlugin(new Ogre::GLPlugin);
  // root->installPlugin(new Ogre::GL3PlusPlugin);
#else
  root->loadPlugin("RenderSystem_GL");
  root->loadPlugin("Codec_STBI");
  // root->loadPlugin("RenderSystem_GL3Plus");
#endif

  const Ogre::RenderSystemList &rsList = root->getAvailableRenderers();
  Ogre::RenderSystem *rs = rsList[0];

  Ogre::StringVector renderOrder;
  renderOrder.push_back("OpenGL");
  // renderOrder.push_back("OpenGL 3+");

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
  root->setRenderSystem(rs);
  root->initialise(false);

  Ogre::NameValuePairList params;

  if (rs->getName().find("GL") <= rs->getName().size()) {
    params["currentGLContext"] = Ogre::String("false");
  }

  Ogre::String handle =
      Ogre::StringConverter::toString(size_t(widgetData->winId));
  unsigned int w = static_cast<unsigned int>(widgetData->displaySizeX);
  unsigned int h = static_cast<unsigned int>(widgetData->displaySizeY);

#if defined(__APPLE__) || defined(__WIN32__)
  params["externalWindowHandle"] =
      Ogre::StringConverter::toString((size_t)(widgetData->winId));
#else
  /* For __linux__ is this enough?
   */
  params["externalWindowHandle"] =
      Ogre::StringConverter::toString((size_t)(widgetData->winId));
#endif

#if defined(__APPLE__)
  params["macAPI"] = "cocoa";
  params["macAPICocoaUseNSView"] = "true";
#endif

  Ogre::String name = "RenderWindow #" + handle;
  // required for texture manager
  root->createRenderWindow(name, w, h, false, &params);

  Ogre::ResourceGroupManager::getSingleton().createResourceGroup("g1");
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
  ImGui_ImplBookfilerWidget_InitForOpenGL(widgetData);
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
  fontArialTitle = io.Fonts->AddFontFromFileTTF("fonts/arial.ttf", 22);
  fontArialText = io.Fonts->AddFontFromFileTTF("fonts/arial.ttf", 12);

  // Our state
  clear_color = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
  return true;
}

bool ClientWidget::render() {
  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplBookfilerWidget_NewFrame(widgetData);
  ImGui::NewFrame();

  ImGuiIO &io = ImGui::GetIO();
#if HOCR_EDIT_MODULE_MAIN_WIDGET_RENDER_1_DEBUG
  std::cout << "w=" << widgetData->displaySizeX
            << " h=" << widgetData->displaySizeY << std::endl;
#endif

  io.FontGlobalScale = 1;
  ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
  ImGui::SetNextWindowSize(
      ImVec2(widgetData->displaySizeX, widgetData->displaySizeY), 0);
  ImGui::Begin("Hello, world!", new bool(false),
               ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                   ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground);
  ImDrawList *list = ImGui::GetWindowDrawList();

  /* Draw the image
   */
  float scaleFactor = 1;
  if (glID) {
    if (image_pixmap->width > widgetData->displaySizeX) {
      scaleFactor = (float)widgetData->displaySizeX / image_pixmap->width;
    }
    ImGui::Image((void *)(intptr_t)glID,
                 ImVec2(image_pixmap->width * scaleFactor,
                        image_pixmap->height * scaleFactor));
  }

  if (!glID) {
    ImGui::PushFont(fontArialTitle);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1));
    ImGui::Text("hOCR Edit Module\nAdd an image and double click the path in "
                "the list on the left to begin.");
    ImGui::PopStyleColor();
    ImGui::PopFont();
  }

  io.FontGlobalScale = scaleFactor * 3;
  // Top left point of scroll window
  ImVec2 clip_rect = list->GetClipRectMin();
  // Current scroll position
  float scroll_x = ImGui::GetScrollX();
  float scroll_y = ImGui::GetScrollY();
  /* overlay text */
  if (overlayWordList) {
    ImGui::PushFont(fontArialText);
    for (auto wordPtr : *overlayWordList) {
      list->AddRect(ImVec2(wordPtr->x0 * scaleFactor + clip_rect.x - scroll_x,
                           wordPtr->y0 * scaleFactor + clip_rect.y - scroll_y),
                    ImVec2(wordPtr->x1 * scaleFactor + clip_rect.x - scroll_x,
                           wordPtr->y1 * scaleFactor + clip_rect.y - scroll_y),
                    IM_COL32(0, 0, 0, 120));
      list->AddText(ImVec2(wordPtr->x0 * scaleFactor + clip_rect.x - scroll_x,
                           wordPtr->y0 * scaleFactor + clip_rect.y - scroll_y),
                    IM_COL32(0, 0, 0, 255), wordPtr->value.c_str());
    }
    ImGui::PopFont();
  }
  io.FontGlobalScale = 1;

  ImGui::End();

  // Rendering
  // Internally converts the UI to a draw list
  ImGui::Render();

  glViewport(0, 0, widgetData->displaySizeX, widgetData->displaySizeY);
  glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  return true;
}

bool ClientWidget::handleEventFromSdl(SDL_Event *) { return true; }

void setMainLoopFlag(bool *mainLoopFlag_) { mainLoopFlag = mainLoopFlag_; }

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
  // style.Colors[ImGuiCol_ChildWindowBg]         = ImVec4(0.00f, 0.00f, 0.00f,
  // 0.00f);
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
  // style.Colors[ImGuiCol_ComboBg]               = ImVec4(0.86f, 0.86f, 0.86f,
  // 0.99f);
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
  // 0.39f, 1.00f); style.Colors[ImGuiCol_ColumnHovered]         = ImVec4(0.26f,
  // 0.59f, 0.98f, 0.78f); style.Colors[ImGuiCol_ColumnActive]          =
  // ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
  // style.Colors[ImGuiCol_CloseButton]           = ImVec4(0.59f, 0.59f, 0.59f,
  // 0.50f); style.Colors[ImGuiCol_CloseButtonHovered]    = ImVec4(0.98f, 0.39f,
  // 0.36f, 1.00f); style.Colors[ImGuiCol_CloseButtonActive]     = ImVec4(0.98f,
  // 0.39f, 0.36f, 1.00f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] =
      ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
  // style.Colors[ImGuiCol_TooltipBg]             = ImVec4(1.00f, 1.00f, 1.00f,
  // 0.94f);
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

} // namespace CSO
