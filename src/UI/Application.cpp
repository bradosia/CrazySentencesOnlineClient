/*
 * @name BookFiler Recognize
 * @author Branden Lee
 * @version 1.00
 * @license GNU LGPL v3
 * @brief Text recognition viewer, editor, and analyzer.

 * Icons and images owned by their respective owners
 */

#include "Application.hpp"

Application::Application(int &argc, char **argv) {
  // Declare the supported options.
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "compression", boost::program_options::value<int>(),
      "set compression level");

  boost::program_options::variables_map vm;
  boost::program_options::store(
      boost::program_options::parse_command_line(argc, argv, desc), vm);
  boost::program_options::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return;
  }

  if (vm.count("compression")) {
    std::cout << "Compression level was set to " << vm["compression"].as<int>()
              << ".\n";
  } else {
    std::cout << "Compression level was not set.\n";
  }

  mainWindow = std::make_unique<MainWindow>();
  mainWindow->show();
}

Application::~Application() {}

int exec() { return 0; }
