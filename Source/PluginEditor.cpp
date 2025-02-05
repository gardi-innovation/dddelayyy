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
    delayGroup.setText("Delay");
    delayGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    delayGroup.addAndMakeVisible(delayTimeKnob);
    addAndMakeVisible(delayGroup);
    
    feedbackGroup.setText("Feedback");
    feedbackGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    feedbackGroup.addAndMakeVisible(feedbackKnob);
    feedbackGroup.addAndMakeVisible(stereoKnob);
    addAndMakeVisible(feedbackGroup);
    
    outputGroup.setText("Output");
    outputGroup.setTextLabelPosition(juce::Justification::horizontallyCentred);
    outputGroup.addAndMakeVisible(gainKnob);
    outputGroup.addAndMakeVisible(mixKnob);
    addAndMakeVisible(outputGroup);
    
    //gainKnob.slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::green); // This overrides the default color of this particular knob.
    
    setSize (500, 330);
}

DddelayyyAudioProcessorEditor::~DddelayyyAudioProcessorEditor()
{
}

//==============================================================================
void DddelayyyAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (Colors::background);
}

void DddelayyyAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    int y = 10;
    int height = bounds.getHeight() - 20;
    
    // Position the groups
    delayGroup.setBounds(10, y, 110, height);
    
    outputGroup.setBounds(bounds.getWidth() - 160, y, 150, height);
    
    feedbackGroup.setBounds(delayGroup.getRight() + 10, y, outputGroup.getX() - delayGroup.getRight() - 20, height);
    
    // Position the knobs inside the groups
    delayTimeKnob.setTopLeftPosition(20, 20);
    mixKnob.setTopLeftPosition(20, 20);
    gainKnob.setTopLeftPosition(mixKnob.getX(), mixKnob.getBottom() + 10);
    feedbackKnob.setTopLeftPosition(20, 20);
    stereoKnob.setTopLeftPosition(feedbackKnob.getRight() + 20, 20);
}
