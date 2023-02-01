
#include "BiQuadFilter.h"
	// Enumeration of filter types
	namespace EBiquadFilter
	{
		enum Type
		{
			Lowpass,
			Highpass,
			Bandpass,
			Notch,
			ParametricEQ,
			AllPass,
		};
	}

	// Biquad filter class which wraps a biquad filter struct
	// Handles multi-channel audio to avoid calculating filter coefficients for multiple channels of audio.
	class FBiquadFilter
	{
	public:
		// Constructor
		FBiquadFilter();
		// Destructor
		virtual ~FBiquadFilter();

		// Initialize the filter
		void Init(const float InSampleRate, const int InNumChannels, const EBiquadFilter::Type InType, const float InCutoffFrequency = 20000, const float InBandwidth = 2.0f, const float InGain = 0.0f);

		// Resets the filter state
		void Reset();

		// Processes a single frame of audio
		void ProcessAudioFrame(const float* InAudio, float* OutAudio);

		// Sets all filter parameters with one function
		void SetParams(const EBiquadFilter::Type InFilterType, const float InCutoffFrequency, const float InBandwidth, const float InGainDB);

		// Sets the type of the filter to use
		void SetType(const EBiquadFilter::Type InType);

		// Sets the filter frequency
		void SetFrequency(const float InCutoffFrequency);

		// Sets the bandwidth (octaves) of the filter
		void SetBandwidth(const float InBandwidth);

		// Sets the gain of the filter in decibels
		void SetGainDB(const float InGainDB);

		// Sets whether or no this filter is enabled (if disabled audio is passed through)
		void SetEnabled(const bool bInEnabled);

	protected:

		// Function computes biquad coefficients based on current filter settings
		void CalculateBiquadCoefficients();

		// What kind of filter to use when computing coefficients
		EBiquadFilter::Type FilterType;

		// Biquad filter objects for each channel
		FBiquad* Biquad;

		// The sample rate of the filter
		float SampleRate;

		// Number of channels in the filter
		int NumChannels;

		// Current frequency of the filter
		float Frequency;

		// Current bandwidth/resonance of the filter
		float Bandwidth;

		// The gain of the filter in DB (for filters that use gain)
		float GainDB;

		// Whether or not this filter is enabled (will bypass otherwise)
		bool bEnabled;
	};

