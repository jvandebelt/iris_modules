//
// Copyright 2010-2012 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INCLUDED_RTL_TYPES_TIME_SPEC_HPP
#define INCLUDED_RTL_TYPES_TIME_SPEC_HPP

#include <boost/config.hpp>
#include <boost/operators.hpp>
#include <ctime>

#if defined(BOOST_MSVC)
    #define RTL_EXPORT         __declspec(dllexport)
    #define RTL_IMPORT         __declspec(dllimport)
    #define RTL_INLINE         __forceinline
#elif defined(__GNUG__) && __GNUG__ >= 4
    #define RTL_EXPORT         __attribute__((visibility("default")))
    #define RTL_IMPORT         __attribute__((visibility("default")))
    #define RTL_INLINE         inline __attribute__((always_inline))
#else
    #define RTL_EXPORT
    #define RTL_IMPORT
    #define RTL_INLINE         inline
#endif

#define RTL_API RTL_IMPORT

namespace rtl{

    /*!
     * A time_spec_t holds a seconds and a fractional seconds time value.
     * Depending upon usage, the time_spec_t can represent absolute times,
     * relative times, or time differences (between absolute times).
     *
     * The time_spec_t provides clock-domain independent time storage,
     * but can convert fractional seconds to/from clock-domain specific units.
     *
     * The fractional seconds are stored as double precision floating point.
     * This gives the fractional seconds enough precision to unambiguously
     * specify a clock-tick/sample-count up to rates of several petahertz.
     */
    class RTL_API time_spec_t : boost::additive<time_spec_t>, boost::totally_ordered<time_spec_t>{
    public:

        /*!
         * Get the system time in time_spec_t format.
         * Uses the highest precision clock available.
         * \return the system time as a time_spec_t
         */
        static time_spec_t get_system_time(void);

        /*!
         * Create a time_spec_t from a real-valued seconds count.
         * \param secs the real-valued seconds count (default = 0)
         */
        time_spec_t(double secs = 0);

        /*!
         * Create a time_spec_t from whole and fractional seconds.
         * \param full_secs the whole/integer seconds count
         * \param frac_secs the fractional seconds count (default = 0)
         */
        time_spec_t(time_t full_secs, double frac_secs = 0);

        /*!
         * Create a time_spec_t from whole seconds and fractional ticks.
         * Translation from clock-domain specific units.
         * \param full_secs the whole/integer seconds count
         * \param tick_count the fractional seconds tick count
         * \param tick_rate the number of ticks per second
         */
        time_spec_t(time_t full_secs, long tick_count, double tick_rate);

        /*!
         * Create a time_spec_t from a 64-bit tick count.
         * Translation from clock-domain specific units.
         * \param ticks an integer count of ticks
         * \param tick_rate the number of ticks per second
         */
        static time_spec_t from_ticks(long long ticks, double tick_rate);

        /*!
         * Convert the fractional seconds to clock ticks.
         * Translation into clock-domain specific units.
         * \param tick_rate the number of ticks per second
         * \return the fractional seconds tick count
         */
        long get_tick_count(double tick_rate) const;

        /*!
         * Convert the time spec into a 64-bit tick count.
         * Translation into clock-domain specific units.
         * \param tick_rate the number of ticks per second
         * \return an integer number of ticks
         */
        long long to_ticks(const double tick_rate) const;

        /*!
         * Get the time as a real-valued seconds count.
         * Note: If this time_spec_t represents an absolute time,
         * the precision of the fractional seconds may be lost.
         * \return the real-valued seconds
         */
        double get_real_secs(void) const;

        /*!
         * Get the whole/integer part of the time in seconds.
         * \return the whole/integer seconds
         */
        time_t get_full_secs(void) const;

        /*!
         * Get the fractional part of the time in seconds.
         * \return the fractional seconds
         */
        double get_frac_secs(void) const;

        //! Implement addable interface
        time_spec_t &operator+=(const time_spec_t &);

        //! Implement subtractable interface
        time_spec_t &operator-=(const time_spec_t &);

    //private time storage details
    private: time_t _full_secs; double _frac_secs;
    };

    //! Implement equality_comparable interface
    RTL_API bool operator==(const time_spec_t &, const time_spec_t &);

    //! Implement less_than_comparable interface
    RTL_API bool operator<(const time_spec_t &, const time_spec_t &);

    RTL_INLINE time_t time_spec_t::get_full_secs(void) const{
        return this->_full_secs;
    }

    RTL_INLINE double time_spec_t::get_frac_secs(void) const{
        return this->_frac_secs;
    }

} //namespace rtl

#endif /* INCLUDED_RTL_TYPES_TIME_SPEC_HPP */


#include <inttypes.h> //imaxdiv, intmax_t

using namespace rtl;

/***********************************************************************
 * Time spec system time
 **********************************************************************/

#ifdef HAVE_CLOCK_GETTIME
#include <time.h>
time_spec_t time_spec_t::get_system_time(void){
    timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return time_spec_t(ts.tv_sec, ts.tv_nsec, 1e9);
}
#endif /* HAVE_CLOCK_GETTIME */


#ifdef HAVE_MACH_ABSOLUTE_TIME
#include <mach/mach_time.h>
time_spec_t time_spec_t::get_system_time(void){
    mach_timebase_info_data_t info; mach_timebase_info(&info);
    intmax_t nanosecs = mach_absolute_time()*info.numer/info.denom;
    return time_spec_t::from_ticks(nanosecs, 1e9);
}
#endif /* HAVE_MACH_ABSOLUTE_TIME */


#ifdef HAVE_QUERY_PERFORMANCE_COUNTER
#include <Windows.h>
time_spec_t time_spec_t::get_system_time(void){
    LARGE_INTEGER counts, freq;
    QueryPerformanceCounter(&counts);
    QueryPerformanceFrequency(&freq);
    return time_spec_t::from_ticks(counts.QuadPart, double(freq.QuadPart));
}
#endif /* HAVE_QUERY_PERFORMANCE_COUNTER */


#ifdef HAVE_MICROSEC_CLOCK
#include <boost/date_time/posix_time/posix_time.hpp>
namespace pt = boost::posix_time;
time_spec_t time_spec_t::get_system_time(void){
    pt::ptime time_now = pt::microsec_clock::universal_time();
    pt::time_duration time_dur = time_now - pt::from_time_t(0);
    return time_spec_t(
        time_t(time_dur.total_seconds()),
        long(time_dur.fractional_seconds()),
        double(pt::time_duration::ticks_per_second())
    );
}
#endif /* HAVE_MICROSEC_CLOCK */

/***********************************************************************
 * Time spec constructors
 **********************************************************************/
#define time_spec_init(full, frac) { \
    const time_t _full = time_t(full); \
    const double _frac = double(frac); \
    const int _frac_int = int(_frac); \
    _full_secs = _full + _frac_int; \
    _frac_secs = _frac - _frac_int; \
    if (_frac_secs < 0) {\
        _full_secs -= 1; \
        _frac_secs += 1; \
    } \
}

RTL_INLINE long long fast_llround(const double x){
    return (long long)(x + 0.5); // assumption of non-negativity
}

time_spec_t::time_spec_t(double secs){
    time_spec_init(0, secs);
}

time_spec_t::time_spec_t(time_t full_secs, double frac_secs){
    time_spec_init(full_secs, frac_secs);
}

time_spec_t::time_spec_t(time_t full_secs, long tick_count, double tick_rate){
    const double frac_secs = tick_count/tick_rate;
    time_spec_init(full_secs, frac_secs);
}

time_spec_t time_spec_t::from_ticks(long long ticks, double tick_rate){
    const imaxdiv_t divres = imaxdiv(ticks, fast_llround(tick_rate));
    return time_spec_t(time_t(divres.quot), double(divres.rem)/tick_rate);
}

/***********************************************************************
 * Time spec accessors
 **********************************************************************/
long time_spec_t::get_tick_count(double tick_rate) const{
    return long(fast_llround(this->get_frac_secs()*tick_rate));
}

long long time_spec_t::to_ticks(double tick_rate) const{
    return fast_llround(this->get_frac_secs()*tick_rate) + \
    (this->get_full_secs() * fast_llround(tick_rate));
}

double time_spec_t::get_real_secs(void) const{
    return this->get_full_secs() + this->get_frac_secs();
}

/***********************************************************************
 * Time spec math overloads
 **********************************************************************/
time_spec_t &time_spec_t::operator+=(const time_spec_t &rhs){
    time_spec_init(
        this->get_full_secs() + rhs.get_full_secs(),
        this->get_frac_secs() + rhs.get_frac_secs()
    );
    return *this;
}

time_spec_t &time_spec_t::operator-=(const time_spec_t &rhs){
    time_spec_init(
        this->get_full_secs() - rhs.get_full_secs(),
        this->get_frac_secs() - rhs.get_frac_secs()
    );
    return *this;
}

bool rtl::operator==(const time_spec_t &lhs, const time_spec_t &rhs){
    return
        lhs.get_full_secs() == rhs.get_full_secs() and
        lhs.get_frac_secs() == rhs.get_frac_secs()
    ;
}

bool rtl::operator<(const time_spec_t &lhs, const time_spec_t &rhs){
    return (
        (lhs.get_full_secs() < rhs.get_full_secs()) or (
        (lhs.get_full_secs() == rhs.get_full_secs()) and
        (lhs.get_frac_secs() < rhs.get_frac_secs())
    ));
}


