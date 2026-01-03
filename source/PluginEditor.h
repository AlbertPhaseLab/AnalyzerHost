#pragma once
#include "PluginProcessor.h"

class AudioAnalyzerEditor : public juce::AudioProcessorEditor, 
                             private juce::Timer
{
public:
    AudioAnalyzerEditor (AudioAnalyzerProcessor&);
    ~AudioAnalyzerEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    // Enumerator for display modes
    enum class DisplayMode {
        Spectrum,
        Oscilloscope,
        PhaseMeter
    };

    DisplayMode currentMode = DisplayMode::Spectrum;

    // UI Elements
    juce::ComboBox modeSelector;

    // Drawing methods
    void drawSpectrum (juce::Graphics& g);
    void drawOscilloscope (juce::Graphics& g);
    void drawPhaseMeter (juce::Graphics& g);
    void drawNextFrameOfSpectrum();

    AudioAnalyzerProcessor& audioProcessor;
    float scopeData[AudioAnalyzerProcessor::fftSize];

    void drawSpectrumGrid (juce::Graphics& g, juce::Rectangle<float> area);
    void drawOscilloscopeGrid (juce::Graphics& g, juce::Rectangle<float> area);
    void drawPhaseGrid (juce::Graphics& g, juce::Rectangle<float> area);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioAnalyzerEditor)
};