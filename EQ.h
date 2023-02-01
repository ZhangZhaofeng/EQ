#ifndef __PEQ_H__
#define __PEQ_H__
#include "Filter.h"
	// Equalizer filter
	// An equalizer is a cascaded (serial) band of parametric EQs
	// This filter allows for setting each band with variable Bandwidth/Q, Frequency, and Gain
	class FEqualizer
	{
	public:
		// Constructor
		FEqualizer();

		// Destructor
		~FEqualizer();

		// Initialize the equalizer
		void Init(const float InSampleRate, const int InNumBands, const int InNumChannels);

		// Sets whether or not the band is enabled
		void SetBandEnabled(const int InBand, const bool bEnabled);

		// Sets all params of the band at once
		void SetBandParams(const int InBand, const float InFrequency, const float InBandwidth, const float InGainDB,EBiquadFilter::Type pass);

		// Sets the band frequency
		void SetBandFrequency(const int InBand, const float InFrequency);

		// Sets the band resonance (use alternatively to bandwidth)
		void SetBandBandwidth(const int InBand, const float InBandwidth);

		// Sets the band gain in decibels
		void SetBandGainDB(const int InBand, const float InGainDB);

		// Processes the audio frame (audio frame must have channels equal to that used during initialization)
		void ProcessAudioFrame(const float* InAudio, float* OutAudio);
		// The number of bands in the equalizer
		int NumBands;

		// The number of channels in the equalizer
		int NumChannels;
		
	private:

		// The array of biquad filters
		FBiquadFilter* FilterBands;
		float SampleRate;

	};
#endif