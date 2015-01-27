/**************************************************************************

 The MIT License (MIT)

 Copyright (c) 2003 Thatcher Ulrich <tu@tulrich.com>
 Copyright (c) 2015 Dmitry Sovetov

 https://github.com/dmsovetov

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 **************************************************************************/

#include "Common.h"
#include "Function.h"

namespace avm2
{
    bool	s_verbose_action = false;
    bool	s_verbose_parse = false;
    bool	s_use_cached_movie_instance = false;
    bool	s_tag_loaders_registered = false;

#ifndef NDEBUG
    bool	s_verbose_debug = true;
#else
    bool	s_verbose_debug = false;
#endif

    bool get_verbose_parse()
    {
        return s_verbose_parse;
    }

    bool get_verbose_debug()
    {
        return s_verbose_debug;
    }

    bool get_verbose_action()
    {
        return s_verbose_action;
    }

    void	set_verbose_action(bool verbose)
    // Enable/disable log messages re: actions.
    {
        s_verbose_action = verbose;
    }


    void	set_verbose_parse(bool verbose)
    // Enable/disable log messages re: parsing the movie.
    {
        s_verbose_parse = verbose;
    }

    void	as_global_trace(Frame* frame)
    {
        assert(frame->nargs() >= 1);

        Str val;

        for( int i = 0; i < frame->nargs(); i++ ) {
            val += frame->arg(i).asString();

            if( i < (frame->nargs() - 1) ) {
                val += " ";
            }
        }
        
        logger::msg("%s\n", val.c_str());
    }
}
