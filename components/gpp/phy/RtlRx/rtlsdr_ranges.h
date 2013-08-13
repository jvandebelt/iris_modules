//
// Copyright 2010-2011 Ettus Research LLC
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

#ifndef RTLSDR_RANGES
#define RTLSDR_RANGES

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <stdexcept>
#include <boost/math/special_functions/round.hpp>
#include <boost/foreach.hpp>
#include <algorithm>
#include <sstream>

#define RTLSDR_PIMPL_DECL(_name) \
    struct _name; boost::shared_ptr<_name>


#define RTLSDR_PIMPL_MAKE(_name, _args) \
    boost::shared_ptr<_name>(new _name _args)

namespace rtl{
    /*!
     * A range object describes a set of discrete values of the form:
     * y = start + step*n, where n is an integer between 0 and (stop - start)/step
     */
    class range_t{
    public:

        /*!
         * Create a range from a single value.
         * The step size will be taken as zero.
         * \param value the only possible value in this range
         */
        range_t(double value = 0);

        /*!
         * Create a range from a full set of values.
         * A step size of zero implies infinite precision.
         * \param start the minimum value for this range
         * \param stop the maximum value for this range
         * \param step the step size for this range
         */
        range_t(double start, double stop, double step = 0);

        //! Get the start value for this range.
        double start(void) const;

        //! Get the stop value for this range.
        double stop(void) const;

        //! Get the step value for this range.
        double step(void) const;

        //! Convert this range to a printable string
        const std::string to_pp_string(void) const;

    private: RTLSDR_PIMPL_DECL(impl) _impl;
    };

    /*!
     * A meta-range object holds a list of individual ranges.
     */
    struct meta_range_t : std::vector<range_t>{

        //! A default constructor for an empty meta-range
        meta_range_t(void);

        /*!
         * Input iterator constructor:
         * Makes boost::assign::list_of work.
         * \param first the begin iterator
         * \param last the end iterator
         */
        template <typename InputIterator>
        meta_range_t(InputIterator first, InputIterator last):
            std::vector<range_t>(first, last){ /* NOP */ }

        /*!
         * A convenience constructor for a single range.
         * A step size of zero implies infinite precision.
         * \param start the minimum value for this range
         * \param stop the maximum value for this range
         * \param step the step size for this range
         */
        meta_range_t(double start, double stop, double step = 0);

        //! Get the overall start value for this meta-range.
        double start(void) const;

        //! Get the overall stop value for this meta-range.
        double stop(void) const;

        //! Get the overall step value for this meta-range.
        double step(void) const;

        /*!
         * Clip the target value to a possible range value.
         * \param value the value to clip to this range
         * \param clip_step if true, clip to steps as well
         * \return a value that is in one of the ranges
         */
        double clip(double value, bool clip_step = false) const;

        //! Convert this meta-range to a printable string
        const std::string to_pp_string(void) const;

    };

    typedef meta_range_t gain_range_t;
    typedef meta_range_t freq_range_t;

}      /* namespace rtl */

using namespace rtl;

/***********************************************************************
 * range_t implementation code
 **********************************************************************/
struct range_t::impl{
    impl(double start, double stop, double step):
        start(start), stop(stop), step(step)
    {
        /* NOP */
    }
    double start, stop, step;
};

range_t::range_t(double value):
    _impl(RTLSDR_PIMPL_MAKE(impl, (value, value, 0)))
{
    /* NOP */
}

range_t::range_t(
    double start, double stop, double step
):
    _impl(RTLSDR_PIMPL_MAKE(impl, (start, stop, step)))
{
    if (stop < start){
        throw std::runtime_error("cannot make range where stop < start");
    }
}

double range_t::start(void) const{
    return _impl->start;
}

double range_t::stop(void) const{
    return _impl->stop;
}

double range_t::step(void) const{
    return _impl->step;
}

const std::string range_t::to_pp_string(void) const{
    std::stringstream ss;
    ss << "(" << this->start();
    if (this->start() != this->stop()) ss << ", " << this->stop();
    if (this->step() != 0) ss << ", " << this->step();
    ss << ")";
    return ss.str();
}

/***********************************************************************
 * meta_range_t implementation code
 **********************************************************************/
void check_meta_range_monotonic(const meta_range_t &mr){
    if (mr.empty()){
        throw std::runtime_error("meta-range cannot be empty");
    }
    for (size_t i = 1; i < mr.size(); i++){
        if (mr.at(i).start() < mr.at(i-1).stop()){
            throw std::runtime_error("meta-range is not monotonic");
        }
    }
}

meta_range_t::meta_range_t(void){
    /* NOP */
}

meta_range_t::meta_range_t(
    double start, double stop, double step
):
    std::vector<range_t > (1, range_t(start, stop, step))
{
    /* NOP */
}

double meta_range_t::start(void) const{
    check_meta_range_monotonic(*this);
    double min_start = this->front().start();
    BOOST_FOREACH(const range_t &r, (*this)){
        min_start = std::min(min_start, r.start());
    }
    return min_start;
}

double meta_range_t::stop(void) const{
    check_meta_range_monotonic(*this);
    double max_stop = this->front().stop();
    BOOST_FOREACH(const range_t &r, (*this)){
        max_stop = std::max(max_stop, r.stop());
    }
    return max_stop;
}

double meta_range_t::step(void) const{
    check_meta_range_monotonic(*this);
    std::vector<double> non_zero_steps;
    range_t last = this->front();
    BOOST_FOREACH(const range_t &r, (*this)){
        //steps at each range
        if (r.step() > 0) non_zero_steps.push_back(r.step());
        //and steps in-between ranges
        double ibtw_step = r.start() - last.stop();
        if (ibtw_step > 0) non_zero_steps.push_back(ibtw_step);
        //store ref to last
        last = r;
    }
    if (non_zero_steps.empty()) return 0; //all zero steps, its zero...
    return *std::min_element(non_zero_steps.begin(), non_zero_steps.end());
}

double meta_range_t::clip(double value, bool clip_step) const{
    check_meta_range_monotonic(*this);
    double last_stop = this->front().stop();
    BOOST_FOREACH(const range_t &r, (*this)){
        //in-between ranges, clip to nearest
        if (value < r.start()){
            return (std::abs(value - r.start()) < std::abs(value - last_stop))?
                r.start() : last_stop;
        }
        //in this range, clip here
        if (value <= r.stop()){
            if (! clip_step || r.step() == 0) return value;
            return boost::math::round((value - r.start())/r.step())*r.step() + r.start();
        }
        //continue on to the next range
        last_stop = r.stop();
    }
    return last_stop;
}

const std::string meta_range_t::to_pp_string(void) const{
    std::stringstream ss;
    BOOST_FOREACH(const range_t &r, (*this)){
        ss << r.to_pp_string() << std::endl;
    }
    return ss.str();
}


#endif /* RTLSDR_RANGES */
