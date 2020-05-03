/*
 * @name Crazy Sentences Online
 * @author Branden Lee
 * @version 1.00
 * @license MIT
 * @brief A game for ESL students to learn and have fun!
 */

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/prettywriter.h>

// Local Project
#include "Module.hpp"

/*
 * CSO = Crazy Sentences Online
 */
namespace CSO {

ModuleExport::ModuleExport() {}
ModuleExport::~ModuleExport(){};

void ModuleExport::initialize() {
  std::cout << "Crazy Sentences Online Client Module" << std::endl;
}

void ModuleExport::registerSettings(
    std::shared_ptr<rapidjson::Document> moduleRequest,
    std::shared_ptr<cb_map_t> moduleCallbackMap) {
  moduleRequest->SetObject();
  moduleRequest->AddMember("FilesystemDatabase", "FilesystemDatabaseCB",
                           moduleRequest->GetAllocator());
  moduleCallbackMap->insert(
      {"FilesystemDatabaseCB",
       std::bind(&ModuleExport::setSettings, this, std::placeholders::_1)});
}

void ModuleExport::setSettings(std::shared_ptr<rapidjson::Value> data) {
#if MODULE_EXPORT_SET_SETTINGS_DEBUG
  rapidjson::StringBuffer buffer;
  rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
  data->Accept(writer);
  std::cout << "bookfiler::ModuleExport::setSettings:\n"
            << buffer.GetString() << std::endl;
#endif
}

std::shared_ptr<Client> ModuleExport::newClient(){
    return std::make_shared<ClientWidget>();
}

} // namespace CSO
