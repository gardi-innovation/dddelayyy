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
#include "LevelMeter.h"

//==============================================================================
/**
*/
class DddelayyyAudioProcessorEditor  : public juce::AudioProcessorEditor, private juce::AudioProcessorParameter::Listener
{
public:
    DddelayyyAudioProcessorEditor (DddelayyyAudioProcessor&);
    ~DddelayyyAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void parameterValueChanged(int, float) override;
    
    void parameterGestureChanged(int, bool) override { }
    
    void updateDelayKnobs(bool tempoSyncActive);
    
    DddelayyyAudioProcessor& audioProcessor;
    RotaryKnob gainKnob { "Gain", audioProcessor.apvts, gainParamID, true };
    RotaryKnob mixKnob { "Mix", audioProcessor.apvts, mixParamID };
    RotaryKnob delayTimeKnob { "Time", audioProcessor.apvts, delayTimeParamID };
    RotaryKnob feedbackKnob { "Feedback", audioProcessor.apvts, feedbackParamID, true };
    RotaryKnob stereoKnob { "Stereo", audioProcessor.apvts, stereoParamID, true };
    RotaryKnob lowCutKnob { "Low Cut", audioProcessor.apvts, lowCutParamID };
    RotaryKnob highCutKnob { "High Cut", audioProcessor.apvts, highCutParamID };
    RotaryKnob delayNoteKnob { "Note", audioProcessor.apvts, delayNoteParamID };
    
    juce::TextButton tempoSyncButton;
    
    juce::AudioProcessorValueTreeState::ButtonAttachment tempoSyncAttachment { audioProcessor.apvts, tempoSyncParamID.getParamID(), tempoSyncButton };
    
    juce::GroupComponent delayGroup, feedbackGroup, outputGroup;
    MainLookAndFeel mainLF;
    
    LevelMeter meter;
    
    juce::ImageButton bypassButton;
    
    juce::AudioProcessorValueTreeState::ButtonAttachment bypassAttachment { audioProcessor.apvts, bypassParamID.getParamID(), bypassButton };
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DddelayyyAudioProcessorEditor)
};
