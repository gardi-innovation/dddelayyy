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
const juce::ParameterID feedbackParamID { "feedback", 1};
const juce::ParameterID stereoParamID { "stereo", 1};
const juce::ParameterID highCutParamID { "lowCut", 1};
const juce::ParameterID lowCutParamID { "highCut", 1};
const juce::ParameterID tempoSyncParamID { "tempoSync", 1 };
const juce::ParameterID delayNoteParamID { "delayNote", 1 };
const juce::ParameterID bypassParamID { "bypass", 1 };

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
    
    float feedback = 0.0f;
    
    float panL = 0.0f;
    float panR = 1.0f;
    
    float lowCut = 20.0f;
    float highCut = 20000.0f;
    
    int delayNote = 0;
    bool tempoSync = false;
    juce::AudioParameterBool* tempoSyncParam;
    
    bool bypassed = false;
    juce::AudioParameterBool* bypassParam;
    
private:
    juce::AudioParameterFloat* gainParam;
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::AudioParameterFloat* delayTimeParam;
    juce::AudioParameterFloat* mixParam;
    juce::LinearSmoothedValue<float> mixSmoother;
    juce::AudioParameterFloat* feedbackParam;
    juce::LinearSmoothedValue<float> feedbackSmoother;
    juce::AudioParameterFloat* stereoParam;
    juce::LinearSmoothedValue<float> stereoSmoother;
    juce::AudioParameterFloat* lowCutParam;
    juce::LinearSmoothedValue<float> lowCutSmoother;
    juce::AudioParameterFloat* highCutParam;
    juce::LinearSmoothedValue<float> highCutSmoother;
    juce::AudioParameterChoice* delayNoteParam;
    
    
    float targetDelayTime = 0.0f;
    float coeff = 0.0f; // one-pole smooting
};

