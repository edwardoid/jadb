#ifndef JADB_ITERATIVE_FILE_H
#define JADB_ITERATIVE_FILE_H

#include <memory>
#include <math.h>
#include "jadb_file.h"

namespace jadb
{
    template<class EntryT>
    struct SizeInfo
    {
        static size_t size() { return sizeof(EntryT); }
    };

    template<class EntryT>
    class IterativeFile
    {
    public:
        class Iterator
        {
        public:
            Iterator() {}
            Iterator(const Iterator& src) { *this = src; }
            const Iterator& operator = (const Iterator& src);
            
            bool operator == (const Iterator& src) const;
            bool operator != (const Iterator& src) const;
            
            Iterator& operator ++ () { ++m_index; return *this; }
            Iterator operator ++ (int) { return Iterator(*m_file, std::max(m_index, m_index + 1)); }

            Iterator& operator -- () { --m_index; return *this; }
            Iterator operator -- (int) { return Iterator(*m_file, std::min(m_index, m_index - 1)); }


            Iterator operator + (size_t offset) { return Iterator(*m_file, std::max(m_index, m_index + offset)); }
            Iterator& operator += (size_t offset) { m_index = std::max(m_index, m_index + offset); return *this; }
            
            Iterator operator - (size_t offset) { return Iterator(*m_file, m_index - offset); }
            Iterator& operator -= (size_t offset) { m_index = std::min<size_t>(m_index - offset, 0); return *this; }

            EntryT operator*()
            {
                m_file->m_file->seekForRead(absolutePos());
                Serialization is(m_file->m_file);
                EntryT ret;
                is.deserialize(ret);
                return ret;
            }

            void operator = (const EntryT& dt)
            {
                m_file->m_file->seekForWrite(absolutePos());
                Serialization os(m_file->m_file);
                os.serialize(dt);
            }

            size_t relativePos() const { return m_index * m_file->m_portion; }
            size_t absolutePos() const { return relativePos() + m_file->m_offset; }

        protected:
            Iterator(IterativeFile& file, size_t index = 0)
                : m_file(&file), m_index(index) {}
            friend class IterativeFile;
        private:
            size_t m_index = 0;
            IterativeFile* m_file = nullptr;
        };

        IterativeFile(std::shared_ptr<File> file, size_t offset = 0, size_t portion = SizeInfo<EntryT>::size())
            : m_file(file), m_offset(offset), m_portion(portion) {}
        Iterator begin()
        { return Iterator(*this, 0); }
        
        Iterator end()
        { return Iterator(*this, std::ceil((double)m_file->size() / (double)m_portion + 1.)); }
    protected:
        friend class Iterator;
        std::shared_ptr<File> m_file;
        size_t m_offset = 0;
        size_t m_portion = 0;
    };

    template<class EntryT>
    const typename IterativeFile<EntryT>::Iterator& IterativeFile<EntryT>::Iterator::operator = (const Iterator& src)
    {
        m_file = src.m_file;
        m_index = src.m_index;
        return *this;
    }

    template<class EntryT>
    bool IterativeFile<EntryT>::Iterator::operator == (const Iterator& src) const
    {
        return (m_file == src.m_file) && (src.m_index == m_index);
    }

    template<class EntryT>
    bool IterativeFile<EntryT>::Iterator::operator != (const Iterator& src) const
    {
        return !(*this == src);
    }
}

#endif // JADB_ITERATIVE_FILE_H