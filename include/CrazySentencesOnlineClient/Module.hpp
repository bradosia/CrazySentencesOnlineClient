/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

#ifndef CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_EXPORT_H
#define CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_EXPORT_H

// config
#include "core/config.hpp"

// C++17
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/reader.h> // rapidjson::ParseResult
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

// Local Project
#include "Interface.hpp"
#include "UI/Client.hpp"

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

using settings_cb_t = std::function<void(std::shared_ptr<rapidjson::Document>)>;
using cb_map_t = std::unordered_map<std::string, settings_cb_t>;

class ModuleExport : public ClientInterface {
private:
public:
  ModuleExport();
  ~ModuleExport();

  void initialize();
  /* registers settings to be found in the master settings file */
  void registerSettings(std::shared_ptr<rapidjson::Document>,
                        std::shared_ptr<cb_map_t>);
  /* settings from the master settings file */
  void setSettings(std::shared_ptr<rapidjson::Value> data);
  /* create a new client */
  std::shared_ptr<Client> newClient();
};

} // namespace CSO

#endif
// end CRAZY_SENTENCES_ONLINE_CLIENT_MODULE
