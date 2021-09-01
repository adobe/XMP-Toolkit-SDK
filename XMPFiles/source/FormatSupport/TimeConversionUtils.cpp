// =================================================================================================
// Copyright Adobe
// Copyright 2014 Adobe
// All Rights Reserved
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. 
// =================================================================================================

#include "public/include/XMP_Environment.h"	// ! XMP_Environment.h must be the first included header.
#include "public/include/XMP_Const.h"

#include "XMPFiles/source/FormatSupport/TimeConversionUtils.hpp"

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace TimeConversionUtils {

	static void DropFrameToHMSF(
		XMP_Int64 inFrames,
		XMP_Int64 inTimecodeFPS,
		XMP_Uns32& outHours,
		XMP_Uns32& outMinutes,
		XMP_Uns32& outSeconds,
		XMP_Uns32& outFrames)
	{
		XMP_Assert((inTimecodeFPS == 30) || (inTimecodeFPS == 60));	// No other drop frame rates are known at this time.

		XMP_Int64 rateAdjustmentFactor = inTimecodeFPS / 30;
		XMP_Int64 framesPerHour = (30 * 3600 - 108) * rateAdjustmentFactor;
		XMP_Int64 framesPer10Minutes = (30 * 600 - 18) * rateAdjustmentFactor;
		XMP_Int64 framesPerMinute = 30 * 60 * rateAdjustmentFactor;
		XMP_Int64 framesPerSecond = 30 * rateAdjustmentFactor;
		XMP_Int64 dropsPerMinute = 2 * rateAdjustmentFactor;

		XMP_Int64 currentFrames = inFrames;
		XMP_Int64 framesLeft = currentFrames;
		if (currentFrames < 0)
		{
			framesLeft = -currentFrames;
		}
		if (framesLeft >= framesPerHour)
		{
			outHours = static_cast<XMP_Int32>(framesLeft / framesPerHour);
			framesLeft = framesLeft % framesPerHour;
		}
		if (framesLeft >= framesPer10Minutes)
		{
			outMinutes = static_cast<XMP_Int32>(framesLeft / framesPer10Minutes) * 10;
			framesLeft = framesLeft % framesPer10Minutes;
		}
		if (framesLeft >= framesPerMinute)
		{
			XMP_Int64 remainingDropMinutes = static_cast<XMP_Int64>((framesLeft - framesPerMinute) /
				(framesPerMinute - dropsPerMinute));
			++remainingDropMinutes;

			outMinutes += static_cast<XMP_Int32>(remainingDropMinutes);
			framesLeft -= ((framesPerMinute - dropsPerMinute) * remainingDropMinutes);
		}
		if (framesLeft >= framesPerSecond)
		{
			outSeconds = static_cast<XMP_Int32>(framesLeft / framesPerSecond);
		}
		outFrames = static_cast<XMP_Int32>(framesLeft % framesPerSecond);
	}

	static bool ConvertSamplesToTimecode(
		std::string &					outTimecode,
		XMP_Int64						inSamples,
		XMP_Uns64						inSampleRate,
		XMP_Int64						inTimecodeFPS,
		bool							inIsDrop,
		bool							inIsNoDrop,
		bool							inShowOnlyFrames = false,
		bool							inOnlyShowSeconds = false ,
		bool							inNoZeroPrefix = false ,
		bool							inShowFractional = false ,
		bool							inNoHours = false )
	{
		if (!(inIsDrop ? !inIsNoDrop : true))
		{
			XMP_Assert( !(inIsDrop ? !inIsNoDrop : true) );
			return false;
		}

		if (inSampleRate == 0)
		{
			outTimecode = "00:00:00:00";
			return true;
		}

		std::string possibleNegStr;
		if (inSamples < 0)
		{
			inSamples *= -1;
			possibleNegStr = "-";
		}

		XMP_Int64 rateNumerator = inTimecodeFPS;
		XMP_Int64 rateDenominator = 1;
		if (inIsDrop || inIsNoDrop)
		{
			rateNumerator = 1000 * inTimecodeFPS;
			rateDenominator = 1001;
		}

		XMP_Int64 frameNumber = (inSamples * rateNumerator) / (inSampleRate * rateDenominator);
		XMP_Int64 hundredthsOfFrames = ((inSamples * rateNumerator * 100) / (inSampleRate * rateDenominator)) % 100;

		std::stringstream stream;
		double fSamples = static_cast<double>(inSamples);
		double fSampleRate = static_cast<double>(inSampleRate);

		if (inIsDrop)
		{
			if (inShowOnlyFrames)
			{
				double fAdjustmentFactor = static_cast<double>(inTimecodeFPS) / 30.0;
				double fCorrectionRatio = (600.0 * static_cast<double>(inTimecodeFPS) / 1.001) / (17982.0 * fAdjustmentFactor);
				double fValue = fSamples * fCorrectionRatio / fSampleRate;

				// "%ld"
				stream << static_cast<int>(fValue * 29.97 * fAdjustmentFactor);
			}
			else
			{
				XMP_Uns32 hours = 0;
				XMP_Uns32 minutes = 0;
				XMP_Uns32 seconds = 0;
				XMP_Uns32 frames = 0;

				DropFrameToHMSF(
					frameNumber,
					inTimecodeFPS,
					hours,
					minutes,
					seconds,
					frames);

				hours = hours % 24;
				// "%02d;%02d;%02d;%02d"
				stream << possibleNegStr << std::setfill('0') << std::setw(2) << static_cast<int>(hours)
					<< ";"
					<< std::setfill('0') << std::setw(2) << static_cast<int>(minutes)
					<< ";"
					<< std::setfill('0') << std::setw(2) << static_cast<int>(seconds)
					<< ";"
					<< std::setfill('0') << std::setw(2) << static_cast<int>(frames);
				possibleNegStr.clear();
			}
		}
		else
		{
			if (inShowOnlyFrames)
			{
				// "%ld"
				stream << static_cast<int>(frameNumber);
			}
			else
			{
				XMP_Int64 framesPerMinute = inTimecodeFPS * 60;
				XMP_Int64 framesPerHour = framesPerMinute * 60;

				XMP_Int64 iHours = frameNumber / framesPerHour;
				frameNumber %= framesPerHour;
				XMP_Int64 mins = frameNumber / framesPerMinute;
				frameNumber %= framesPerMinute;
				XMP_Int64 seconds = frameNumber / inTimecodeFPS;
				XMP_Int64 ss = frameNumber % inTimecodeFPS;
				XMP_Int64 s = seconds;

				if (inNoHours)
				{
					mins += iHours * 60;
					iHours = 0;
				}

				if (((iHours) || (!inNoZeroPrefix)) && (!inNoHours))
				{
					iHours = iHours % 24;
					// "%02ld:"
					stream << possibleNegStr << std::setfill('0') << std::setw(2) << static_cast<int>(iHours)
						<< ":";
					possibleNegStr.clear();
				}

				if ((iHours) || (!inNoZeroPrefix))
				{
					// "%02ld:"
					stream << possibleNegStr << std::setfill('0') << std::setw(2) << static_cast<int>(mins)
						<< ":";
					possibleNegStr.clear();
				}
				else if (mins)
				{
					// "%ld:"
					stream << possibleNegStr << static_cast<int>(mins)
						<< ":";
					possibleNegStr.clear();
				}

				if (inOnlyShowSeconds)
				{
					// "%02ld"
					stream << possibleNegStr << std::setfill('0') << std::setw(2) << static_cast<int>(s);
					possibleNegStr.clear();
				}
				else
				{
					if ((iHours) || (mins) || (!inNoZeroPrefix))
					{
						// "%02ld:"
						stream << possibleNegStr << std::setfill('0') << std::setw(2) << static_cast<int>(s)
							<< ":";
						possibleNegStr.clear();
					}
					else if (s)
					{
						// "%ld:"
						stream << possibleNegStr << static_cast<int>(s)
							<< ":";
						possibleNegStr.clear();
					}

					if ((iHours) || (mins) || (s) || (!inNoZeroPrefix))
					{
						if (inTimecodeFPS <= 10)
						{
							// "%01ld"
							stream << possibleNegStr << std::setfill('0') << std::setw(1) << static_cast<int>(ss);
							possibleNegStr.clear();
						}
						else if ((inTimecodeFPS <= 100))
						{
							// "%02ld"
							stream << possibleNegStr << std::setfill('0') << std::setw(2) << static_cast<int>(ss);
							possibleNegStr.clear();
						}
						else if (inTimecodeFPS <= 1000)
						{
							// "%03ld"
							stream << possibleNegStr << std::setfill('0') << std::setw(3) << static_cast<int>(ss);
							possibleNegStr.clear();
						}
						else
						{
							// "%04ld"
							stream << possibleNegStr << std::setfill('0') << std::setw(4) << static_cast<int>(ss);
							possibleNegStr.clear();
						}
					}
					else
					{
						// "%ld"
						stream << possibleNegStr << static_cast<int>(ss);
						possibleNegStr.clear();
					}

					if (inShowFractional)
					{
						// ".%02d"
						stream << possibleNegStr << "."
							<< std::setfill('0') << std::setw(2) << static_cast<int>(hundredthsOfFrames);
						possibleNegStr.clear();
					}
				}
			}
		}

		outTimecode = stream.str();

		return true;
	}

	bool ConvertSamplesToSMPTETimecode(
		std::string &			outTimecode,
		XMP_Int64				inSamples,
		XMP_Uns64				inSampleRate,
		const std::string &		inTimecodeFormat )
	{
		bool result = false;

		if ( inTimecodeFormat.compare( "24Timecode" ) == 0 ) {
			result = ConvertSamplesToTimecode( outTimecode, inSamples, inSampleRate, 24, false, false );
		} else if ( inTimecodeFormat.compare( "25Timecode" ) == 0 ) {
			result = ConvertSamplesToTimecode( outTimecode, inSamples, inSampleRate, 25, false, false );
		} else if ( inTimecodeFormat.compare( "2997DropTimecode") == 0 ) {
			result = ConvertSamplesToTimecode( outTimecode, inSamples, inSampleRate, 30, true, false );
		} else if ( inTimecodeFormat.compare( "2997NonDropTimecode") == 0 ) {
			result = ConvertSamplesToTimecode( outTimecode, inSamples, inSampleRate, 30, false, true );
		} else if ( inTimecodeFormat.compare( "30Timecode" ) == 0 ) {
			result = ConvertSamplesToTimecode( outTimecode, inSamples, inSampleRate, 30, false, false );
		} else if ( inTimecodeFormat.compare( "50Timecode" ) == 0 ) {
			result = ConvertSamplesToTimecode( outTimecode, inSamples, inSampleRate, 50, false, false );
		} else if ( inTimecodeFormat.compare( "5994DropTimecode" ) == 0 ) {
			result = ConvertSamplesToTimecode( outTimecode, inSamples, inSampleRate, 60, true, false );
		} else if ( inTimecodeFormat.compare( "5994NonDropTimecode" ) == 0 ) {
			result = ConvertSamplesToTimecode( outTimecode, inSamples, inSampleRate, 60, false, true );
		} else if ( inTimecodeFormat.compare( "60Timecode" ) == 0 ) {
			result = ConvertSamplesToTimecode( outTimecode, inSamples, inSampleRate, 60, false, false );
		} else if ( inTimecodeFormat.compare( "23976Timecode" ) == 0 ) {
			result = ConvertSamplesToTimecode(outTimecode, inSamples, inSampleRate, 24, false, true);
		}
		return result;
	}

	static bool StringToNumber(
		XMP_Int32 &			outNumber,
		const std::string & inString )
	{
		bool numberFound = false;
		outNumber = 0;
		for ( size_t i = 0, endIndex = inString.size(); i < endIndex; i++ ) {
			XMP_Int32 digit = inString[i] - '0';
			if ( digit >= 0 && digit <= 9 ) {
				outNumber *= 10;
				outNumber += digit;
				numberFound = true;
			} else {
				return numberFound;
			}
		}
		return numberFound;
	}

	static void ParseTimeCodeString(
		const std::string &		inTimecode, 
		XMP_Int32 &				outHours,
		XMP_Int32 &				outMinutes,
		XMP_Int32 &				outSeconds,
		XMP_Int32 &				outFrames,
		XMP_Int32 &				outFractionalFrameNumerator,
		XMP_Int32 &				outFractionalFrameDenominator )
	{
		XMP_Int32 m1 = 0;
		XMP_Int32 m2 = 0;
		XMP_Int32 m3 = 0;
		XMP_Int32 m4 = 0;
		XMP_Int32 m5 = 0;
		bool hasFoundDecimal = false;
		XMP_Int32 digitCount = 0;

		outFractionalFrameNumerator = 0;
		outFractionalFrameDenominator = 1;

		std::string::const_iterator iter = inTimecode.begin();
		std::string::const_iterator iterEnd = inTimecode.end();

		while (1)
		{					// Skip leading white space
			while ( iter != iterEnd && (*iter < '0' || *iter > '9') )
			{
				if (*iter == '.')
					hasFoundDecimal = true;
				iter++;
			}				// hh:mm:ss:ff.ddd
			if (iter == iterEnd)
				break;

			if (!hasFoundDecimal)
			{
				// get MSB digits
				StringToNumber(m1, std::string(iter, iterEnd));

				// Skip the digits
				while (iter != iterEnd && (*iter >= '0' && *iter <= '9'))
					iter++;

				// Skip the white space, note if "." or ":" ("." signifies decimal portion of frame)
				while ( iter != iterEnd && (*iter < '0' || *iter > '9'))
				{
					if (*iter == '.')
						hasFoundDecimal = true;
					iter++;
				}

				if (iter == iterEnd)
					break;
			}
			// shift and scan next MSB digits

			if (!hasFoundDecimal)
			{
				m2 = m1;
				digitCount = static_cast< XMP_Int32 >( iterEnd - iter );
				StringToNumber(m1, std::string(iter, iterEnd));

				// Skip the digits
				while ( iter != iterEnd && (*iter >= '0' && *iter <= '9') )
					iter++;

				// Skip the white space, note if "." or ":" ("." signifies
				// decimal portion of frame)
				while (iter != iterEnd && (*iter < '0' || *iter > '9'))
				{
					if (*iter == '.')
						hasFoundDecimal = true;
					iter++;
				}
			}

			if (iter == iterEnd)
				break;

			m3 = m2;
			m2 = m1;
			digitCount = static_cast< XMP_Int32 >( iterEnd - iter );
			StringToNumber(m1, std::string(iter, iterEnd));
			if (hasFoundDecimal)
				break;

			while (iter != iterEnd && (*iter >= '0' && *iter <= '9'))
				iter++;

			// Skip the white space, note if "." or ":" ("." signifies decimal portion of frame)
			while (iter != iterEnd && (*iter < '0' || *iter > '9'))
			{
				if (*iter == '.')
					hasFoundDecimal = true;
				iter++;
			}
			if (iter == iterEnd)
				break;

			m4 = m3;
			m3 = m2;
			m2 = m1;
			digitCount = static_cast< XMP_Int32 >( iterEnd - iter );
			StringToNumber(m1, std::string(iter, iterEnd));
			if (hasFoundDecimal)
				break;

			while (iter != iterEnd && (*iter >= '0' && *iter <= '9'))
			{
				iter++;
			}

			// Skip the white space, note if "." or ":" ("." signifies decimal portion of frame)
			while (iter != iterEnd && (*iter < '0' || *iter > '9'))
			{
				if (*iter == '.')
					hasFoundDecimal = true;
				iter++;
			}

			if (iter == iterEnd)
				break;

			m5 = m4;
			m4 = m3;
			m3 = m2;
			m2 = m1;
			digitCount = static_cast< XMP_Int32 >( iterEnd - iter );
			StringToNumber(m1, std::string(iter, iterEnd));
			break;
		}

		if (hasFoundDecimal)
		{
			outFractionalFrameDenominator = static_cast<XMP_Int32>(pow(10.0, digitCount) + 0.5);
			outFractionalFrameNumerator = m1;
			m1 = m2;
			m2 = m3;
			m3 = m4;
			m4 = m5;
			m5 = 0;
		}
		outHours = m4;
		outMinutes = m3;
		outSeconds = m2;
		outFrames = m1;
	}

	static bool ConvertTimecodeToSamples(
		XMP_Int64 &						outSamples,
		const std::string &				inTimecode,
		XMP_Uns64						inSampleRate,
		XMP_Int64						inTimecodeFPS,
		bool							inNTSC,
		bool							inDropFrame)
	{
		/// @TODO: Ensure that negative and >64-bit values are OK and work as expected.

		if (inTimecode.empty())
		{
			outSamples = static_cast<XMP_Int64>(-1);
			return true;
		}

		XMP_Int32 hours;
		XMP_Int32 minutes;
		XMP_Int32 seconds;
		XMP_Int32 frames;
		XMP_Int32 fractionalFrameNumerator;
		XMP_Int32 fractionalFrameDenominator;

		ParseTimeCodeString(inTimecode, hours, minutes, seconds, frames, fractionalFrameNumerator, fractionalFrameDenominator);

		XMP_Int64 framesPerSecond = inTimecodeFPS;
		XMP_Int64 framesPerMinute = framesPerSecond * 60;
		XMP_Int64 framesPerHour = framesPerMinute * 60;
		XMP_Int64 wholeFrames = 0;
		XMP_Int64 frameRateNumerator = inTimecodeFPS;
		XMP_Int64 frameRateDenominator = 1;

		if (inNTSC)
		{
			frameRateNumerator = 1000 * inTimecodeFPS;
			frameRateDenominator = 1001;
		}

		if (inDropFrame)
		{
			XMP_Int64 frameGroupDropped = 2 * inTimecodeFPS / 30;		// 2 or 4 frames dropped at a time.
			XMP_Int64 framesPerHourDropped = 108 * inTimecodeFPS / 30;
			framesPerHour -= framesPerHourDropped;
			XMP_Int64 framesPerTenMinutes = framesPerHour / 6;
			XMP_Assert( framesPerHour % 6 == 0 );	//, "Drop frame not supported on the given frame rate."
			XMP_Int64 framesDroppedWithinTheLeastTenMinutes = 0;
			if (minutes % 10 != 0)
			{
				if ((seconds == 0) && (frames < frameGroupDropped))
				{
					frames = static_cast<XMP_Int32>(frameGroupDropped);	// Make sure invalid strings snap to the next higher valid frame.
				}
				framesDroppedWithinTheLeastTenMinutes = (minutes % 10) * frameGroupDropped;
			}
			wholeFrames = hours * framesPerHour + (minutes / 10) * framesPerTenMinutes + (minutes % 10) * framesPerMinute + seconds * framesPerSecond + frames - framesDroppedWithinTheLeastTenMinutes;
		}
		else
		{
			wholeFrames = hours * framesPerHour + minutes * framesPerMinute + seconds * framesPerSecond + frames;
		}

		if (frameRateNumerator * fractionalFrameDenominator == 0)
		{
			XMP_Assert( "Divide by zero in ConvertTimecodeToSamples" );
			outSamples = 0;
			return true;
		}

		//
		// (frame count / frames per second) * samples per second = sample count.
		//
		// ((frame count + fractionalFrameNumerator / fractionalFrameDenominator) * samples per second / frames per second) = sample count.
		// or in integer math:
		// ((frame count * fractionalFrameDenominator + fractionalFrameNumerator) * samples per second / (frames per second * fractionalFrameDenominator)) = sample count.
		// with rounding correction to give us the first sample contained entirely in the frame:

		// There is a non-zero probability of rolling over this integer arithmetic.
		double integerFailsafeNumerator = ((static_cast<double>(wholeFrames) * static_cast<double>(fractionalFrameDenominator) + fractionalFrameNumerator) * static_cast<double>(frameRateDenominator) * static_cast<double>(inSampleRate) + (frameRateNumerator * fractionalFrameDenominator - 1));
		if (integerFailsafeNumerator > static_cast<double>(0x7000000000000000LL))
		{
			outSamples = static_cast<XMP_Int64>(integerFailsafeNumerator / (static_cast<double>(frameRateNumerator) * static_cast<double>(fractionalFrameDenominator)));
		}
		else
		{
			outSamples = ((wholeFrames * fractionalFrameDenominator + fractionalFrameNumerator) * frameRateDenominator * inSampleRate + (frameRateNumerator * fractionalFrameDenominator - 1)) / (frameRateNumerator * fractionalFrameDenominator);
		}
		return true;
	}

	bool ConvertSMPTETimecodeToSamples(
		XMP_Int64 &				outSamples,
		const std::string &		inTimecode,
		XMP_Uns64				inSampleRate,
		const std::string & inTimecodeFormat )
	{
		bool result = false;

		if ( inTimecodeFormat.compare( "24Timecode" ) == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 24, false, false );
		} else if ( inTimecodeFormat.compare( "25Timecode" ) == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 25, false, false );
		} else if ( inTimecodeFormat.compare( "2997DropTimecode") == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 30, true, true );
		} else if ( inTimecodeFormat.compare( "2997NonDropTimecode") == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 30, true, false );
		} else if ( inTimecodeFormat.compare( "30Timecode" ) == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 30, false, false );
		} else if ( inTimecodeFormat.compare( "50Timecode" ) == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 50, false, false );
		} else if ( inTimecodeFormat.compare( "5994DropTimecode" ) == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 60, true, true );
		} else if ( inTimecodeFormat.compare( "5994NonDropTimecode" ) == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 60, true, false );
		} else if ( inTimecodeFormat.compare( "60Timecode" ) == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 60, false, false );
		} else if ( inTimecodeFormat.compare( "23976Timecode" ) == 0 ) {
			result = ConvertTimecodeToSamples( outSamples, inTimecode, inSampleRate, 24, true, false );
		}
		return result;
	}

}
