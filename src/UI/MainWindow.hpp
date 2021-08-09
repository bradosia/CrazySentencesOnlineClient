/*
 * @name BookFiler Recognize
 * @author Branden Lee
 * @version 1.00
 * @license GNU LGPL v3
 * @brief Text recognition viewer, editor, and analyzer.

 * Icons and images owned by their respective owners
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

// Config
#include "../core/config.hpp"

// C++
#include <memory>

/* boost 1.76.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/program_options.hpp>

// bradosia libraries 1.0
#include <ModuleManager/ModuleManager.hpp>
#include <SettingsManager/SettingsManager.hpp>

// Modules
#include <BookFiler-Module-File-Tree-Pane/Interface.hpp>
#include <BookFiler-Module-Filesystem-Database/Interface.hpp>

// Local Project
//#include "RenderWidget.hpp"

class MainWindow {
public:
  MainWindow(boost::program_options::variables_map);
  ~MainWindow();
  void loadModules();
  int FSDB_ModuleLoaded(
      std::shared_ptr<bookfiler::FileSystemDatabaseInterface>);
  int fileTreePaneModuleLoaded(
      std::shared_ptr<bookfiler::FileTreePaneInterface>);
  int allModulesLoaded();

private:
  unsigned int modulesLoadedNum;
  unsigned int modulesLoadedTotalNum;
  std::shared_ptr<bookfiler::FileSystemDatabaseInterface> FSDB_Module;
  std::shared_ptr<bookfiler::FileTreePaneInterface> fileTreePaneModule;
  std::shared_ptr<bradosia::ModuleManager> moduleManagerPtr;
  std::shared_ptr<bradosia::SettingsManager> settingsManagerPtr;
  std::shared_ptr<bookfiler::FileTreePaneWidget> fileTreePaneWidgetList,
      fileTreePaneWidgetMain;
};
#endif // MAIN_WINDOW_H
