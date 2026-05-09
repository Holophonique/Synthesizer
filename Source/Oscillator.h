#pragma once

#include <vector>


class Oscillator {
  public:
    Oscillator(std::vector<float> waveTable, double sampleRate);

    void setFrequency(float frequency);
    float getSample();
    void stop();
    bool isPlaying() const;

  private:
    float interpolateLinearly() const;

    std::vector<float> waveTable_;
    double sampleRate_;
    float index_ = 0.f;
    float indexIncrement_ = 0.f;
};
