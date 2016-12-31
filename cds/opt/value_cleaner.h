/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2017

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CDSLIB_OPT_ITEM_DISPOSER_H
#define CDSLIB_OPT_ITEM_DISPOSER_H

#include <cds/details/defs.h>

namespace cds { namespace opt {

    /// [type-option] value cleaning
    /**
        The cleaner is a functor called when an item is removed from a container.
        Note, the cleaner should not delete (deallocate) the value \p val passed in.
        However, if the \p value_type type is a structure that contains dynamically allocated
        field(s), the cleaning functor may deallocate it and initialize to default value (usually, \p nullptr).

        The interface for type \p value_type is:
        \code
        struct myCleaner {
            void operator ()( value_type& val )
            {
                ...
                // code to cleanup \p val
            }
        }
        \endcode

        Predefined option types:
            \li opt::v::empty_cleaner
    */
    template <typename Type>
    struct value_cleaner {
        //@cond
        template <typename BASE> struct pack: public BASE
        {
            typedef Type value_cleaner;
        };
        //@endcond
    };

    namespace v {

        /// Empty cleaner
        /**
            One of available type for \p opt::value_cleaner option.
            This cleaner is empty, i.e. it does not do any cleaning.
        */
        struct empty_cleaner
        {
            //@cond
            template <typename T>
            void operator()( T& /*val*/ )
            {}
            //@endcond
        };

        /// Cleaner that calls destructor of type \p T
        /**
            One of available type for \p opt::value_cleaner option.
        */
        struct destruct_cleaner
        {
            //@cond
            template <typename T>
            void operator()( T& val )
            {
                (&val)->T::~T();
            }
            //@endcond
        };

    }   // namespace v
}}  // namespace cds::opt

#endif // #ifndef CDSLIB_OPT_ITEM_DISPOSER_H
