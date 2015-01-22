//$$CDS-header$$

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
            One of available type for opt::value_cleaner option.
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
            One of available type for opt::value_cleaner option.
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
