/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Parameters.h"
#include "RotaryKnob.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*/
class DddelayyyAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    DddelayyyAudioProcessorEditor (DddelayyyAudioProcessor&);
    ~DddelayyyAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    DddelayyyAudioProcessor& audioProcessor;
    RotaryKnob gainKnob { "Gain", audioProcessor.apvts, gainParamID, true };
    RotaryKnob mixKnob { "Mix", audioProcessor.apvts, mixParamID };
    RotaryKnob delayTimeKnob { "Time", audioProcessor.apvts, delayTimeParamID };
    RotaryKnob feedbackKnob { "Feedback", audioProcessor.apvts, feedbackParamID, true };
    RotaryKnob stereoKnob { "Stereo", audioProcessor.apvts, stereoParamID, true };
    juce::GroupComponent delayGroup, feedbackGroup, outputGroup;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DddelayyyAudioProcessorEditor)
};
