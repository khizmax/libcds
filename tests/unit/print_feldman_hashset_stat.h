//$$CDS-header$$

#ifndef CDSUNIT_PRINT_FELDMAN_HASHSET_STAT_H
#define CDSUNIT_PRINT_FELDMAN_HASHSET_STAT_H

#include <cds/intrusive/details/feldman_hashset_base.h>
#include <ostream>

namespace std {

    static inline ostream& operator <<( ostream& o, cds::intrusive::feldman_hashset::stat<> const& s )
    {
        return
        o << "Stat [cds::intrusive::feldman_hashset::stat]\n"
            << "\t\t          m_nInsertSuccess: " << s.m_nInsertSuccess.get()           << "\n"
            << "\t\t           m_nInsertFailed: " << s.m_nInsertFailed.get()            << "\n"
            << "\t\t            m_nInsertRetry: " << s.m_nInsertRetry.get()             << "\n"
            << "\t\t              m_nUpdateNew: " << s.m_nUpdateNew.get()               << "\n"
            << "\t\t         m_nUpdateExisting: " << s.m_nUpdateExisting.get()          << "\n"
            << "\t\t           m_nUpdateFailed: " << s.m_nUpdateFailed.get()            << "\n"
            << "\t\t            m_nUpdateRetry: " << s.m_nUpdateRetry.get()             << "\n"
            << "\t\t           m_nEraseSuccess: " << s.m_nEraseSuccess.get()            << "\n"
            << "\t\t            m_nEraseFailed: " << s.m_nEraseFailed.get()             << "\n"
            << "\t\t             m_nEraseRetry: " << s.m_nEraseRetry.get()              << "\n"
            << "\t\t            m_nFindSuccess: " << s.m_nFindSuccess.get()             << "\n"
            << "\t\t             m_nFindFailed: " << s.m_nFindFailed.get()              << "\n"
            << "\t\t      m_nExpandNodeSuccess: " << s.m_nExpandNodeSuccess.get()       << "\n"
            << "\t\t       m_nExpandNodeFailed: " << s.m_nExpandNodeFailed.get()        << "\n"
            << "\t\t            m_nSlotChanged: " << s.m_nSlotChanged.get()             << "\n"
            << "\t\t         m_nSlotConverting: " << s.m_nSlotConverting.get()          << "\n"
            << "\t\t         m_nArrayNodeCount: " << s.m_nArrayNodeCount.get()          << "\n"
            << "\t\t                 m_nHeight: " << s.m_nHeight.get()                  << "\n";
    }

    static inline ostream& operator <<( ostream& o, cds::intrusive::feldman_hashset::empty_stat const& /*s*/ )
    {
        return o;
    }

    static inline ostream& operator<<( ostream& o, std::vector< cds::intrusive::feldman_hashset::level_statistics > const& level_stat )
    {
        o << "Level statistics, height=" << level_stat.size() << "\n";
        size_t i = 0;
        o << "  i   node_count capacity    data_cell   array_cell   empty_cell\n";
        for ( auto it = level_stat.begin(); it != level_stat.end(); ++it, ++i ) {
            o << std::setw( 3 ) << i << std::setw( 0 ) << " "
              << std::setw( 12 ) << it->array_node_count << std::setw( 0 ) << " "
              << std::setw( 8 ) << it->node_capacity << std::setw( 0 ) << " "
              << std::setw( 12 ) << it->data_cell_count << std::setw( 0 ) << " "
              << std::setw( 12 ) << it->array_cell_count << std::setw( 0 ) << " "
              << std::setw( 12 ) << it->empty_cell_count << std::setw( 0 )
              << "\n";
        }
        return o;
    }

} // namespace std

#endif // #ifndef CDSUNIT_PRINT_FELDMAN_HASHSET_STAT_H
