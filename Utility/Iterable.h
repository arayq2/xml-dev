
#pragma once

#include <iterator>

namespace Utility
{
    template<typename Wrappee>
    class Iterable
    : public std::iterator<std::output_iterator_tag, void, void, void, void >
    {
    public:
        Iterable(Wrappee& wrappee)
        : wrappee_(wrappee)
        {}
        
        Iterable& operator* () { return *this; }
        Iterable& operator++ () { return *this; }
        Iterable& operator++ (int) { return *this; }
        
        template<typename T>
        Iterable& operator= ( T& item ) 
        {
            wrappee_( item );
            return *this; 
        }
        
    private:
        Wrappee&    wrappee_;
    };

} // namespace Utility
