#ifndef __TIMER_H__
#define __TIMER_H__

#include "Utils.h"

class Timer
{
	public :
		static std::string Get()
		{
			SYSTEMTIME sysTime;
			FILETIME fTime, fTempTime;

			GetSystemTimeAsFileTime( &fTempTime );
			FileTimeToLocalFileTime( &fTempTime, &fTime );
			FileTimeToSystemTime( &fTime, &sysTime );

			sprintf( m_Temp, "%d", sysTime.wHour ); m_Hour = m_Temp;
			sprintf( m_Temp, "%d", sysTime.wMinute ); m_Minute = m_Temp;
			sprintf( m_Temp, "%d", sysTime.wSecond ); m_Second = m_Temp;
			sprintf( m_Temp, "%d", sysTime.wMilliseconds ); m_Milliseconds = m_Temp;

			CheckNumOfDigits( m_Hour, 2 );
			CheckNumOfDigits( m_Minute, 2 );
			CheckNumOfDigits( m_Second, 2 );
			CheckNumOfDigits( m_Milliseconds, 3 );

			return std::string( m_Hour + ":" + m_Minute + ":" + m_Second + ":" + m_Milliseconds );
		}

		static void CheckNumOfDigits( std::string& Value, int Num )
		{
			while ( Value.size() < Num )
			{
				Value = "0" + Value;
			}
		}

	private :
		static char m_Temp[ MAX_LENGTH ];
		static std::string m_Hour;
		static std::string m_Minute;
		static std::string m_Second;
		static std::string m_Milliseconds;
};

#endif // __TIMER_H__