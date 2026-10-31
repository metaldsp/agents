// SPDX-FileCopyrightText: 2026 Pier Luigi Fiorini
// SPDX-License-Identifier: LicenseRef-Proprietary

#include <PluginProcessor.h>

#include <gtest/gtest.h>

namespace AudioPlugin {

TEST(PluginProcessor, RestoresParameterState)
{
    juce::ScopedJuceInitialiser_GUI initialiseJuce;
    PluginProcessor source;
    auto *parameter = source.getState().getParameter(PluginParameters::parameterID);
    ASSERT_NE(parameter, nullptr);
    parameter->setValueNotifyingHost(parameter->convertTo0to1(6.0f));

    juce::MemoryBlock state;
    source.getStateInformation(state);

    PluginProcessor restored;
    restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

    const auto restoredValue = restored.getState()
                                   .getRawParameterValue(PluginParameters::parameterID)
                                   ->load(std::memory_order_relaxed);
    EXPECT_NEAR(restoredValue, 6.0f, 1.0e-4f);
}

TEST(PluginProcessor, PassesAudioThrough)
{
    juce::ScopedJuceInitialiser_GUI initialiseJuce;
    PluginProcessor processor;
    constexpr int numSamples = 16;
    processor.prepareToPlay(44100.0, numSamples);

    juce::AudioBuffer<float> buffer(2, numSamples);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            buffer.setSample(channel, sample, 0.25f);
    juce::MidiBuffer midi;
    processor.processBlock(buffer, midi);

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            EXPECT_NEAR(buffer.getSample(channel, sample), 0.25f, 1.0e-6f);
}

} // namespace AudioPlugin
