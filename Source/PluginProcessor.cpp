/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProtectYourEars.h"

//==============================================================================
DddelayyyAudioProcessor::DddelayyyAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),
params(apvts)
{
    lowCutFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);
    highCutFilter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
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
    
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = juce::uint32(samplesPerBlock);
    spec.numChannels = 2;
    
    //delayLine.prepare(spec);
    
    double numSamples = (Parameters::maxDelayTime / 1000.0) * sampleRate;
    int maxDelayInSamples = int(std::ceil(numSamples));
    //delayLine.setMaximumDelayInSamples(maxDelayInSamples);
    //delayLine.reset();
    
    delayLineL.setMaximumDelayInSamples(maxDelayInSamples);
    delayLineR.setMaximumDelayInSamples(maxDelayInSamples);
    delayLineL.reset();
    delayLineR.reset();
    
    feedbackL = 0.0f;
    feedbackR = 0.0f;
    
    lowCutFilter.prepare(spec);
    lowCutFilter.reset();
    
    highCutFilter.prepare(spec);
    highCutFilter.reset();
    
    tempo.reset();
    
    levelL.reset();
    levelR.reset();
    
    //delayInSamples = 0.0f;                                      // Crossfade
    //targetDelay = 0.0f;                                         // Crossfade
    //xfade = 0.0f;                                               // Crossfade
    //xfadeInc = static_cast<float>(1.0 / (0.5 * sampleRate));    // Crossfade 50 ms
    
    delayInSamples = 0.0f;
    targetDelay = 0.0f;
    fade = 1.0f;
    fadeTarget = 1.0f;
    coeff = 1.0f - std::exp(-1.0f / (0.05f * float(sampleRate)));
    wait = 0.0f;
    waitInc = 1.0f / (0.3f * float(sampleRate));
    
    //DBG(maxDelayInSamples);
}

void DddelayyyAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DddelayyyAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto mainIn = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();
    
    if (mainIn == mono && mainOut == mono) { return true; }
    if (mainIn == mono && mainOut == stereo) { return true; }
    if (mainIn == stereo && mainOut == stereo) { return true; }
    
    return false;
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
    
    tempo.update(getPlayHead());
    
    float syncedTime = float (tempo.getMillisecondsForNoteLength(params.delayNote));
    if( syncedTime > Parameters::maxDelayTime) {
        syncedTime = Parameters::maxDelayTime;
    }
    
    float sampleRate = float(getSampleRate());
    
    auto mainInput = getBusBuffer(buffer, true, 0);
    auto mainInputChannels = mainInput.getNumChannels();
    auto isMainInputStereo = mainInputChannels > 1;
    const float* inputDataL = mainInput.getReadPointer(0);
    const float* inputDataR = mainInput.getReadPointer(isMainInputStereo ? 1 : 0);
    
    auto mainOutput = getBusBuffer(buffer, false, 0);
    auto mainOutputChannels = mainOutput.getNumChannels();
    auto isMainOutputStereo = mainOutputChannels > 1;
    float* outputDataL = mainOutput.getWritePointer(0);
    float* outputDataR = mainOutput.getWritePointer(isMainOutputStereo ? 1 : 0);
    
    float maxL = 0.0f;
    float maxR = 0.0f;
    
    if (isMainOutputStereo){
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample){
            params.smoothen();
            
            // No crossfade
            //float delayTime = params.tempoSync ? syncedTime : params.delayTime;
            //delayInSamples = delayTime / 1000.0f * sampleRate;
            
            /*
            // Crossfade
            if (xfade == 0.0f) {
                float delayTime = params.tempoSync ? syncedTime : params.delayTime;
                targetDelay = delayTime / 1000.0f * sampleRate;
                
                if (delayInSamples == 0.0f) { // first time
                    delayInSamples = targetDelay;
                }
                
                else if (targetDelay != delayInSamples) {  // start crossfade
                    xfade = xfadeInc;
                }
            }
             */
            
            // Ducking
            float delayTime = params.tempoSync ? syncedTime : params.delayTime;
            float newTargetDelay = delayTime / 1000.0f * sampleRate;
            
            if (newTargetDelay != targetDelay) {
                targetDelay = newTargetDelay;
                
                if (delayInSamples == 0.0f) {
                    delayInSamples = targetDelay;
                }
                else {
                    wait = waitInc;
                    fadeTarget = 0.0f;
                }
            }
            
            lowCutFilter.setCutoffFrequency(params.lowCut);
            highCutFilter.setCutoffFrequency(params.highCut);
            
            float dryL = inputDataL[sample];
            float dryR = inputDataR[sample];
            
            float mono = (dryL + dryR) * 0.5f;
            
            delayLineL.write(mono * params.panL + feedbackL);
            delayLineR.write(mono * params.panR + feedbackR);
            
            float wetL = delayLineL.read(delayInSamples);
            float wetR = delayLineR.read(delayInSamples);
            
            /*
            // If crossfade is turned on
            if (xfade > 0.0f) { // crossfading?
                float newL = delayLineL.read(targetDelay);
                float newR = delayLineR.read(targetDelay);
                
                wetL = (1.0f - xfade) * wetL + xfade * newL;
                wetR = (1.0f - xfade) * wetR + xfade * newR;
                
                xfade += xfadeInc;
                if (xfade >= 1.0f) {
                    delayInSamples = targetDelay;
                    xfade = 0.0f;
                }
            }
             */
            
            // Ducking
            fade += (fadeTarget - fade) * coeff;
            
            wetL *= fade;
            wetR *= fade;
            
            if (wait > 0.0f) {
                wait += waitInc;
                if (wait >= 1.0f) {
                    delayInSamples = targetDelay;
                    wait = 0.0f;
                    fadeTarget = 1.0f;
                }
            }
            
            feedbackL = wetL * params.feedback;
            feedbackL = lowCutFilter.processSample(0, feedbackL);
            feedbackL = highCutFilter.processSample(0, feedbackL);
            
            feedbackR = wetR * params.feedback;
            feedbackR = lowCutFilter.processSample(1, feedbackR);
            feedbackR = highCutFilter.processSample(1, feedbackR);
            
            float mixL = dryL + wetL * params.mix;
            float mixR = dryR + wetR * params.mix;
            
            float outL = mixL * params.gain;
            float outR = mixR * params.gain;
            
            outputDataL[sample] = outL;
            outputDataR[sample] = outR;
            
            maxL = std::max(maxL, std::abs(outL));
            maxR = std::max(maxR, std::abs(outR));
        }
        
        levelL.updateIfGreater(maxL);
        levelR.updateIfGreater(maxR);
    }
    else {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample){
            params.smoothen();
            
            delayInSamples = (params.delayTime / 1000.0f) * sampleRate;
            
            float dry = inputDataL[sample];
            
            delayLineL.write(params.panL + feedbackL);
            
            float wet = delayLineL.read(delayInSamples);
            
            feedbackL = wet * params.feedback;
            
            float mix = dry + wet * params.mix;
            
            outputDataL[sample] = mix * params.gain;
        }
    }

    
#if JUCE_DEBUG
    protectYourEars(buffer);
#endif
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
