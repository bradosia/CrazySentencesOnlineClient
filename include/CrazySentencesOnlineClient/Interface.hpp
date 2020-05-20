/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

#ifndef CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_H
#define CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_H

// c++17
#include <functional>
#include <memory>
#include <vector>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>

/* SDL 2.0.12
 * License: MIT
 */
#include <SDL2/SDL.h>

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

class Client {
public:
  virtual bool handleEventFromSdl(SDL_Event *) = 0;
  virtual void setMainLoopFlag(bool *) = 0;
  virtual bool initGraphics(SDL_Window *) = 0;
  virtual bool render() = 0;
  virtual bool exit() = 0;
};

using settings_cb_t = std::function<void(std::shared_ptr<rapidjson::Document>)>;
using cb_map_t = std::unordered_map<std::string, settings_cb_t>;
class ClientInterface {
public:
  virtual void initialize() = 0;
  /* registers settings to be found in the master settings file */
  virtual void registerSettings(std::shared_ptr<rapidjson::Document>,
                                std::shared_ptr<cb_map_t>) = 0;
  /* settings from the master settings file */
  virtual void setSettings(std::shared_ptr<rapidjson::Value> data) = 0;
  /* create a new client */
  virtual std::shared_ptr<Client> newClient() = 0;
};

} // namespace CSO

#endif // end CRAZY_SENTENCES_ONLINE_CLIENT_MODULE_H
