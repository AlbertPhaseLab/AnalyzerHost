# AlbertPhaseLab AnalyzerHost

**AlbertPhaseLab AnalyzerHost** is a high-performance, JUCE-based audio plugin designed for real-time signal monitoring and forensic audio analysis. Developed in **C++20**, this tool provides engineers with a transparent "pass-through" architecture, ensuring the input signal remains bit-identical at the output while providing deep visual insights into the audio's frequency, time, and phase domains.

---

## 🚀 Core Features

* **Transparent Pass-Through:** Engineered for zero-latency processing. The audio signal is captured for analysis without modifying the original signal path.
* **Triple-Engine Analysis Suite:**
    * **FFT Spectrum:** High-resolution frequency distribution visualization (2048 points).
    * **Oscilloscope:** Real-time time-domain waveform monitoring.
    * **Phase Meter (Goniometer):** Advanced L/R correlation plotting for stereo field and mono compatibility analysis.
* **Optimized Performance:** Uses `juce::dsp` and `juce::FloatVectorOperations` for efficient, vectorized signal processing.
* **Dynamic UI:** Mode-switching interface to toggle between visualizers instantly via a simple dropdown menu.
* 
---
