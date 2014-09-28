//$$CDS-header$$

#ifndef __CDS_DETAILS_FUNCTOR_WRAPPER_H
#define __CDS_DETAILS_FUNCTOR_WRAPPER_H

#include <functional>   // ref

//@cond
namespace cds { namespace details {

    template <typename Functor>
    struct functor_wrapper
    {
    public:
        functor_wrapper()
        {}

        functor_wrapper( Functor /*f*/)
        {}

        Functor get()
        {
            return Functor();
        }
    };

    template <typename Functor>
    struct functor_wrapper<Functor&>
    {
        Functor&    m_func;
    public:
        functor_wrapper( Functor& f)
        : m_func(f)
        {}

        Functor& get()
        {
            return m_func;
        }
    };

    template <typename Functor>
    struct functor_wrapper< std::reference_wrapper<Functor> >
    {
        std::reference_wrapper<Functor>    m_func;
    public:
        functor_wrapper( std::reference_wrapper<Functor> f)
        : m_func(f)
        {}

        Functor& get()
        {
            return m_func.get();
        }
    };

    template <typename Result, typename... Args>
    struct functor_wrapper<Result (*)(Args...)>
    {
        typedef Result (* func_ptr)(Args...);
        typedef Result (& func_ref)(Args...);
        func_ptr m_func;
    public:
        functor_wrapper( func_ptr f )
            : m_func(f)
        {}

        func_ref get()
        {
            assert( m_func );
            return *m_func;
        }
    };
}}  // namespace cds::details
//@endcond

#endif // #ifndef __CDS_DETAILS_FUNCTOR_WRAPPER_H
