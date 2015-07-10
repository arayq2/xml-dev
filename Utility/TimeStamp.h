#pragma once

#include <time.h>
#include <ostream>
#include <iomanip>

namespace Utility
{
    class FormatSaver
    {
    public:
        FormatSaver(std::ostream& os)
        : os_(os)
        , state_(nullptr)
        {
            state_.copyfmt( os_ );
        }
        
        ~FormatSaver()
        {
            os_.copyfmt( state_ );
        }

    private:
        FormatSaver(FormatSaver const&) = delete;
        FormatSaver& operator= ( FormatSaver const& ) = delete;
        std::ostream&       os_;
        std::ios            state_;
    };

    class TimeStamp
    {
    public:
        TimeStamp()
        {
            ::clock_gettime( CLOCK_REALTIME, &ts_ );
            ::localtime_r( &ts_.tv_sec, &tm_ );
        }
        
        TimeStamp(time_t secs, long nano = 0)
        : ts_({ secs, nano })
        {
            ::localtime_r( &ts_.tv_sec, &tm_ );
        }
        
        std::ostream& output( std::ostream& os ) const
        {
            FormatSaver _sf(os);
            
            os << std::setfill( '0' )
               << std::setw( 4 ) << (tm_.tm_year + 1900) << "-"
               << std::setw( 2 ) << (tm_.tm_mon + 1) << "-"
               << std::setw( 2 ) << tm_.tm_mday << "T"
               << std::setw( 2 ) << tm_.tm_hour << ":"
               << std::setw( 2 ) << tm_.tm_min << ":"
               << std::setw( 2 ) << tm_.tm_sec << "."
               << std::setw( 3 ) << (ts_.tv_nsec / 1000000)
               ;
               
            return os;
        }
        
    private:
        struct timespec     ts_;
        struct tm           tm_;
    };

} // namespace Utility

std::ostream& operator<< ( std::ostream& os, Utility::TimeStamp const& ts )
{
    return ts.output( os );
}

