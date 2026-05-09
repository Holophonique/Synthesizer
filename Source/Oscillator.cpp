#include "Oscillator.h"

#include <cmath>


Oscillator::Oscillator(std::vector<float> waveTable, double sampleRate) :
    waveTable_(std::move(waveTable)),
    sampleRate_(sampleRate) {}

void
Oscillator::setFrequency(float frequency) {
    indexIncrement_ = frequency * waveTable_.size() / sampleRate_;
}

float
Oscillator::getSample() {
    const float sample = interpolateLinearly();
    index_ += indexIncrement_;
    index_ = std::fmod(index_, waveTable_.size());
    return sample;
}

float
Oscillator::interpolateLinearly() const {
    const int truncatedIndex = static_cast<int>(index_);
    const int nextIndex = (truncatedIndex + 1) % static_cast<int>(waveTable_.size());

    const float nextIndexWeight = index_ - static_cast<float>(truncatedIndex);
    const float truncatedIndexWeight = 1.f - nextIndexWeight;

    return truncatedIndexWeight * waveTable_[truncatedIndex] + nextIndexWeight * waveTable_[nextIndex];
}

void
Oscillator::stop() {
    index_ = 0.f;
    indexIncrement_ = 0.f;
}

bool
Oscillator::isPlaying() const {
    return indexIncrement_ != 0.f;
}
