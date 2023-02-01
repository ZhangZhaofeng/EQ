#include "EQ.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <algorithm>

	FEqualizer::FEqualizer()
		: NumBands(0)
		, NumChannels(0)
		, FilterBands(nullptr)
	{
	}

	FEqualizer::~FEqualizer()
	{
		if (FilterBands)
		{
			delete[] FilterBands;
			FilterBands = nullptr;
		}
	}

	void FEqualizer::Init(const float InSampleRate, const int InNumBands, const int InNumChannels)
	{
		if (FilterBands)
		{
			delete[] FilterBands;
			FilterBands = nullptr;
		}
		SampleRate = InSampleRate;
		NumBands = InNumBands;
		NumChannels = InNumChannels;
		FilterBands = new FBiquadFilter[NumBands];

		for (int Band = 0; Band < NumBands; ++Band)
		{
			FilterBands[Band].Init(InSampleRate, InNumChannels, EBiquadFilter::ParametricEQ, 500.0f, 1.0f, 0.0f);
		}
	}

	void FEqualizer::SetBandEnabled(const int InBand, const bool bInEnabled)
	{
		if (!FilterBands || InBand >= NumBands)
		{
			return;
		}

		FilterBands[InBand].SetEnabled(bInEnabled);
	}

	void FEqualizer::SetBandParams(const int InBand, const float InFrequency, const float InBandwidth, const float InGainDB,EBiquadFilter::Type pass)
	{
		if (!FilterBands || InBand >= NumBands)
		{
			return;
		}
		//float Frequency = std::min(InFrequency, SampleRate/2-10);

		FilterBands[InBand].SetParams(pass, InFrequency, InBandwidth, InGainDB);
	}

	void FEqualizer::SetBandFrequency(const int InBand, const float InFrequency)
	{
		if (!FilterBands || InBand >= NumBands)
		{
			return;
		}


		FilterBands[InBand].SetFrequency(InFrequency);
	}

	void FEqualizer::SetBandBandwidth(const int InBand, const float InBandwidth)
	{
		if (!FilterBands || InBand >= NumBands)
		{
			return;
		}

		FilterBands[InBand].SetBandwidth(InBandwidth);
	}

	void FEqualizer::SetBandGainDB(const int InBand, const float InGainDB)
	{
		if (!FilterBands || InBand >= NumBands)
		{
			return;
		}

		FilterBands[InBand].SetGainDB(InGainDB);
	}

	void FEqualizer::ProcessAudioFrame(const float* InAudio, float* OutAudio)
	{
		float AudioBufferScratchInput[8];

		for (int Channel = 0; Channel < NumChannels; ++Channel)
		{
			AudioBufferScratchInput[Channel] = InAudio[Channel];
		}
	
		// Process each band in parallel
		for (int Band = 0; Band < NumBands; ++Band)
		{
			FilterBands[Band].ProcessAudioFrame(AudioBufferScratchInput, OutAudio);

			for (int Channel = 0; Channel < NumChannels; ++Channel)
			{
				AudioBufferScratchInput[Channel] = OutAudio[Channel];
			}
		}
		
		
	}


