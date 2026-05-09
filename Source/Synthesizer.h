#pragma once

#include "Oscillator.h"

#include <JuceHeader.h>


class Synthesizer {
  public:
    void prepareToPlay(double sampleRate);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages);

  private:
    void initializeOscillators();
    std::vector<float> generateSineWaveTable() const;
    void handleMidiEvent(const juce::MidiMessage& midiEvent);
    float midiNoteNumberToFrequency(int midiNoteNumber);
    void render(juce::AudioBuffer<float>& buffer, int startSample, int endSample);

    double sampleRate_;
    std::vector<Oscillator> oscillators_;
};
