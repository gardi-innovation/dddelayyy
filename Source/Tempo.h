/*
  ==============================================================================

    Tempo.h
    Created: 20 Feb 2025 6:57:36pm
    Author:  Edmund í Garði

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Tempo
{
    public:
        void reset() noexcept;
        
        void update(const juce::AudioPlayHead* playhead) noexcept;
        
        double getMillisecondsForNoteLength(int index) const noexcept;
        
        double getTempo() const noexcept
        {
            return bpm;
        }
    
    private:
        double bpm = 120.0;
};
