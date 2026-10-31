# SPDX-FileCopyrightText: 2026 Pier Luigi Fiorini
# SPDX-License-Identifier: CC0-1.0

# Add assets.
# juce_add_binary_data(Assets
#     HEADER_NAME PluginAssets.h
#     NAMESPACE AudioPlugin::Assets
#     SOURCES
#         XXX
# )

# Required for Linux happiness:
# See https://forum.juce.com/t/loading-pytorch-model-using-binarydata/39997/2
# set_target_properties(Assets PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
