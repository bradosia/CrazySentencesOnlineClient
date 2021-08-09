/*
 * @name BookFiler Recognize
 * @author Branden Lee
 * @version 1.00
 * @license GNU LGPL v3
 * @brief Text recognition viewer, editor, and analyzer.

 * Icons and images owned by their respective owners
 */

#ifndef APPLICATION_H
#define APPLICATION_H

// c++
#include <memory>

// MainWindow
#include "MainWindow.hpp"

class Application {
public:
  Application(int &argc, char **argv);
  ~Application();
  int exec();

private:
  std::unique_ptr<MainWindow> mainWindow;
};

#endif // APPLICATION_H
