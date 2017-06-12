#ifndef CDSLIB_CONTAINER_IMPL_NC_FELDMAN_HASHSET_H
#define CDSLIB_CONTAINER_IMPL_NC_FELDMAN_HASHSET_H

#include <cds/intrusive/impl/nc_feldman_hashset.h>
#include <cds/container/details/nc_feldman_hashset_base.h>

namespace cds { namespace container {

    /// Hash set based on multi-level array
    /** 
        Similar to FeldmanHashset container. Changes are same as for intrusive versions. Most arguments were changed from hash to value. Some methods were removed.        
        
        Template parameters:
        - \p GC - safe memory reclamation schema. Can be \p gc::HP, \p gc::DHP or one of \ref cds_urcu_type "RCU type"
        - \p T - a value type to be stored in the set
        - \p Traits - type traits, the structure based on \p feldman_hashset::traits or result of \p feldman_hashset::make_traits metafunction.
            \p Traits is the mandatory argument because it has one mandatory type - an @ref feldman_hashset::traits::hash_accessor "accessor"
            to hash value of \p T. The set algorithm does not calculate that hash value.

        There are several specializations of \p %FeldmanHashSet for each \p GC. You should include:
        - <tt><cds/container/feldman_hashset_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/container/feldman_hashset_dhp.h></tt> for \p gc::DHP garbage collector
    */
    template <
        class GC
        , typename T
#ifdef CDS_DOXYGEN_INVOKED
        , class Traits = feldman_hashset::traits
#else
        , class Traits
#endif
    >
    class FeldmanHashSet
#ifdef CDS_DOXYGEN_INVOKED
        : protected cds::intrusive::FeldmanHashSet< GC, T, Traits >
#else
        : protected cds::container::details::make_feldman_hashset< GC, T, Traits >::type
#endif
    {
        //@cond
        typedef cds::container::details::make_feldman_hashset< GC, T, Traits > maker;
        typedef typename maker::type base_class;
        //@endcond

    public:
        typedef GC      gc;         ///< Garbage collector
        typedef T       value_type; ///< type of value stored in the set
        typedef Traits  traits;     ///< Traits template parameter, see \p feldman_hashset::traits

        typedef typename base_class::hash_accessor hash_accessor; ///< Hash accessor functor
        typedef typename base_class::hash_type hash_type; ///< Hash type deduced from \p hash_accessor return type
        typedef typename base_class::hash_comparator hash_comparator; ///< hash compare functor based on \p opt::compare and \p opt::less option setter

        typedef typename traits::item_counter   item_counter;   ///< Item counter type
        typedef typename traits::allocator      allocator;      ///< Element allocator
        typedef typename traits::node_allocator node_allocator; ///< Array node allocator
        typedef typename traits::memory_model   memory_model;   ///< Memory model
        typedef typename traits::back_off       back_off;       ///< Backoff strategy
        typedef typename traits::stat           stat;           ///< Internal statistics type

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

        /// Count of hazard pointers required
        static CDS_CONSTEXPR size_t const c_nHazardPtrCount = base_class::c_nHazardPtrCount;

        /// The size of \p hash_type in bytes, see \p feldman_hashset::traits::hash_size for explanation
        static CDS_CONSTEXPR size_t const c_hash_size = base_class::c_hash_size;

        /// Level statistics
        typedef feldman_hashset::level_statistics level_statistics;

    protected:
        //@cond
        typedef typename maker::cxx_node_allocator cxx_node_allocator;
        typedef std::unique_ptr< value_type, typename maker::node_disposer > scoped_node_ptr;
        //@endcond

    public:
        /// Creates empty set
        /**
            @param head_bits - 2<sup>head_bits</sup> specifies the size of head array, minimum is 4.
            @param array_bits - 2<sup>array_bits</sup> specifies the size of array node, minimum is 2.

            Equation for \p head_bits and \p array_bits:
            \code
            sizeof(hash_type) * 8 == head_bits + N * array_bits
            \endcode
            where \p N is multi-level array depth.
        */
        FeldmanHashSet( size_t head_bits = 8, size_t array_bits = 4 )
            : base_class( head_bits, array_bits )
        {}

        /// Destructs the set and frees all data
        ~FeldmanHashSet()
        {}

        /// Inserts new element
        /**
            The function creates an element with copy of \p val value and then inserts it into the set.

            The type \p Q should contain as minimum the complete hash for the element.
            The object of \ref value_type should be constructible from a value of type \p Q.
            In trivial case, \p Q is equal to \ref value_type.

            Returns \p true if \p val is inserted into the set, \p false otherwise.
        */
        template <typename Q>
        bool insert( Q const& val )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( val ));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Inserts new element
        /**
            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls \p f functor to initialize value-fields of \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted. User-defined functor \p f should guarantee that during changing
            \p val no any other changes could be made on this set's item by concurrent threads.
            The user-defined functor is called only if the inserting is success.
        */
        template <typename Q, typename Func>
        bool insert( Q const& val, Func f )
        {
            scoped_node_ptr sp( cxx_node_allocator().New( val ));
            if ( base_class::insert( *sp, f )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Inserts data of type \p value_type created in-place from <tt>std::forward<Args>(args)...</tt>
        /**
            Returns \p true if inserting successful, \p false otherwise.
        */
        template <typename... Args>
        bool emplace( Args&&... args )
        {
            scoped_node_ptr sp( cxx_node_allocator().MoveNew( std::forward<Args>(args)... ));
            if ( base_class::insert( *sp )) {
                sp.release();
                return true;
            }
            return false;
        }

        /// Deletes the item from the set
        /**
            The function searches \p hash in the set,
            deletes the item found, and returns \p true.
            If that item is not found the function returns \p false.
        */
        bool erase( value_type const& val )
        {
            return base_class::erase( val );
        }

        /// Deletes the item from the set
        /**
            The function searches \p hash in the set,
            call \p f functor with item found, and deltes the element from the set.

            The \p Func interface is
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode

            If \p hash is not found the function returns \p false.
        */
        template <typename Func>
        bool erase( value_type const& val, Func f )
        {
            return base_class::erase( val, f );
        }

        /// Extracts the item with specified \p hash
        /**
            The function searches \p hash in the set,
            unlinks it from the set, and returns a guarded pointer to the item extracted.
            If \p hash is not found the function returns an empty guarded pointer.

            The item returned is reclaimed by garbage collector \p GC
            when returned \ref guarded_ptr object to be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::container::FeldmanHashSet< your_template_args > my_set;
            my_set theSet;
            // ...
            {
                my_set::guarded_ptr gp( theSet.extract( 5 ));
                if ( gp ) {
                    // Deal with gp
                    // ...
                }
                // Destructor of gp releases internal HP guard
            }
            \endcode
        */
        guarded_ptr extract( value_type const& val )
        {
            return base_class::extract( val );
        }

        /// Finds an item by it's \p hash
        /**
            The function searches the item by \p hash and calls the functor \p f for item found.
            The interface of \p Func functor is:
            \code
            struct functor {
                void operator()( value_type& item );
            };
            \endcode
            where \p item is the item found.

            The functor may change non-key fields of \p item. Note that the functor is only guarantee
            that \p item cannot be disposed during the functor is executing.
            The functor does not serialize simultaneous access to the set's \p item. If such access is
            possible you must provide your own synchronization schema on item level to prevent unsafe item modifications.

            The function returns \p true if \p hash is found, \p false otherwise.
        */
        template <typename Func>
        bool find( value_type const& val, Func f )
        {
            return base_class::find( val, f );
        }

        /// Checks whether the set contains \p hash
        /**
            The function searches the item by its \p hash
            and returns \p true if it is found, or \p false otherwise.
        */
        bool contains( value_type const& val )
        {
            return base_class::contains( val );
        }

        /// Finds an item by it's \p hash and returns the item found
        /**
            The function searches the item by its \p hash
            and returns the guarded pointer to the item found.
            If \p hash is not found the function returns an empty \p guarded_ptr.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::container::FeldmanHashSet< your_template_params >  my_set;
            my_set theSet;
            // ...
            {
                my_set::guarded_ptr gp( theSet.get( 5 ));
                if ( theSet.get( 5 )) {
                    // Deal with gp
                    //...
                }
                // Destructor of guarded_ptr releases internal HP guard
            }
            \endcode
        */
        guarded_ptr get( value_type const& val )
        {
            return base_class::get( val );
        }

        /// Clears the set (non-atomic)
        /**
            The function unlink all data node from the set.
            The function is not atomic but is thread-safe.
            After \p %clear() the set may not be empty because another threads may insert items.
        */
        void clear()
        {
            base_class::clear();
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
            Thus, the correct item counting feature is an important part of the set implementation.
        */
        bool empty() const
        {
            return base_class::empty();
        }

        /// Returns item count in the set
        size_t size() const
        {
            return base_class::size();
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return base_class::statistics();
        }

        /// Returns the size of head node
        size_t head_size() const
        {
            return base_class::head_size();
        }

        /// Returns the size of the array node
        size_t array_node_size() const
        {
            return base_class::array_node_size();
        }

        /// Collects tree level statistics into \p stat
        /**
            The function traverses the set and collects statistics for each level of the tree
            into \p feldman_hashset::level_statistics struct. The element of \p stat[i]
            represents statistics for level \p i, level 0 is head array.
            The function is thread-safe and may be called in multi-threaded environment.

            Result can be useful for estimating efficiency of hash functor you use.
        */
        void get_level_statistics(std::vector< feldman_hashset::level_statistics>& stat) const
        {
            base_class::get_level_statistics(stat);
        }
    };

}} // namespace cds::container

#endif // #ifndef CDSLIB_CONTAINER_IMPL_NC_FELDMAN_HASHSET_H
