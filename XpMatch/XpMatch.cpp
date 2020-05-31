
#include "XpMatch.h"

    int main( int ac, char *av[] )
    {
        try
        {
            XmlSys::XpMatch().run( ac, av );
            return 0;
        }
        catch ( std::exception& e )
        {
            std::cerr << "Exception caught (and exiting): " << e.what() << std::endl; 
            return 1;
        }
    }
    