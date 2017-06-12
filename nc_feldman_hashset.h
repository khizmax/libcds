#ifndef CDSLIB_INTRUSIVE_IMPL_NC_FELDMAN_HASHSET_H
#define CDSLIB_INTRUSIVE_IMPL_NC_FELDMAN_HASHSET_H

#include <functional>   // std::ref
#include <iterator>     // std::iterator_traits
#include <vector>
#include <iostream>

#include <cds/intrusive/details/nc_feldman_hashset_base.h>
#include <cds/details/allocator.h>
#include <cds/container/iterable_list_hp.h>

namespace cds { namespace intrusive {
    /// Intrusive hash set with solved collisions based on multi-level array
    /**      
        Class similar to the FeldmanHashSet class. 
        The main difference is that this class supports hash collisions by using collision-lists inside.
    
        Unfortnatly, interface is different to the basic FeldmanHashSet. The reason of it, that this class requires 
        references to values instead of hashes (like FeldmanHashSet does). 
        
        Mehtods like 'update()' are not supported. 
        Also unlink function now is simular to erase function, but both of them are supported for the similar interface.
        
       Template parameters:
        - \p GC - safe memory reclamation schema. Can be \p gc::HP, \p gc::DHP or one of \ref cds_urcu_type "RCU type"
        - \p T - a value type to be stored in the set
        - \p Traits - type traits, the structure based on \p feldman_hashset::traits or result of \p feldman_hashset::make_traits metafunction.
            \p Traits is the mandatory argument because it has mandatory types.
            
        There are several specializations of \p %FeldmanHashSet for each \p GC. You should include:
        - <tt><cds/intrusive/feldman_hashset_hp.h></tt> for \p gc::HP garbage collector
        - <tt><cds/intrusive/feldman_hashset_dhp.h></tt> for \p gc::DHP garbage collector
    */
    template <
        class GC
        ,typename T
#ifdef CDS_DOXYGEN_INVOKED
       ,typename Traits = feldman_hashset::traits
#else
       ,typename Traits
#endif
    >
    class FeldmanHashSet: protected feldman_hashset::multilevel_array<T, Traits>
    {
        //@cond
        typedef feldman_hashset::multilevel_array<T, Traits> base_class;
        //@endcond

    public:
        typedef GC      gc;         ///< Garbage collector
        typedef T       value_type; ///< type of value stored in the set
        typedef Traits  traits;     ///< Traits template parameter, see \p feldman_hashset::traits

        typedef typename traits::hash_accessor hash_accessor;   ///< Hash accessor functor
        typedef typename base_class::hash_type hash_type;       ///< Hash type deduced from \p hash_accessor return type
        typedef typename traits::disposer disposer;             ///< data node disposer
        typedef typename base_class::hash_comparator hash_comparator; ///< hash compare functor based on \p traits::compare and \p traits::less options
        typedef typename base_class::list_type list_type;
        typedef typename base_class::list_hash_accessor list_hash_accessor;   
        typedef typename base_class::list_allocator list_allocator; 
        typedef cds::details::Allocator< list_type, list_allocator > cxx_list_allocator;
        
        typedef typename traits::item_counter   item_counter;   ///< Item counter type
        typedef typename traits::node_allocator node_allocator; ///< Array node allocator
        typedef typename traits::memory_model   memory_model;   ///< Memory model
        typedef typename traits::back_off       back_off;       ///< Backoff strategy
        typedef typename traits::stat           stat;           ///< Internal statistics type

        typedef typename traits::value_equal value_equal;     ///< Value equality functor
        static_assert(!std::is_same< value_equal, cds::opt::none >::value, "value_equal functor must be specified");

        typedef typename gc::template guarded_ptr< value_type > guarded_ptr; ///< Guarded pointer

        /// Count of hazard pointers required
        static CDS_CONSTEXPR size_t const c_nHazardPtrCount = 6; //more for inner list

        /// The size of hash_type in bytes, see \p feldman_hashset::traits::hash_size for explanation
        static CDS_CONSTEXPR size_t const c_hash_size = base_class::c_hash_size;

        /// Level statistics
        typedef feldman_hashset::level_statistics level_statistics;
        
        typedef cds::intrusive::IterableList< gc, int,
            typename cds::intrusive::iterable_list::make_traits<
                cds::opt::item_counter< cds::atomicity::item_counter >
            >::type
        > list;

    protected:
        //@cond
        typedef typename base_class::node_ptr node_ptr;
        typedef typename base_class::atomic_node_ptr atomic_node_ptr;
        typedef typename base_class::array_node array_node;
        typedef typename base_class::traverse_data traverse_data;

        using base_class::to_array;
        using base_class::to_node;
        using base_class::stats;
        using base_class::head;
        using base_class::metrics;
        //@endcond

    private:
        //@cond
        item_counter      m_ItemCounter; ///< Item counter
        //@endcond

    public:
        /// Creates empty set
        /**
            @param head_bits - 2<sup>head_bits</sup> specifies the size of head array, minimum is 4.
            @param array_bits - 2<sup>array_bits</sup> specifies the size of array node, minimum is 2.

            Equation for \p head_bits and \p array_bits:
            \code
            sizeof( hash_type ) * 8 == head_bits + N * array_bits
            \endcode
            where \p N is multi-level array depth.
        */
        FeldmanHashSet( size_t head_bits = 8, size_t array_bits = 4 )
            : base_class( head_bits, array_bits )
        {}

        /// Destructs the set and frees all data
        ~FeldmanHashSet()
        {
            clear();
        }

        /// Inserts new node
        /**
            The function inserts \p val in the set if it does not contain
            an item with that hash.

            Returns \p true if \p val is placed into the set, \p false otherwise.
        */
        bool insert( value_type& val )
        {
            return insert( val, []( value_type& ) {} );
        }

        /// Inserts new node
        /**
            This function is intended for derived non-intrusive containers.

            The function allows to split creating of new item into two part:
            - create item with key only
            - insert new item into the set
            - if inserting is success, calls \p f functor to initialize \p val.

            The functor signature is:
            \code
                void func( value_type& val );
            \endcode
            where \p val is the item inserted.

            The user-defined functor is called only if the inserting is success.

            @warning See \ref cds_intrusive_item_creating "insert item troubleshooting".
        */

        template <typename Func>
        bool insert( value_type& val, Func f )
        {            
            hash_type const& hash = hash_accessor()( val );
            traverse_data pos( hash, *this );
            hash_comparator cmp;
            typename gc::template GuardArray<2> guards;

            guards.assign( 1, &val );
            while ( true ) {
                node_ptr slot = base_class::traverse( pos );
                assert( slot.bits() == 0 );
                // protect data node by hazard pointer
                if ( guards.protect( 0, pos.pArr->nodes[pos.nSlot], []( node_ptr p ) -> list_type* { return p.ptr(); }) != slot ) {
                    // slot value has been changed - retry
                    stats().onSlotChanged();
                }
                else if ( slot.ptr()) {
                    if ( !pos.splitter.eos() ) {
                        // the slot must be expanded
                        assert(base_class::expand_slot( pos, slot ));
                    }
                    else {
                        if ( cmp( hash, list_hash_accessor()( *slot.ptr())) == 0 ) {
                            if ( slot.ptr()->insert( val, f )) {
                                ++m_ItemCounter;
                                stats().onInsertSuccess();
                                stats().height( pos.nHeight );
                                gc::template retire<disposer> ( &val ); //now list is responsible for val         
                                return true;
                            }
                            else {    
                                stats().onInsertFailed();
                                return false;
                            }
                        }
                    }
                }
                else {
                    // the slot is empty, try to insert new list
                    node_ptr pNull;
                    
                    list_type * l = cxx_list_allocator().New( );
                    if ( !l->insert( val, f ) ) {
                        cxx_list_allocator().Delete( l ); 
                        stats().onInsertRetry();
                        continue;                                                                
                    }

                    if ( pos.pArr->nodes[pos.nSlot].compare_exchange_strong( pNull, node_ptr( l ), memory_model::memory_order_release, atomics::memory_order_relaxed ))
                    {
                        // the new data node has been inserted
                        ++m_ItemCounter;
                        stats().onInsertSuccess();
                        stats().height( pos.nHeight );
                        gc::template retire<disposer> ( &val ); //now list is responsible for val         
                        return true;
                    }
                    // insert failed - slot has been changed by another thread
                    // retry inserting
                    cxx_list_allocator().Delete( l ); 
                    stats().onInsertRetry();
                    return false;
                } 
            }
            return true;
        }

        /// Unlinks the item \p val from the set
        /**
            The function searches the item \p val in the set and unlink it
            if it is found and its address is equal to <tt>&val</tt>.

            The function returns \p true if success and \p false otherwise.
        */
        bool unlink( value_type const& val )
        {
            typename gc::Guard guard;
            auto pred = [&val]( value_type const& item ) -> bool { return &item == &val; };
            value_type * p = do_erase( val, guard, std::ref( pred ));
            return p != nullptr;
        }

        /// Deletes the item from the set
        /**
            Now absolutly simular to unlink.
        */
        bool erase( value_type const& val )
        {
            return erase( val, []( value_type const& ) {} );
        }

        /// Deletes the item from the set
        /**
            The function searches \p val in the set,
            call \p f functor with item found, and unlinks it from the set.
            The \ref disposer specified in \p Traits is called
            by garbage collector \p GC asynchronously.

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
            typename gc::Guard guard;
            value_type * p = do_erase( val, guard, []( value_type const&) -> bool {return true; } );

            // p is guarded by HP
            if ( p ) {
                f( *p );
                return true;
            }
            return false;
        }

        /// Extracts the item with specified \p value
        /**
            The function searches \p value in the set,
            unlinks it from the set, and returns an guarded pointer to the item extracted.
            If \p value is not found the function returns an empty guarded pointer.

            The \p disposer specified in \p Traits class' template parameter is called automatically
            by garbage collector \p GC when returned \ref guarded_ptr object to be destroyed or released.
            @note Each \p guarded_ptr object uses the GC's guard that can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::FeldmanHashSet< your_template_args > my_set;
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
            typename gc::Guard guard;
            if ( do_erase( val, guard, []( value_type const&) -> bool {return true;} ))
                return guarded_ptr( std::move( guard ));
            return guarded_ptr();
        }

        /// Finds an item by it's \p value
        /**
            The function searches the item by \p value and calls the functor \p f for item found.
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

            The function returns \p true if \p value is found, \p false otherwise.
        */
        template <typename Func>
        bool find( value_type const& val, Func f )
        {
            typename gc::Guard guard;
            value_type * p = search( val, guard );

            // p is guarded by HP
            if ( p ) {
                f( *p );
                return true;
            }
            return false;
        }

        /// Checks whether the set contains \p value
        /**
            The function searches the item by its \p value
            and returns \p true if it is found, or \p false otherwise.
        */
        bool contains( value_type const& val )
        {
            return find( val, []( value_type& ) {} );
        }

        /// Finds an item by it's \p value and returns the item found
        /**
            The function searches the item by its \p value
            and returns the guarded pointer to the item found.
            If \p value is not found the function returns an empty \p guarded_ptr.

            @note Each \p guarded_ptr object uses one GC's guard which can be limited resource.

            Usage:
            \code
            typedef cds::intrusive::FeldmanHashSet< your_template_params >  my_set;
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
            typename gc::Guard guard;
            if ( search( val , guard ))
                return guarded_ptr( std::move( guard ));
            return guarded_ptr();
        }

        /// Clears the set (non-atomic)
        /**
            The function unlink all data node from the set.
            The function is not atomic but is thread-safe.
            After \p %clear() the set may not be empty because another threads may insert items.

            For each item the \p disposer is called after unlinking.
        */
        void clear()
        {
            clear_array( head(), head_size());
        }

        /// Checks if the set is empty
        /**
            Emptiness is checked by item counting: if item count is zero then the set is empty.
            Thus, the correct item counting feature is an important part of the set implementation.
        */
        bool empty() const
        {
            return size() == 0;
        }

        /// Returns item count in the set
        size_t size() const
        {
            return m_ItemCounter;
        }

        /// Returns const reference to internal statistics
        stat const& statistics() const
        {
            return stats();
        }

        /// Returns the size of head node
        using base_class::head_size;

        /// Returns the size of the array node
        using base_class::array_node_size;

        /// Collects tree level statistics into \p stat
        /**
            The function traverses the set and collects statistics for each level of the tree
            into \p feldman_hashset::level_statistics struct. The element of \p stat[i]
            represents statistics for level \p i, level 0 is head array.
            The function is thread-safe and may be called in multi-threaded environment.

            Result can be useful for estimating efficiency of hash functor you use.
        */
        void get_level_statistics( std::vector< feldman_hashset::level_statistics>& stat ) const
        {
            base_class::get_level_statistics( stat );
        }

    private:
        //@cond
        void clear_array( array_node * pArrNode, size_t nSize )
        {
            back_off bkoff;

            for ( atomic_node_ptr * pArr = pArrNode->nodes, *pLast = pArr + nSize; pArr != pLast; ++pArr ) {
                while ( true ) {
                    node_ptr slot = pArr->load( memory_model::memory_order_acquire );
                    if ( slot.bits() == base_class::flag_array_node ) {
                        // array node, go down the tree
                        assert( slot.ptr() != nullptr );
                        clear_array( to_array( slot.ptr()), array_node_size());
                        break;
                    }
                    else if ( slot.bits() == base_class::flag_array_converting ) {
                        // the slot is converting to array node right now
                        while (( slot = pArr->load( memory_model::memory_order_acquire )).bits() == base_class::flag_array_converting ) {
                            bkoff();
                            stats().onSlotConverting();
                        }
                        bkoff.reset();

                        assert( slot.ptr() != nullptr );
                        assert( slot.bits() == base_class::flag_array_node );
                        clear_array( to_array( slot.ptr()), array_node_size());
                        break;
                    }
                    else {
                        // data node
                        if ( pArr->compare_exchange_strong( slot, node_ptr(), memory_model::memory_order_acquire, atomics::memory_order_relaxed )) {
                            if ( slot.ptr()) {
                                for (unsigned int i = 0; i < slot.ptr()->size(); ++i) {
                                    --m_ItemCounter;
                                    stats().onEraseSuccess();
                                }
                                cxx_list_allocator().Delete( slot.ptr() ); // also destructs list                   
                            }
                            break;
                        }
                    }
                }
            }
        }
        //@endcond

    protected:
        //@cond
        value_type * search( value_type const& val, typename gc::Guard& guard )
        {
            typename gc::Guard inner_guard; // we require one more hazard pointer to protect found list (prevent it from moving)
            hash_type const& hash = hash_accessor()( val );
            traverse_data pos( hash, *this );
            hash_comparator cmp;
            value_equal equal;

            while ( true ) {
                node_ptr slot = base_class::traverse( pos );
                assert( slot.bits() == 0 );

                // protect data node by hazard pointer
                if ( inner_guard.protect( pos.pArr->nodes[pos.nSlot], []( node_ptr p ) -> list_type* { return p.ptr(); }) != slot) {
                    // slot value has been changed - retry
                    stats().onSlotChanged();
                    continue;
                }
                else 
                if ( slot.ptr() && cmp( hash, list_hash_accessor()( *slot.ptr())) == 0 ) {
                    // list found
                    for (auto& elem : *slot.ptr())
                        if (equal(elem, val)) {
                            //item found
                            guard.assign( &elem ); //set guard correctly
                            stats().onFindSuccess();
                            return &elem;
                        }
                            
                    stats().onFindFailed();
                    return nullptr;
                }
                stats().onFindFailed();
                return nullptr;
            }
        }

        template <typename Predicate>
        value_type * do_erase( value_type const& val, typename gc::Guard& guard, Predicate pred )
        {
            typename gc::Guard inner_guard; // we need one more guard to protect found list
            hash_type const& hash = hash_accessor()( val );
            traverse_data pos( hash, *this );
            hash_comparator cmp;
            value_equal equal;

            while ( true ) {
                node_ptr slot = base_class::traverse( pos );
                assert( slot.bits() == 0 );
                value_type* ans = nullptr;

                // protect data node by hazard pointer
                if ( inner_guard.protect( pos.pArr->nodes[pos.nSlot], []( node_ptr p ) -> list_type* { return p.ptr(); }) != slot ) {
                    // slot value has been changed - retry
                    stats().onSlotChanged();
                }
                else if ( slot.ptr()) {
                    
                    //we found the right list
                    if ( cmp( hash, list_hash_accessor()( *slot.ptr())) == 0) {
                        //find exact element
                        for (auto& elem : *slot.ptr())
                            if (equal(elem, val) && pred(elem)) {
                                if ( slot.ptr()->erase( val ) ) {
                                    guard.assign( &elem );
                                    stats().onEraseSuccess();
                                    --m_ItemCounter;
                                    ans = &elem;
                                }
                                else {
                                    stats().onEraseRetry();
                                    continue;
                                }                                                                               
                            }
                            
                        //if our list is empty now - delete it    
                        while ( slot.ptr()->empty() ) { //try to delete list while it is empty                             
                            if ( pos.pArr->nodes[pos.nSlot].compare_exchange_strong( slot, node_ptr( nullptr ), memory_model::memory_order_acquire, atomics::memory_order_relaxed)) {
                                cxx_list_allocator().Delete( slot.ptr() );
                                return ans;
                            } 
                        }                  
                        return ans;                       
                    }
                    
                    stats().onEraseFailed();
                    return ans;
                }
                else {
                    // the slot is empty
                    stats().onEraseFailed();
                    return ans;
                }
            }
        }
    };

}} // namespace cds::intrusive

#endif // #ifndef CDSLIB_INTRUSIVE_IMPL_NC_FELDMAN_HASHSET_H
