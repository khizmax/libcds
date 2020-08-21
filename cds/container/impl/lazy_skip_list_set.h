#ifndef CDSLIB_CONTAINER_IMPL_LAZY_SKIP_LIST_SET_H
#define CDSLIB_CONTAINER_IMPL_LAZY_SKIP_LIST_SET_H

#include <cds/details/binary_functor_wrapper.h>
#include <cds/container/details/guarded_ptr_cast.h>

namespace cds { namespace container {

    template <
        typename GC,
        typename T,
#ifdef CDS_DOXYGEN_INVOKED
        typename Traits = lazy_skip_list::traits
#else
        typename Traits
#endif
    >
    class LazySkipListSet:
#ifdef CDS_DOXYGEN_INVOKED
        protected intrusive::LazySkipListSet< GC, T, Traits >
#else
        protected details::make_lazy_skip_list_set< GC, T, Traits >::type
#endif
    {
        typedef details::make_lazy_skip_list_set< GC, T, Traits > maker;
        typedef typename maker::type base_class;

    public:
        typedef GC     gc;
        typedef T      value_type;
        typedef Traits traits;

        typedef typename base_class::back_off     back_off;
        typedef typename traits::allocator        allocator_type;
        typedef typename base_class::item_counter item_counter;
        typedef typename maker::key_comparator    key_comparator;
        typedef typename base_class::memory_model memory_model;
        typedef typename traits::random_level_generator random_level_generator;
        typedef typename traits::stat             stat;

        static size_t const c_nHazardPtrCount = base_class::c_nHazardPtrCount;

    protected:
        typedef typename maker::node_type           node_type;
        typedef typename maker::node_allocator      node_allocator;

        typedef std::unique_ptr< node_type, typename maker::node_deallocator >    scoped_node_ptr;

    public:
        typedef typename gc::template guarded_ptr< node_type, value_type, details::guarded_ptr_cast_set<node_type, value_type> > guarded_ptr;

    protected:
        unsigned int random_level()
        {
            return base_class::random_level();
        }

    public:
        LazySkipListSet()
            : base_class()
        {}

        ~LazySkipListSet()
        {}

    public:
        typedef lazy_skip_list::details::iterator< typename base_class::iterator >  iterator;
        typedef lazy_skip_list::details::iterator< typename base_class::const_iterator >   const_iterator;

        iterator begin()
        {
            return iterator( base_class::begin());
        }

        const_iterator begin() const
        {
            return const_iterator( base_class::begin());
        }

        const_iterator cbegin() const
        {
            return const_iterator( base_class::cbegin());
        }

        iterator end()
        {
            return iterator( base_class::end());
        }

        const_iterator end() const
        {
            return const_iterator( base_class::end());
        }

        const_iterator cend() const
        {
            return const_iterator( base_class::cend());
        }

    public:
        template <typename Q>
        bool insert( Q const& val )
        {
            scoped_node_ptr sp( node_allocator().New( random_level(), val ));
            if ( base_class::insert( *sp.get())) {
                sp.release();
                return true;
            }
            return false;
        }

        template <typename Q, typename Func>
        bool insert( Q const& val, Func f )
        {
            scoped_node_ptr sp( node_allocator().New( random_level(), val ));
            if ( base_class::insert( *sp.get(), [&f]( node_type& v ) { f( v.m_Value ); } )) {
                sp.release();
                return true;
            }
            return false;
        }

        template <typename Q, typename Func>
        std::pair<bool, bool> update( const Q& val, Func func, bool bInsert = true )
        {
            scoped_node_ptr sp( node_allocator().New( random_level(), val ));
            std::pair<bool, bool> bRes = base_class::update( *sp,
                [&func, &val](bool bNew, node_type& node, node_type&){ func( bNew, node.m_Value, val ); },
                bInsert );
            if ( bRes.first && bRes.second )
                sp.release();
            return bRes;
        }

        template <typename Q, typename Func>
        CDS_DEPRECATED("ensure() is deprecated, use update()")
        std::pair<bool, bool> ensure( const Q& val, Func func )
        {
            return update( val, func, true );
        }

        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr sp( node_allocator().New( random_level(), std::forward<Args>(args)... ));
            if ( base_class::insert( *sp.get())) {
                sp.release();
                return true;
            }
            return false;
        }

        template <typename Q>
        bool erase( Q const& key )
        {
            return base_class::erase( key );
        }

        template <typename Q, typename Less>
        bool erase_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >());
        }

        template <typename Q, typename Func>
        bool erase( Q const& key, Func f )
        {
            return base_class::erase( key, [&f]( node_type const& node) { f( node.m_Value ); } );
        }

        template <typename Q, typename Less, typename Func>
        bool erase_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::erase_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >(),
                [&f]( node_type const& node) { f( node.m_Value ); } );
        }

        template <typename Q>
        guarded_ptr extract( Q const& key )
        {
            return base_class::extract_( key, typename base_class::key_comparator());
        }

        template <typename Q, typename Less>
        guarded_ptr extract_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            typedef cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >  wrapped_less;
            return base_class::extract_( key, cds::opt::details::make_comparator_from_less<wrapped_less>());
        }

        guarded_ptr extract_min()
        {
            return base_class::extract_min_();
        }

        guarded_ptr extract_max()
        {
            return base_class::extract_max_();
        }

        template <typename Q, typename Func>
        bool find( Q& key, Func f )
        {
            return base_class::find( key, [&f]( node_type& node, Q& v ) { f( node.m_Value, v ); });
        }

        template <typename Q, typename Func>
        bool find( Q const& key, Func f )
        {
            return base_class::find( key, [&f]( node_type& node, Q& v ) { f( node.m_Value, v ); } );
        }

        template <typename Q, typename Less, typename Func>
        bool find_with( Q& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >(),
                [&f]( node_type& node, Q& v ) { f( node.m_Value, v ); } );
        }

        template <typename Q, typename Less, typename Func>
        bool find_with( Q const& key, Less pred, Func f )
        {
            CDS_UNUSED( pred );
            return base_class::find_with( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >(),
                                          [&f]( node_type& node, Q const& v ) { f( node.m_Value, v ); } );
        }

        template <typename Q>
        bool contains( Q const& key )
        {
            return base_class::contains( key );
        }

        template <typename Q>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find( Q const& key )
        {
            return contains( key );
        }

        template <typename Q, typename Less>
        bool contains( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            return base_class::contains( key, cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >());
        }

        template <typename Q, typename Less>
        CDS_DEPRECATED("deprecated, use contains()")
        bool find_with( Q const& key, Less pred )
        {
            return contains( key, pred );
        }

        template <typename Q>
        guarded_ptr get( Q const& key )
        {
            return base_class::get_with_( key, typename base_class::key_comparator());
        }

        template <typename Q, typename Less>
        guarded_ptr get_with( Q const& key, Less pred )
        {
            CDS_UNUSED( pred );
            typedef cds::details::predicate_wrapper< node_type, Less, typename maker::value_accessor >  wrapped_less;
            return base_class::get_with_( key, cds::opt::details::make_comparator_from_less< wrapped_less >());
        }

        void clear()
        {
            base_class::clear();
        }

        bool empty() const
        {
            return base_class::empty();
        }

        size_t size() const
        {
            return base_class::size();
        }

        stat const& statistics() const
        {
            return base_class::statistics();
        }
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_IMPL_LAZY_SKIP_LIST_SET_H
