#include <PluginProcessor.h>

#include <gtest/gtest.h>

namespace AudioPlugin {

TEST(PluginProcessor, RestoresParameterState)
{
    juce::ScopedJuceInitialiser_GUI initialiseJuce;
    PluginProcessor source;
    auto *gain = source.getState().getParameter(PluginParameters::gainID);
    ASSERT_NE(gain, nullptr);
    gain->setValueNotifyingHost(gain->convertTo0to1(6.0f));

    juce::MemoryBlock state;
    source.getStateInformation(state);

    PluginProcessor restored;
    restored.setStateInformation(state.getData(), static_cast<int>(state.getSize()));

    const auto restoredGain = restored.getState()
                                  .getRawParameterValue(PluginParameters::gainID)
                                  ->load(std::memory_order_relaxed);
    EXPECT_NEAR(restoredGain, 6.0f, 1.0e-4f);
}

TEST(PluginProcessor, AppliesGainToAudio)
{
    juce::ScopedJuceInitialiser_GUI initialiseJuce;
    PluginProcessor processor;
    auto *gain = processor.getState().getParameter(PluginParameters::gainID);
    ASSERT_NE(gain, nullptr);
    gain->setValueNotifyingHost(gain->convertTo0to1(6.0f));

    constexpr int numSamples = 16;
    processor.prepareToPlay(44100.0, numSamples);

    juce::AudioBuffer<float> buffer(2, numSamples);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            buffer.setSample(channel, sample, 0.25f);
    juce::MidiBuffer midi;
    processor.processBlock(buffer, midi);

    const auto expected = 0.25f * juce::Decibels::decibelsToGain(6.0f);
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            EXPECT_NEAR(buffer.getSample(channel, sample), expected, 1.0e-5f);
}

TEST(PluginProcessor, DefaultsToUnityGain)
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
