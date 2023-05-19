#include "Log.h"

Log::Log() {}

Log::~Log() {}

void Log::Info( const char* Str, ... )
{
	char Buff[ MAX_LENGTH ] = "INFO ";

	va_list pArg;
	va_start( pArg, Str );
	vsprintf( Buff + strlen( Buff ), Str, pArg );
	va_end( pArg );

	std::string Temp = Timer::Get() + "| " + Buff;

	m_Message.push_back( Temp );
}

void Log::Error( const char* Str, ... )
{
	char Buff[ MAX_LENGTH ] = "ERROR ";

	va_list pArg;
	va_start( pArg, Str );
	vsprintf( Buff + strlen( Buff ), Str, pArg );
	va_end( pArg );

	std::string Temp = Timer::Get() + " | " + Buff;

	m_Message.push_back( Temp );
}

void Log::Warn( const char* Str, ... )
{
	char Buff[ MAX_LENGTH ] = "WARN ";

	va_list pArg;
	va_start( pArg, Str );
	vsprintf( Buff + strlen( Buff ), Str, pArg );
	va_end( pArg );

	std::string Temp = Timer::Get() + " | " + Buff;

	m_Message.push_back( Temp );
}

std::vector< std::string>& Log::GetMessage() { return m_Message; }
void Log::Clear() { m_Message.clear(); }

Log Log::m_Log;
std::vector< std::string > Log::m_Message;