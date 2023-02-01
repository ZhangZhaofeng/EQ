#include "Filter.h"
#include <math.h>
#include <algorithm>
FBiquadFilter::FBiquadFilter()
		: FilterType(EBiquadFilter::Lowpass)
		, Biquad(nullptr)
		, SampleRate(0.0f)
		, NumChannels(0)
		, Frequency(0.0f)
		, Bandwidth(0.0f)
		, GainDB(0.0f)
		, bEnabled(true)
	{
	}

	FBiquadFilter::~FBiquadFilter()
	{
		if (Biquad)
		{
			delete[] Biquad;
		}
	}

	void FBiquadFilter::Init(const float InSampleRate, const int InNumChannels, const EBiquadFilter::Type InFilterType, const float InCutoffFrequency, const float InBandwidth, const float InGainDB)
	{
		SampleRate = InSampleRate;
		NumChannels = InNumChannels;
		FilterType = InFilterType;
		Frequency = std::max(InCutoffFrequency, 20.0f);
		Bandwidth = InBandwidth;
		GainDB = InGainDB;
		if (Biquad)
		{
			delete[] Biquad;
		}

		Biquad = new FBiquad[NumChannels];
		Reset();
		CalculateBiquadCoefficients();
	}

	void FBiquadFilter::ProcessAudioFrame(const float* InAudio, float* OutAudio)
	{
		if (bEnabled)
		{
			for (int Channel = 0; Channel < NumChannels; ++Channel)
			{
				OutAudio[Channel] = Biquad[Channel].ProcessAudio(InAudio[Channel]);
			}
		}
		else
		{
			// Pass through if disabled
			for (int Channel = 0; Channel < NumChannels; ++Channel)
			{
				OutAudio[Channel] = InAudio[Channel];
			}
		}
	}

	void FBiquadFilter::Reset()
	{
		for (int Channel = 0; Channel < NumChannels; ++Channel)
		{
			Biquad[Channel].Reset();
		}
	}

	void FBiquadFilter::SetParams(const EBiquadFilter::Type InFilterType, const float InCutoffFrequency, const float InBandwidth, const float InGainDB)
	{
		const float InCutoffFrequencyClamped = std::max(InCutoffFrequency, 20.0f);

		if (InFilterType != FilterType ||
			InCutoffFrequencyClamped != InCutoffFrequency ||
			Bandwidth != InBandwidth ||
			GainDB != InGainDB)
		{
			FilterType = InFilterType;
			Frequency = InCutoffFrequency;
			Bandwidth = InBandwidth;
			GainDB = InGainDB;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadFilter::SetType(const EBiquadFilter::Type InFilterType)
	{
		if (FilterType != InFilterType)
		{
			FilterType = InFilterType;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadFilter::SetFrequency(const float InCutoffFrequency)
	{
		const float InCutoffFrequencyClamped = std::max(InCutoffFrequency, 0.0f);
		if (Frequency != InCutoffFrequencyClamped)
		{
			Frequency = InCutoffFrequencyClamped;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadFilter::SetBandwidth(const float InBandwidth)
	{
		if (Bandwidth != InBandwidth)
		{
			Bandwidth = InBandwidth;
			CalculateBiquadCoefficients();
		}
	}

	void FBiquadFilter::SetGainDB(const float InGainDB)
	{
		if (GainDB != InGainDB)
		{
			GainDB = InGainDB;
			if (FilterType == EBiquadFilter::ParametricEQ)
			{
				CalculateBiquadCoefficients();
			}
		}
	}

	void FBiquadFilter::SetEnabled(const bool bInEnabled)
	{
		bEnabled = bInEnabled;
	}

	void FBiquadFilter::CalculateBiquadCoefficients()
	{
		static const float NaturalLog2 = 0.69314718055994530942f;

		const float Omega = 2.0f * 3.1415926f * Frequency / SampleRate;
		const float Sn =sinf(Omega);
		const float Cs = cosf(Omega);

		const float Alpha = Sn * sinf(0.5f * NaturalLog2 * Bandwidth * Omega / Sn);

		float a0;
		float a1;
		float a2;
		float b0;
		float b1;
		float b2;

		switch (FilterType)
		{
			default:
			case EBiquadFilter::Lowpass:
			{
				a0 = (1.0f - Cs) / 2.0f;
				a1 = (1.0f - Cs);
				a2 = (1.0f - Cs) / 2.0f;
				b0 = 1.0f + Alpha;
				b1 = -2.0f * Cs;
				b2 = 1.0f - Alpha;
			}
			break;

			case EBiquadFilter::Highpass:
			{
				a0 = (1.0f + Cs) / 2.0f;
				a1 = -(1.0f + Cs);
				a2 = (1.0f + Cs) / 2.0f;
				b0 = 1.0f + Alpha;
				b1 = -2.0f * Cs;
				b2 = 1.0f - Alpha;
			}
			break;

			case EBiquadFilter::Bandpass:
			{
				a0 = Alpha;
				a1 = 0.0f;
				a2 = -Alpha;
				b0 = 1.0f + Alpha;
				b1 = -2.0f * Cs;
				b2 = 1.0f - Alpha;
			}
			break;

			case EBiquadFilter::Notch:
			{
				a0 = 1.0f;
				a1 = -2.0f * Cs;
				a2 = 1.0f;
				b0 = 1.0f + Alpha;
				b1 = -2.0f * Cs;
				b2 = 1.0f - Alpha;
			}
			break;

			case EBiquadFilter::ParametricEQ:
			{
				const float Amp = pow(10.0f, GainDB / 40.0f);
				const float Beta = sqrt(2.0f * Amp);

				a0 = 1.0f + (Alpha * Amp);
				a1 = -2.0f * Cs;
				a2 = 1.0f - (Alpha * Amp);
				b0 = 1.0f + (Alpha / Amp);
				b1 = -2.0f * Cs;
				b2 = 1.0f - (Alpha / Amp);
			}
			break;

			case EBiquadFilter::AllPass:
			{
				a0 = 1.0f - Alpha;
				a1 = -2.0f * Cs;
				a2 = 1.0f + Alpha;
				b0 = 1.0f + Alpha;
				b1 = -2.0f * Cs;
				b2 = 1.0f - Alpha;
			}
			break;
		}

		a0 /= b0;
		a1 /= b0;
		a2 /= b0;
		b1 /= b0;
		b2 /= b0;

		for (int Channel = 0; Channel < NumChannels; ++Channel)
		{
			Biquad[Channel].A0 = a0;
			Biquad[Channel].A1 = a1;
			Biquad[Channel].A2 = a2;
			Biquad[Channel].B1 = b1;
			Biquad[Channel].B2 = b2;
		}
	}



