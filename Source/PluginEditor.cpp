/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DddelayyyAudioProcessorEditor::DddelayyyAudioProcessorEditor (DddelayyyAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

DddelayyyAudioProcessorEditor::~DddelayyyAudioProcessorEditor()
{
}

//==============================================================================
void DddelayyyAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::blue);        //1
    g.setColour (juce::Colours::white);     //2
    g.setFont (40.0f);                      //3
    g.drawFittedText ("My First Plug-in!", getLocalBounds(), juce::Justification::centred, 1);
}

void DddelayyyAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
