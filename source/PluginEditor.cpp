#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <cmath> // For log10 and pow

AudioAnalyzerEditor::AudioAnalyzerEditor (AudioAnalyzerProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (800, 500);

    // Mode Selector Settings
    addAndMakeVisible (modeSelector);
    modeSelector.addItem ("FFT Spectrum", 1);
    modeSelector.addItem ("Oscilloscope", 2);
    modeSelector.addItem ("Phase Meter", 3);
    modeSelector.setSelectedId (1);
    
    modeSelector.onChange = [this] {
        switch (modeSelector.getSelectedId()) {
            case 1: currentMode = DisplayMode::Spectrum; break;
            case 2: currentMode = DisplayMode::Oscilloscope; break;
            case 3: currentMode = DisplayMode::PhaseMeter; break;
        }
        repaint();
    };

    startTimerHz (60);
}

AudioAnalyzerEditor::~AudioAnalyzerEditor() 
{ 
    stopTimer(); 
}

void AudioAnalyzerEditor::timerCallback()
{
    if (audioProcessor.nextBlockReady)
    {
        if (currentMode == DisplayMode::Spectrum) 
            drawNextFrameOfSpectrum();
            
        audioProcessor.nextBlockReady = false;
        repaint();
    }
}

void AudioAnalyzerEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    // We define the main visual area, leaving room for the units.
    auto visualArea = getLocalBounds().removeFromBottom (getHeight() - 60).reduced (20).toFloat();

    switch (currentMode) {
        case DisplayMode::Spectrum:     
            drawSpectrumGrid (g, visualArea);
            drawSpectrum (g); 
            break;
        case DisplayMode::Oscilloscope: 
            drawOscilloscopeGrid (g, visualArea);
            drawOscilloscope (g); 
            break;
        case DisplayMode::PhaseMeter:   
            drawPhaseGrid (g, visualArea);
            drawPhaseMeter (g); 
            break;
    }
}

// --- SUPPORT METHODS (GRIDS AND UNITS) ---

void AudioAnalyzerEditor::drawSpectrumGrid (juce::Graphics& g, juce::Rectangle<float> area)
{
    g.setColour (juce::Colours::grey.withAlpha (0.3f));
    
    // Reference frequencies (Hz) - Explicit casting to float to avoid errors in jmap
    float freqs[] = { 100.0f, 500.0f, 1000.0f, 5000.0f, 10000.0f, 20000.0f };
    for (auto f : freqs) {
        float x = juce::jmap (std::log10 (f), 
                              std::log10 (20.0f), 
                              std::log10 (20000.0f), 
                              area.getX(), 
                              area.getRight());

        g.drawVerticalLine (static_cast<int>(x), area.getY(), area.getBottom());
        g.drawText (f >= 1000.0f ? juce::String (f/1000.0f, 0) + "k" : juce::String (f, 0), 
                    static_cast<int>(x + 2), static_cast<int>(area.getBottom() + 2), 30, 15, juce::Justification::left);
    }

    // Amplitude (dB)
    float dbs[] = { 0.0f, -12.0f, -24.0f, -48.0f, -72.0f };
    for (auto db : dbs) {
        float y = juce::jmap (db, -100.0f, 0.0f, area.getBottom(), area.getY());
        g.drawHorizontalLine (static_cast<int>(y), area.getX(), area.getRight());
        g.drawText (juce::String (db, 0) + "dB", static_cast<int>(area.getX() - 40), static_cast<int>(y - 7), 35, 15, juce::Justification::right);
    }
}

void AudioAnalyzerEditor::drawOscilloscopeGrid (juce::Graphics& g, juce::Rectangle<float> area)
{
    g.setColour (juce::Colours::grey.withAlpha (0.3f));
    float midY = area.getCentreY();
    
    float levels[] = { 1.0f, 0.5f, 0.0f, -0.5f, -1.0f };
    for (auto l : levels) {
        float y = midY - (l * area.getHeight() * 0.4f);
        g.drawHorizontalLine (static_cast<int>(y), area.getX(), area.getRight());
        g.drawText (juce::String (l, 1), static_cast<int>(area.getX() - 30), static_cast<int>(y - 7), 25, 15, juce::Justification::right);
    }
}

void AudioAnalyzerEditor::drawPhaseGrid (juce::Graphics& g, juce::Rectangle<float> area)
{
    g.setColour (juce::Colours::grey.withAlpha (0.3f));
    auto center = area.getCentre();
    
    g.drawEllipse (center.x - 100, center.y - 100, 200, 200, 1.0f);
    g.drawVerticalLine (static_cast<int>(center.x), area.getY(), area.getBottom());
    g.drawHorizontalLine (static_cast<int>(center.y), area.getX(), area.getRight());
    
    g.setColour (juce::Colours::white.withAlpha (0.5f));
    g.drawText ("MID (M)", static_cast<int>(center.x - 25), static_cast<int>(area.getY() - 20), 50, 20, juce::Justification::centred);
    g.drawText ("SIDE (S)", static_cast<int>(area.getRight() + 5), static_cast<int>(center.y - 10), 50, 20, juce::Justification::left);
}

// --- SIGNAL DRAWING METHODS ---

void AudioAnalyzerEditor::drawOscilloscope (juce::Graphics& g)
{
    g.setColour (juce::Colours::yellow);
    juce::Path p;
    auto area = getLocalBounds().removeFromBottom (getHeight() - 60).reduced (20).toFloat();
    auto midY = area.getCentreY();

    for (int i = 0; i < AudioAnalyzerProcessor::fftSize; ++i) {
        float x = juce::jmap (static_cast<float>(i), 0.0f, static_cast<float>(AudioAnalyzerProcessor::fftSize), area.getX(), area.getRight());
        float y = midY + (audioProcessor.fifoL[i] * area.getHeight() * 0.4f);
        
        if (i == 0) p.startNewSubPath (x, y);
        else p.lineTo (x, y);
    }
    g.strokePath (p, juce::PathStrokeType (1.5f));
}

void AudioAnalyzerEditor::drawPhaseMeter (juce::Graphics& g)
{
    g.setColour (juce::Colours::limegreen);
    auto area = getLocalBounds().removeFromBottom (getHeight() - 60).reduced (20).toFloat();
    auto center = area.getCentre();
    
    juce::Path p;
    for (int i = 0; i < AudioAnalyzerProcessor::fftSize; i += 8) 
    {
        float l = audioProcessor.fifoL[i];
        float r = audioProcessor.fifoR[i];
        
        float x = center.x + (l - r) * (area.getWidth() * 0.35f);
        float y = center.y - (l + r) * (area.getHeight() * 0.35f);
        
        if (i == 0) p.startNewSubPath (x, y);
        else p.lineTo (x, y);
    }
    g.strokePath (p, juce::PathStrokeType (1.2f));
}

void AudioAnalyzerEditor::drawNextFrameOfSpectrum()
{
    audioProcessor.window.multiplyWithWindowingTable (audioProcessor.fftData, AudioAnalyzerProcessor::fftSize);
    audioProcessor.forwardFFT.performFrequencyOnlyForwardTransform (audioProcessor.fftData);

    for (int i = 0; i < AudioAnalyzerProcessor::fftSize; ++i)
    {
        auto level = juce::jmap (juce::jlimit (-100.0f, 0.0f, juce::Decibels::gainToDecibels (audioProcessor.fftData[i])),
                                 -100.0f, 0.0f, 0.0f, 1.0f);
        scopeData[i] = level;
    }
}

void AudioAnalyzerEditor::drawSpectrum (juce::Graphics& g)
{
    g.setColour (juce::Colours::cyan);
    juce::Path p;
    auto area = getLocalBounds().removeFromBottom (getHeight() - 60).reduced (20).toFloat();
    
    auto fftSizeHalf = static_cast<float>(AudioAnalyzerProcessor::fftSize) / 2.0f;
    for (int i = 0; i < static_cast<int>(fftSizeHalf); ++i)
    {
        float skew = 0.2f; 
        float x = area.getX() + area.getWidth() * std::pow (static_cast<float>(i) / fftSizeHalf, skew);
        float y = area.getBottom() - (scopeData[i] * area.getHeight());

        if (i == 0) p.startNewSubPath (x, y);
        else p.lineTo (x, y);
    }
    g.strokePath (p, juce::PathStrokeType (1.5f));
}

void AudioAnalyzerEditor::resized()
{
    modeSelector.setBounds (10, 10, 150, 30);
}