
#pragma once

namespace XmlSys
{
    template<typename Target>
    class TargetMethods
#ifdef NEVER_DEFINED
    {
        static void label( Target& target, std::string const& label );
        static void item( Target& target, std::string const& item );
        static void no_data( Target& target );
        static void end( Target& target );
    }
#endif
    ;
} // namespace Utility

