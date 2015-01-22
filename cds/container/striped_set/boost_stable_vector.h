//$$CDS-header$$

#ifndef CDSLIB_CONTAINER_STRIPED_SET_BOOST_STABLE_VECTOR_ADAPTER_H
#define CDSLIB_CONTAINER_STRIPED_SET_BOOST_STABLE_VECTOR_ADAPTER_H

#include <boost/version.hpp>
#if BOOST_VERSION < 104800
#   error "For boost::container::stable_vector you must use boost 1.48 or above"
#endif

#include <functional>   // ref
#include <algorithm>    // std::lower_bound
#include <utility>      // std::pair
#include <cds/container/striped_set/adapter.h>
#include <boost/container/stable_vector.hpp>

//@cond
namespace cds { namespace container {
    namespace striped_set {

        // Copy policy for boost::container::stable_vector
        template <typename T, typename Alloc>
        struct copy_item_policy< boost::container::stable_vector< T, Alloc > >
        {
            typedef boost::container::stable_vector< T, Alloc > vector_type;
            typedef typename vector_type::iterator iterator;

            void operator()( vector_type& vec, iterator itInsert, iterator itWhat )
            {
                vec.insert( itInsert, *itWhat );
            }
        };

        // Swap policy for boost::container::stable_vector
        template <typename T, typename Alloc>
        struct swap_item_policy< boost::container::stable_vector< T, Alloc > >
        {
            typedef boost::container::stable_vector< T, Alloc > vector_type;
            typedef typename vector_type::iterator iterator;

            void operator()( vector_type& vec, iterator itInsert, iterator itWhat )
            {
                typename vector_type::value_type newVal;
                itInsert = vec.insert( itInsert, newVal );
                std::swap( *itInsert, *itWhat );
            }
        };

        // Move policy for boost::container::stable_vector
        template <typename T, typename Alloc>
        struct move_item_policy< boost::container::stable_vector< T, Alloc > >
        {
            typedef boost::container::stable_vector< T, Alloc > vector_type;
            typedef typename vector_type::iterator iterator;

            void operator()( vector_type& vec, iterator itInsert, iterator itWhat )
            {
                vec.insert( itInsert, std::move( *itWhat ));
            }
        };

    }   // namespace striped_set
}} // namespace cds::container

namespace cds { namespace intrusive { namespace striped_set {
    /// boost::container::stable_vector adapter for hash set bucket
    template <typename T, class Alloc, typename... Options>
    class adapt< boost::container::stable_vector<T, Alloc>, Options... >
    {
    public:
        typedef boost::container::stable_vector<T, Alloc>     container_type          ;   ///< underlying container type

    private:
        /// Adapted container type
        class adapted_container: public cds::container::striped_set::adapted_sequential_container
        {
        public:
            typedef typename container_type::value_type value_type  ;   ///< value type stored in the container
            typedef typename container_type::iterator      iterator ;   ///< container iterator
            typedef typename container_type::const_iterator const_iterator ;    ///< container const iterator

            static bool const has_find_with = true;
            static bool const has_erase_with = true;

        private:
            //@cond
            typedef typename cds::opt::details::make_comparator_from_option_list< value_type, Options... >::type key_comparator;

            typedef typename cds::opt::select<
                typename cds::opt::value<
                    typename cds::opt::find_option<
                        cds::opt::copy_policy< cds::container::striped_set::move_item >
                        , Options...
                    >::type
                >::copy_policy
                , cds::container::striped_set::copy_item, cds::container::striped_set::copy_item_policy<container_type>
                , cds::container::striped_set::swap_item, cds::container::striped_set::swap_item_policy<container_type>
                , cds::container::striped_set::move_item, cds::container::striped_set::move_item_policy<container_type>
            >::type copy_item;

            struct find_predicate
            {
                bool operator()( value_type const& i1, value_type const& i2) const
                {
                    return key_comparator()( i1, i2 ) < 0;
                }

                template <typename Q>
                bool operator()( Q const& i1, value_type const& i2) const
                {
                    return key_comparator()( i1, i2 ) < 0;
                }

                template <typename Q>
                bool operator()( value_type const& i1, Q const& i2) const
                {
                    return key_comparator()( i1, i2 ) < 0;
                }
            };
            //@endcond

        private:
            //@cond
            container_type  m_Vector;
            //@endcond

        public:

            /// Insert value \p val of type \p Q into the container
            /**
                The function allows to split creating of new item into two part:
                - create item with key only from \p val
                - try to insert new item into the container
                - if inserting is success, calls \p f functor to initialize value-field of the new item.

                The functor signature is:
                \code
                    void func( value_type& item );
                \endcode
                where \p item is the item inserted.

                The type \p Q may differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q and constructible from type \p Q,

                The user-defined functor is called only if the inserting is success.
            */
            template <typename Q, typename Func>
            bool insert( const Q& val, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, find_predicate() );
                if ( it == m_Vector.end() || key_comparator()( val, *it ) != 0 ) {
                    value_type newItem( val );
                    it = m_Vector.insert( it, newItem );
                    f( *it );
                    return true;
                }
                return false;
            }

            template <typename... Args>
            bool emplace( Args&&... args )
            {
                value_type val( std::forward<Args>(args)... );
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, find_predicate() );
                if ( it == m_Vector.end() || key_comparator()( val, *it ) != 0 ) {
                    it = m_Vector.emplace( it, std::move( val ) );
                    return true;
                }
                return false;
            }

            /// Ensures that the \p item exists in the container
            /**
                The operation performs inserting or changing data.

                If the \p val key not found in the container, then the new item created from \p val
                is inserted. Otherwise, the functor \p func is called with the item found.
                The \p Func functor has interface:
                \code
                    void func( bool bNew, value_type& item, const Q& val );
                \endcode
                or like a functor:
                \code
                    struct my_functor {
                        void operator()( bool bNew, value_type& item, const Q& val );
                    };
                \endcode

                where arguments are:
                - \p bNew - \p true if the item has been inserted, \p false otherwise
                - \p item - container's item
                - \p val - argument \p val passed into the \p ensure function

                The functor may change non-key fields of the \p item.

                The type \p Q may differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q and constructible from type \p Q,

                Returns <tt> std::pair<bool, bool> </tt> where \p first is true if operation is successfull,
                \p second is true if new item has been added or \p false if the item with \p val key
                already exists.
            */
            template <typename Q, typename Func>
            std::pair<bool, bool> ensure( const Q& val, Func func )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, find_predicate() );
                if ( it == m_Vector.end() || key_comparator()( val, *it ) != 0 ) {
                    // insert new
                    value_type newItem( val );
                    it = m_Vector.insert( it, newItem );
                    func( true, *it, val );
                    return std::make_pair( true, true );
                }
                else {
                    // already exists
                    func( false, *it, val );
                    return std::make_pair( true, false );
                }
            }

            /// Delete \p key
            /**
                The function searches an item with key \p key, calls \p f functor
                and deletes the item. If \p key is not found, the functor is not called.

                The functor \p Func interface is:
                \code
                struct extractor {
                    void operator()(value_type const& val);
                };
                \endcode

                The type \p Q may differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q.

                Return \p true if key is found and deleted, \p false otherwise
            */
            template <typename Q, typename Func>
            bool erase( const Q& key, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), key, find_predicate() );
                if ( it == m_Vector.end() || key_comparator()( key, *it ) != 0 )
                    return false;

                // key exists
                f( *it );
                m_Vector.erase( it );
                return true;
            }

            template <typename Q, typename Less, typename Func>
            bool erase( const Q& key, Less pred, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), key, pred );
                if ( it == m_Vector.end() || pred( key, *it ) || pred( *it, key ) )
                    return false;

                // key exists
                f( *it );
                m_Vector.erase( it );
                return true;
            }

            /// Find the key \p val
            /**
                The function searches the item with key equal to \p val and calls the functor \p f for item found.
                The interface of \p Func functor is:
                \code
                struct functor {
                    void operator()( value_type& item, Q& val );
                };
                \endcode
                where \p item is the item found, \p val is the <tt>find</tt> function argument.

                The functor may change non-key fields of \p item.
                The \p val argument is non-const since it can be used as \p f functor destination i.e., the functor
                may modify both arguments.

                The type \p Q may differ from \ref value_type of items storing in the container.
                Therefore, the \p value_type should be comparable with type \p Q.

                The function returns \p true if \p val is found, \p false otherwise.
            */
            template <typename Q, typename Func>
            bool find( Q& val, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, find_predicate() );
                if ( it == m_Vector.end() || key_comparator()( val, *it ) != 0 )
                    return false;

                // key exists
                f( *it, val );
                return true;
            }

            template <typename Q, typename Less, typename Func>
            bool find( Q& val, Less pred, Func f )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), val, pred );
                if ( it == m_Vector.end() || pred( val, *it ) || pred( *it, val ) )
                    return false;

                // key exists
                f( *it, val );
                return true;
            }

            /// Clears the container
            void clear()
            {
                m_Vector.clear();
            }

            iterator begin()                { return m_Vector.begin(); }
            const_iterator begin() const    { return m_Vector.begin(); }
            iterator end()                  { return m_Vector.end(); }
            const_iterator end() const      { return m_Vector.end(); }

            void move_item( adapted_container& /*from*/, iterator itWhat )
            {
                iterator it = std::lower_bound( m_Vector.begin(), m_Vector.end(), *itWhat, find_predicate() );
                assert( it == m_Vector.end() || key_comparator()( *itWhat, *it ) != 0 );

                copy_item()( m_Vector, it, itWhat );
            }

            size_t size() const
            {
                return m_Vector.size();
            }
        };

    public:
        typedef adapted_container type ; ///< Result of \p adapt metafunction

    };
}}} // namespace cds::intrusive::striped_set


//@endcond

#endif // #ifndef CDSLIB_CONTAINER_STRIPED_SET_BOOST_STABLE_VECTOR_ADAPTER_H
