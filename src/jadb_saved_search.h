#ifndef JADB_SAVED_SEARCH_H
#define JADB_SAVED_SEARCH_H

#include "jadb_record.h"

namespace jadb {
    class SavedSearch
    {
    public:
        SavedSearch() {}
        virtual ~SavedSearch() {}
        void update(const Record& record) {}
    private:

    }
}

#endif // JADB_SAVED_SEARCH_H