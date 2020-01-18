#include "GameTimer.h"

GameTimer::GameTimer()
{
	mDeltaTime = 0;
	mBaseTime = 0;
	mPausedTime = 0;
	mPrevTime = 0;
	mCurrTime = 0;
	mStopTime = -1;
	mPaused = false;

	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

void GameTimer::Tick()
{
	if (mPaused)
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;
	mPrevTime = mCurrTime;

	if (mDeltaTime < 0)
	{
		mDeltaTime = 0;
	}
}

float GameTimer::DeltaTime() const
{
	return static_cast<float>(mDeltaTime);
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mPaused = false;
}

void GameTimer::Pause()
{
	if (!mPaused)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mStopTime = currTime;
		mPaused = true;
	}
}

void GameTimer::Start()
{
	if (mPaused)
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		
		mPaused = false;
		mStopTime = 0;
	}
}

float GameTimer::TotalTime() const
{
	if (mPaused)
	{
		return static_cast<float>( (mStopTime - mPausedTime - mBaseTime)* mSecondsPerCount );
	}
	else
	{
		return static_cast<float>( (mCurrTime - mBaseTime - mPausedTime) * mSecondsPerCount );
	}
}
