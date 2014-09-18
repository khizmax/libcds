//$$CDS-header$$

#ifndef __CDS_DETAILS_FUNCTOR_WRAPPER_H
#define __CDS_DETAILS_FUNCTOR_WRAPPER_H

#include <cds/ref.h>

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
    struct functor_wrapper< boost::reference_wrapper<Functor> >
    {
        boost::reference_wrapper<Functor>    m_func;
    public:
        functor_wrapper( boost::reference_wrapper<Functor> f)
        : m_func(f)
        {}

        Functor& get()
        {
            return m_func.get();
        }
    };

#ifdef CDS_CXX11_VARIADIC_TEMPLATE_SUPPORT
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
            assert( m_func != NULL );
            return *m_func;
        }
    };
#else
    template <typename Result>
    struct functor_wrapper<Result (*)()>
    {
        typedef Result (* func_ptr)();
        typedef Result (& func_ref)();
        func_ptr m_func;
    public:
        functor_wrapper( func_ptr f )
            : m_func(f)
        {}

        func_ref get()
        {
            assert( m_func != NULL );
            return *m_func;
        }
    };

    template <typename Result, typename Arg1>
    struct functor_wrapper<Result (*)(Arg1)>
    {
        typedef Result (* func_ptr)(Arg1);
        typedef Result (& func_ref)(Arg1);
        func_ptr m_func;
    public:
        functor_wrapper( func_ptr f )
            : m_func(f)
        {}

        func_ref get()
        {
            assert( m_func != NULL );
            return *m_func;
        }
    };

    template <typename Result, typename Arg1, typename Arg2>
    struct functor_wrapper<Result (*)(Arg1, Arg2)>
    {
        typedef Result (* func_ptr)(Arg1, Arg2);
        typedef Result (& func_ref)(Arg1, Arg2);
        func_ptr m_func;
    public:
        functor_wrapper( func_ptr f )
            : m_func(f)
        {}

        func_ref get()
        {
            assert( m_func != NULL );
            return *m_func;
        }
    };

    template <typename Result, typename Arg1, typename Arg2, typename Arg3>
    struct functor_wrapper<Result (*)(Arg1, Arg2, Arg3)>
    {
        typedef Result (* func_ptr)(Arg1, Arg2, Arg3);
        typedef Result (& func_ref)(Arg1, Arg2, Arg3);
        func_ptr m_func;
    public:
        functor_wrapper( func_ptr f )
            : m_func(f)
        {}

        func_ref get()
        {
            assert( m_func != NULL );
            return *m_func;
        }
    };

#endif
}}  // namespace cds::details
//@endcond

#endif // #ifndef __CDS_DETAILS_FUNCTOR_WRAPPER_H
