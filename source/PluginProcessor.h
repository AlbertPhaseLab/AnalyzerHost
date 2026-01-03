#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class AudioAnalyzerProcessor  : public juce::AudioProcessor
{
public:
    AudioAnalyzerProcessor();
    ~AudioAnalyzerProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "AlbertPhaseLab"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int index) override { juce::ignoreUnused (index); }
    const juce::String getProgramName (int index) override { juce::ignoreUnused (index); return {}; }
    void changeProgramName (int index, const juce::String& newName) override { juce::ignoreUnused (index, newName); }
    
    void getStateInformation (juce::MemoryBlock& destData) override { juce::ignoreUnused (destData); }
    void setStateInformation (const void* data, int sizeInBytes) override { juce::ignoreUnused (data, sizeInBytes); }
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    static constexpr int fftOrder = 11;
    static constexpr int fftSize = 1 << fftOrder;
    
    float fftData[fftSize * 2];
    float fifoL[fftSize]; // We will use L for the default mono oscilloscope
    float fifoR[fftSize];
    int fifoIndex = 0;
    bool nextBlockReady = false; // Flag to indicate when a new block is ready for processing

    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioAnalyzerProcessor)
};