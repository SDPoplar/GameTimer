#pragma once

#include <string>
#include <vector>

namespace SeaDrip
{
    namespace GameTimer
    {
        template <class T>
        class CursorList
        {
            public:
                void ResetIndex( void );
                void ClearList( void );

                T *GetItem( unsigned int nIndex );
                T *GetCurrentItem( void );

                bool Front( bool cycle = false );
                bool Next( bool cycle = false );
            protected:
                std::vector<T *> m_list;
                unsigned int m_nIndex;
        };

        class NameRecord
        {
            public:
                std::wstring GetItemName( void );
                std::wstring GetComment( void );

            protected:
                std::wstring m_wsName;
                std::wstring m_wsComment;
        };

        class SkillRecord : public NameRecord
        {
            public:
                SkillRecord( int delay, std::wstring name, std::wstring comment );

                int SecondsDiff( int second );

            protected:
                int m_nDelay;
        };

        class DataItem : public CursorList<SkillRecord>, public NameRecord
        {
            public:
                void SetInfo( std::wstring name, std::wstring comment = TEXT( "" ) );
                void AddTimePoint( int delay, std::wstring skillName, std::wstring comment );               
        };

        class DataManager : public CursorList<DataItem>
        {
            public:
                DataManager( void );

                bool DataValid( void );

                bool LoadDataByFileName( std::wstring fileName );
                void ResetCurrentData( void );

                std::wstring GetDataAuthor( void );
                std::wstring GetDataName( void );

                bool IsTicking( void );
                void BeginTick( void );
                void EndTick( void );

                int GetCurrentDelay( void );

            protected:
                std::wstring m_wsDataName;
                std::wstring m_wsDataAuthor;

                bool m_bValid;
                time_t m_timeStartFrom;
        };

        /* CursorList ======================================================================== */
        template<class T>
        T *CursorList<T>::GetItem( unsigned int nIndex )
        {
            return ( nIndex < this->m_list.size() ) ? this->m_list[ nIndex ] : NULL;
        }

        template<class T>
        T *CursorList<T>::GetCurrentItem( void )
        {
            return this->GetItem( this->m_nIndex );
        }

        template<class T>
        void CursorList<T>::ResetIndex( void )
        {
            this->m_nIndex = 0;
        }

        template<class T>
        bool CursorList<T>::Front( bool cycle )
        {
            if( this->m_list.empty() )
            {
                return false;
            }
            if( cycle )
            {
                this->m_nIndex = ( this->m_nIndex + this->m_list.size() - 1 ) % this->m_list.size();
                return true;
            }
            if( this->m_nIndex == 0 )
            {
                return false;
            }
            this->m_nIndex--;
            return true;
        }

        template<class T>
        bool CursorList<T>::Next( bool cycle )
        {
            if( this->m_list.empty() )
            {
                return false;
            }
            unsigned int oldIndex = this->m_nIndex;
            this->m_nIndex = cycle ? ( ( this->m_nIndex + 1 ) % this->m_list.size() ) : ( this->m_nIndex + 1 );
            return cycle ? this->m_nIndex > oldIndex : this->m_nIndex < this->m_list.size();
        }

        template<class T>
        void CursorList<T>::ClearList( void )
        {
            T *p;
            while( !this->m_list.empty() )
            {
                p = this->m_list[ 0 ];
                this->m_list.erase( this->m_list.begin() );
                delete p;
            }
        }

    };
};
