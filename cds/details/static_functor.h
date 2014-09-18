//$$CDS-header$$

#ifndef __CDS_DETAILS_STATIC_FUNCTOR_H
#define __CDS_DETAILS_STATIC_FUNCTOR_H

//@cond
namespace cds { namespace details {

    template <class Functor, typename T>
    struct static_functor
    {
        static void call( T * p )
        {
            Functor()( p );
        }
    };

}} // namespace cds::details
//@endcond

#endif // #ifndef __CDS_DETAILS_STATIC_FUNCTOR_H
