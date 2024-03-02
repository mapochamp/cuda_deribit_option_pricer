#pragma once
#include <iostream>
#include "json.hpp"
#include "Poco/DateTime.h"
#include "Poco/DateTimeParser.h"
#include "Poco/DateTimeFormat.h"

using json = nlohmann::json;

namespace Models
{
  // === ENUMS ===
  enum class ConnectivityStatus
  {
    connected,
    disconnected
  };

  // === FUNCTIONS ===
} // namespace Models
