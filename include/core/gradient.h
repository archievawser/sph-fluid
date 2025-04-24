#pragma once
#include <vector>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include "utils/utils.h"


template<typename T>
struct GradientKeyframe
{
	GradientKeyframe(float timepoint, T value)
		: Timepoint(timepoint), Value(value)
	{ }

	GradientKeyframe()
		: Timepoint(0.f), Value()
	{ }

	bool operator < (const GradientKeyframe& other)
	{
		return Timepoint < other.Timepoint;
	}

	float Timepoint;
	T Value;
};

template<typename T = float>
class Gradient
{
public:
	Gradient(std::vector<GradientKeyframe<T>> keys = std::vector<GradientKeyframe<T>>())
		: mKeyframes(keys)
	{
		std::sort(mKeyframes.begin(), mKeyframes.end());

		if (mKeyframes.size() == 0 || mKeyframes.front().Timepoint != 0.f)
			mKeyframes.insert(mKeyframes.begin(), GradientKeyframe(0.f, T()));

		if (mKeyframes.size() == 0 || mKeyframes.back().Timepoint != 1.f)
			mKeyframes.push_back(GradientKeyframe(1.f, T()));
	}

public:
	inline T Sample(float timepoint)
	{
		if (timepoint <= 0.f) return mKeyframes.front().Value;
		if (timepoint >= 1.f) return mKeyframes.back().Value;

		GradientKeyframe<T>* initialValue = &mKeyframes.front();
		GradientKeyframe<T>* finalValue = &mKeyframes.back();

		for (int i = 0; i < mKeyframes.size(); i++)
		{
			if (mKeyframes[i].Timepoint > timepoint)
			{
				initialValue = &mKeyframes[i - 1];
				finalValue = &mKeyframes[i];
				break;
			}
		}

		float flattenedFinalT = finalValue->Timepoint - initialValue->Timepoint;
		float flattenedT = timepoint - initialValue->Timepoint;
		float relativizedT = flattenedT / flattenedFinalT;
		return Lerp(initialValue->Value, finalValue->Value, relativizedT);
	}

	static inline T Lerp(T a, T b, float t)
	{
		return a + (b - a) * t;
	}

	static inline float Map(float value, float lower0, float upper0, float lower1, float upper1)
	{
		return lower1 + (upper1 - lower1) * (value - lower0 / upper0 - lower0);
	}

	static inline Color3 FromHex(uint32_t hex)
	{
		return Color3(
			(hex >> 16) & 0x0000ff,
			(hex >> 8) & 0x0000ff,
			(hex) & 0x0000ff
		) / 255.f;
	}

private:
	std::vector<GradientKeyframe<T>> mKeyframes;
};