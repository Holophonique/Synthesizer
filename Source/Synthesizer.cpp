#include "Synthesizer.h"

#include <numbers>


void
Synthesizer::prepareToPlay(double sampleRate) {
    sampleRate_ = sampleRate;

    initializeOscillators();
}

void
Synthesizer::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    int currentSample = 0;

    for(const auto midiMessage : midiMessages) {
        const juce::MidiMessage midiEvent = midiMessage.getMessage();
        const int midiEventSample = static_cast<int>(midiEvent.getTimeStamp());

        render(buffer, currentSample, midiEventSample);
        handleMidiEvent(midiEvent);

        currentSample = midiEventSample;
    }

    render(buffer, currentSample, buffer.getNumSamples());
}

std::vector<float>
Synthesizer::generateSineWaveTable() const {
    constexpr int WAVETABLE_LENGTH = 64;

    std::vector<float> sineWaveTable(WAVETABLE_LENGTH);

    for (int i = 0; i < WAVETABLE_LENGTH; ++i) {
        sineWaveTable[i] = std::sinf(2 * std::numbers::pi * i / WAVETABLE_LENGTH);
    }

    return sineWaveTable;
}

void
Synthesizer::initializeOscillators() {
    constexpr int OSCILLATORS_COUNT = 128;  // one oscillator per MIDI note (0-127)
    const std::vector<float> waveTable = generateSineWaveTable();

    oscillators_.clear();
    for (int i = 0; i < OSCILLATORS_COUNT; ++i) {
        oscillators_.emplace_back(waveTable, sampleRate_);
    }
}

void
Synthesizer::render(juce::AudioBuffer<float>& buffer, int startSample, int endSample) {
    auto* firstChannel = buffer.getWritePointer(0);

    for (Oscillator& oscillator : oscillators_) {
        if (oscillator.isPlaying()) {
            for (int sample = startSample; sample < endSample; ++sample) {
                firstChannel[sample] += oscillator.getSample();
            }
        }
    }

    for (int channel = 1; channel < buffer.getNumChannels(); ++channel) {
        std::copy(firstChannel + startSample, firstChannel + endSample, buffer.getWritePointer(channel) + startSample);
    }
}

void
Synthesizer::handleMidiEvent(const juce::MidiMessage& midiEvent) {
    if (midiEvent.isNoteOn()) {
        const int oscillatorId = midiEvent.getNoteNumber();
        const float frequency = midiNoteNumberToFrequency(oscillatorId);
        oscillators_[oscillatorId].setFrequency(frequency);
    } else if (midiEvent.isNoteOff()) {
        const int oscillatorId = midiEvent.getNoteNumber();
        oscillators_[oscillatorId].stop();
    } else if (midiEvent.isAllNotesOff()) {
        for (Oscillator& oscillator : oscillators_) {
            oscillator.stop();
        }
    }
}

float
Synthesizer::midiNoteNumberToFrequency(int midiNoteNumber) {
    constexpr float A4_FREQUENCY = 440.f;
    constexpr float A4_NOTE_NUMBER = 69.f;
    constexpr float SEMITONES_IN_AN_OCTAVE = 12.f;
    return A4_FREQUENCY * std::powf(2.f, (midiNoteNumber - A4_NOTE_NUMBER) / SEMITONES_IN_AN_OCTAVE);
}
