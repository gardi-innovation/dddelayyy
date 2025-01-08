/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DddelayyyAudioProcessor::DddelayyyAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),
params(apvts)
{
    // do nothing
}

DddelayyyAudioProcessor::~DddelayyyAudioProcessor()
{
}

//==============================================================================
const juce::String DddelayyyAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DddelayyyAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DddelayyyAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DddelayyyAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DddelayyyAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DddelayyyAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DddelayyyAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DddelayyyAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DddelayyyAudioProcessor::getProgramName (int index)
{
    return {};
}

void DddelayyyAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DddelayyyAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    params.prepareToPlay(sampleRate);
    params.reset();
}

void DddelayyyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DddelayyyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

void DddelayyyAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, [[maybe_unused]]juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    params.update();
    
    float* channelDataL = buffer.getWritePointer(0);
    float* channelDataR = buffer.getWritePointer(1);
    
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample){
        params.smoothen();
        
        channelDataL[sample] *= params.gain;
        channelDataR[sample] *= params.gain;
    }
}

//==============================================================================
bool DddelayyyAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DddelayyyAudioProcessor::createEditor()
{
    return new DddelayyyAudioProcessorEditor (*this);
}

//==============================================================================
void DddelayyyAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    //DBG(apvts.copyState().toXmlString());
    
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void DddelayyyAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if( xml.get() != nullptr && xml->hasTagName(apvts.state.getType())){
        apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DddelayyyAudioProcessor();
}
