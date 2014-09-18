//$$CDS-header$$

#ifndef __CDS_REF_H
#define __CDS_REF_H

#include <cds/details/defs.h>
#include <boost/ref.hpp>

//@cond
namespace cds {

    using boost::ref;

    namespace details {
        template <typename T>
        struct unwrap_reference
        {
            typedef T type;
            static T apply(T f)
            {
                return f;
            }
        };

        template <typename T>
        struct unwrap_reference<T&>
        {
            typedef T& type;
            static T& apply( T& f )
            {
                return f;
            }
        };

        template <typename T>
        struct unwrap_reference<boost::reference_wrapper<T> >
        {
            typedef T& type;
            static T& apply( boost::reference_wrapper<T> f )
            {
                return f.get();
            }
        };
    }

    template <typename T>
    static inline typename details::unwrap_reference<T>::type unref( T f )
    {
        return cds::details::unwrap_reference<T>::apply(f);
    }
}
//@endcond

#endif // #ifndef __CDS_REF_H
