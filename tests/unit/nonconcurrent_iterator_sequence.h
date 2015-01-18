/*
    This file is a part of libcds - Concurrent Data Structures library
    Version: 2.0.0

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2014
    Distributed under the BSD license (see accompanying file license.txt)

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
*/

#ifndef __UNIT_NONCONCURRENT_ITERATOR_SEQUENCE_H
#define __UNIT_NONCONCURRENT_ITERATOR_SEQUENCE_H

#include <cds/details/bounded_array.h>
#include <cds/atomic.h>
#include <algorithm>    // std::sort

namespace map { namespace nonconcurrent_iterator {

    class Sequence
    {
    public:
        typedef int        TKey;
        struct TValue {
            TKey        keyControl    ;    // ������������ ��� ��������, ��� ������ ��������� � �����
            int            nShuffle    ;    // ��������� ��������, ������������ ��� ����������
            TValue *    pOrigItem    ;    // ��������� �� ������� � �������; ��� ��� ������ ������ �����,
                                        // �� � ������� ����� ���� ������������ ������ � ��������� ��������

            cds::atomics::item_counter<cds::membar_release>   nAccess        ;    // ������� ������� ��� ������ ������ �� ����������
                                                // (Atomic, ��� ��� �������� ����������� ������)

            TValue()
            {
                ++nConstructCount;
            }
            TValue( const TValue& v )
            {
                memcpy( this, &v, sizeof(*this) );
                ++nConstructCount;
            }
            ~TValue()
            {
                ++nDestructCount;
            }
        };

        struct Data {
            TKey    key;
            TValue    value;
        };

    protected:
        static size_t nConstructCount;
        static size_t nDestructCount;

    public:
        typedef cds::details::BoundedArray<Data>        TDataArray;
        typedef TDataArray::const_iterator              const_iterator;

        TDataArray                                        arrData;

        const_iterator begin() const    { return arrData.begin(); }
        const_iterator end() const      { return arrData.end(); }

    public:
        Sequence( size_t nSize )
            : arrData( nSize )
        {}

        static unsigned int Rand( unsigned int nMax )
        {
            double rnd = double( rand() ) / double( RAND_MAX );
            unsigned int n = (unsigned int) (rnd * nMax);
            return n < nMax ? n : (n-1);
        }

        void generateSequence()
        {
            // ���������� �������� ������ ������. ������ ������ ���� ��������� ��������� �������, �������
            // � �������� �������� ���� value.nShuffle ���������� ���������, � ��������� ������
            // �� ����� ���������� ��������

            size_t nMax = arrData.capacity();
            for ( size_t i = 0; i < nMax; ++i ) {
                arrData[i].key = (unsigned int) i;
                arrData[i].value.keyControl = (unsigned int) i;
                arrData[i].value.nShuffle = Rand( (unsigned int) nMax );
                arrData[i].value.pOrigItem = &(arrData[i].value);
                arrData[i].value.nAccess.reset( cds::membar_relaxed::order );
            }
        }

        void restoreLinks()
        {
            size_t nMax = arrData.capacity();
            for ( size_t i = 0; i < nMax; ++i )
                arrData[i].value.pOrigItem = &(arrData[i].value);
        }

        static bool sortValue( const Data& p1, const Data&p2 )
        {
            return p1.value.nShuffle < p2.value.nShuffle;
        }

        void makeRandomSortedSequence()
        {
            std::sort( arrData.begin(), arrData.end(), sortValue );
            restoreLinks();
        }

        static bool sortAsc( const Data& p1, const Data&p2 )
        {
            return p1.key < p2.key;
        }

        void makeAscSortedSequence()
        {
            // ��������� ������ � ������� ����������� ������
            std::sort( arrData.begin(), arrData.end(), sortAsc );
            restoreLinks();
        }

        static bool sortDesc( const Data& p1, const Data&p2 )
        {
            return p2.key < p1.key;
        }

        void makeDescSortedSequence()
        {
            // ��������� ������ � ������� �������� ������
            std::sort( arrData.begin(), arrData.end(), sortDesc );
            restoreLinks();
        }

        void clearAccess()
        {
            size_t nMax = arrData.capacity();
            for ( size_t i = 0; i < nMax; ++i )
                arrData[i].value.nAccess.reset( cds::membar_relaxed::order );
        }
    };

} } // namespace map::nonconcurrent_iterator

#endif    // #ifndef __UNIT_NONCONCURRENT_ITERATOR_SEQUENCE_H
