//======================================================================================
// Copyright (c) 2026 Pier Luigi Fiorini
// All rights reserved.
//======================================================================================

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace AudioPlugin {

class PluginParameters {
public:
  /*
   * Parameter IDs
   */

  static constexpr const char *parameterID = "parameter";

  /*
   * Parameter names
   */

  static constexpr const char *parameterName = "Parameter";

  /*
   * Default values
   */

  static constexpr float parameterDefault = 0.0f;
};

} // namespace AudioPlugin
