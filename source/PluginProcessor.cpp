#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioAnalyzerProcessor::AudioAnalyzerProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),
      forwardFFT (fftOrder),
      window ((size_t)fftSize, juce::dsp::WindowingFunction<float>::hann)
{
}

AudioAnalyzerProcessor::~AudioAnalyzerProcessor()
{
}

//==============================================================================
void AudioAnalyzerProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Clear unused variable warnings
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    // Initialize buffers
    juce::zeromem (fftData, sizeof (fftData));
    juce::zeromem (fifoL, sizeof (fifoL));
    juce::zeromem (fifoR, sizeof (fifoR));
    fifoIndex = 0;
    nextBlockReady = false;
}

void AudioAnalyzerProcessor::releaseResources()
{
}

bool AudioAnalyzerProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AudioAnalyzerProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    // Clear mideiMessages to avoid unused variable warnings
    juce::ignoreUnused (midiMessages);

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // Audio capture logic (Pass-through)
    for (int i = 0; i < numSamples; ++i)
    {
        float sampleL = 0.0f;
        float sampleR = 0.0f;

        if (numChannels >= 2) // Stereo
        {
            sampleL = buffer.getReadPointer (0)[i];
            sampleR = buffer.getReadPointer (1)[i];
        }
        else if (numChannels == 1) // Mono
        {
            sampleL = sampleR = buffer.getReadPointer (0)[i];
        }

        fifoL[fifoIndex] = sampleL;
        fifoR[fifoIndex] = sampleR;

        if (++fifoIndex == fftSize) // Buffer full
        {
            if (!nextBlockReady) // Only set if the previous block has been processed
            {
                juce::FloatVectorOperations::clear (fftData, fftSize * 2); // Clear fftData buffer
                for (int j = 0; j < fftSize; ++j) // Copy interleaved data
                {
                    fftData[j] = (fifoL[j] + fifoR[j]) * 0.5f; // Mono mix for FFT
                }
                
                nextBlockReady = true; // Indicate that a new block is ready for processing
            }
            fifoIndex = 0; // Reset index for next block
        }
    }
}

//==============================================================================
juce::AudioProcessorEditor* AudioAnalyzerProcessor::createEditor()
{
    return new AudioAnalyzerEditor (*this);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioAnalyzerProcessor();
}