/*
  ==============================================================================

    Parameters.h
    Created: 8 Jan 2025 8:24:09pm
    Author:  Edmund í Garði

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
const juce::ParameterID gainParamID { "gain", 1};
const juce::ParameterID delayTimeParamID { "delayTime", 1};
const juce::ParameterID mixParamID { "mix", 1 };

class Parameters
{
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);
    
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
    void update() noexcept;
    void prepareToPlay(double sampleRate) noexcept;
    void reset() noexcept;
    void smoothen() noexcept;
    
    float gain = 0.0f;
    
    static constexpr float minDelayTime = 5.0f;
    static constexpr float maxDelayTime = 5000.0f;
    
    float delayTime = 0.0f;
    
    float mix = 1.0f;
    
private:
    juce::AudioParameterFloat* gainParam;
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::AudioParameterFloat* delayTimeParam;
    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;
    float targetDelayTime = 0.0f;
    float coeff = 0.0f; // one-pole smooting
};

