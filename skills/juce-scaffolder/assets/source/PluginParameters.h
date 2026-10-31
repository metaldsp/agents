// SPDX-FileCopyrightText: 2026 Pier Luigi Fiorini
// SPDX-License-Identifier: LicenseRef-Proprietary

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace AudioPlugin {

class PluginParameters
{
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
