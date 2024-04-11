#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include "json.hpp"
#include "models.h"
#include "interfaces.h"
#include "Poco/Delegate.h"
#include "option_map_manager.h"

class VolFitter 
{
private:
	OptionMapManager &omm;


public:
  VolFitter(OptionMapManager &omm);
  ~VolFitter();
};
