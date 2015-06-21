
#pragma once

#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>

namespace Utility
{
    namespace
    {
        struct LogOnly
        {
            void on_warning( std::string const& msg )
            {
                std::cerr << msg << std::endl;
            }
        };
    } // namespace
    
    class NameMaker
    {
    public:
        typedef std::function<std::string const(std::string const&, std::string const&, std::string const&)> Munger;
        explicit
        NameMaker(Munger const& munger = Simple())
        : prefix_("./")
        , separator_("/")
        , suffix_("")
        , munger_(munger)
        {}
        
        NameMaker& prefix( std::string const& value ) { prefix_.assign( value ); return *this; }
        NameMaker& separator( std::string const& value ) { separator_.assign( value ); return *this; }
        NameMaker& suffix( std::string const& value ) { suffix_.assign( value ); return *this; }
        NameMaker& munger( Munger const& value ) { munger_ = value; return *this; }
        
        NameMaker& normalize()
        {
            if ( separator_.length() > 0 )
            {
                size_t      _pos(prefix_.rfind( separator_ ));
                
                if ( _pos != (prefix_.length() - separator_.length()) )
                {
                    prefix_ += separator_;
                }
            }
            return *this;
        }
        
        NameMaker& normalize( std::string const& directory )
        {
            if ( directory.length() > 0 )
            {
                return prefix( directory ).normalize();
            }
            else
            {
                return prefix( "" );
            }
        }
        
        std::string const operator() ( std::string const& key ) const
        {
            return munger_( prefix_, key, suffix_ );
        }
        
    private:
        struct Simple
        {
            std::string const operator() ( std::string const& prefix, 
                std::string const& key, std::string const& suffix ) const
            {
                return prefix + key + suffix;
            }
        };
        
        std::string     prefix_;
        std::string     separator_;
        std::string     suffix_;
        Munger          munger_;
    };

    template<typename Handler, typename ErrorPolicy = LogOnly>
    class FileListProcessor
    {
    public:
        FileListProcessor(Handler& handler, std::string const& directory)
        : directory_(directory)
        , processor_(NameMaker().normalize( directory_ ), handler)
        {}
        
        template<typename Iterator>
        void do_list( Iterator begin, Iterator const end ) const
        {
            for ( ; begin != end; ++begin )
            {
                processor_( *begin );
            }
        }
        
        void do_stream( std::istream& input ) const
        {
            std::string     _line;
            
            while ( std::getline( input, _line ) )
            {
                processor_( _line );
            }
        }
        
        void do_file( std::string const& file ) const
        {
            std::ifstream       _input(file.c_str());
            
            if ( _input )
            {
                do_stream( _input );
            }
            else
            {
                ErrorPolicy().on_warning( "Problem opening list file [" + file + "]!" );
            }
        }
        
    private:
        std::string const   directory_;
        class Processor
        {
        public:
            Processor(NameMaker const& nameMaker, Handler& handler)
            : nameMaker_(nameMaker)
            , handler_(handler)
            {}
            
            void operator() ( std::string const& key ) const
            {
                std::string const   _name(nameMaker_( key ));
                std::ifstream       _file(_name.c_str());
                
                if ( _file )
                {
                    handler_( _file, key );
                }
                else
                {
                    ErrorPolicy().on_warning( "Could not open file [" + _name + "]!" );
                }
            }
            
        private:
            NameMaker       nameMaker_;
            Handler&        handler_;
        }                   processor_;
    };
    
} // namespace Utility
