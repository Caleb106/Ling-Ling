#pragma once

namespace llpp::config
{
    inline const char* required_data = R"(
{
  "general": {
    "ark": {
      "game_root_directory": "F:/SteamLibrary/steamapps/common/ARK Survival Ascended"
    },
    "bot": {
      "assets": "C:/dev/ASAPP/ASAPP/assets",
      "itemdata": "C:/dev/ASAPP/ASAPP/source/itemdata.json",
      "tessdata": "C:/dev/LLPP/LLPP/tessdata"
    }
  },
  "discord": {
    "token": "",
    "authorization": {
      "command_channels": [],
      "authorized_roles": [],
      "authorized_users": []
    },
    "roles": {
      "helper_no_access": "",
      "helper_access": ""
    },
    "channels": {
      "info": "",
      "error": ""
    },
    "advanced": {
      "ephemeral_replies": false
    }
  },
  "bots": {
    "paste": {
      "prefix": "PASTE",
      "render_prefix": "RENDER",
      "num_stations": 16,
      "interval": 5,
      "load_for": 15,
      "ocr_amount": false,
      "allow_partial": false
    },
    "drops": {
    }
  }
}
)";
}