#pragma once

#include <string>
#include <vector>
#include <istream>
#include <sstream>

namespace DocSys
{
    class FormatParser
    {
    public:
        FormatParser()
        : title_("Source")
        , ifs_(" \t")
        {}
        
        std::string const title() const { return title_; }
        FormatParser& title( std::string const& value ) { title_.assign( value ); return *this; }
        FormatParser& ifs( std::string const& value ) { ifs_.assign( value ); return *this; }
        
        template<typename Handler>
        bool operator() ( std::string const& line, size_t index, Handler& handler ) const
        {
            // ignore blanks and comments
            if ( line.length() == 0 or line[0] == '#' )
            { 
                return false;
            }
            // ignore leading whitespace
            size_t          _bpos(line.find_first_not_of( ifs_ ));
            if ( _bpos == std::string::npos )
            {
                return false;
            }
            // next whitespace
            size_t          _lpos(line.find_first_of( ifs_, _bpos ));
            if ( _lpos == std::string::npos )
            {
                std::ostringstream      _oss;
                _oss << "[" << index << "]";
                handler( _oss.str(), line.substr( _bpos ) );
            }
            else
            {
                // second word
                size_t      _rpos(line.find_first_not_of( ifs_, _lpos ));
                if ( _rpos == std::string::npos )
                {
                    std::ostringstream      _oss;
                    _oss << "[" << index << "]";
                    handler( _oss.str(), line.substr( _bpos, _lpos - _bpos ) );
                }
                else
                {
                    handler( line.substr( _bpos, _lpos - _bpos ), line.substr( _rpos ) );
                }
            }
            return true;
        }
        
    private:
        std::string     title_;
        std::string     ifs_;
    };

    // template<typename Client> ClientMethods;
    
    template<typename Client>
    class SetLoader
    {
    public:
        SetLoader(Client& client, FormatParser const& parser = FormatParser())
        : client_(client)
        , parser_(parser)
        , index_(1)
        {
            //ClientMethods<Client>::add_title( client_, parser_.title() );
            client_.add_title( parser_.title() );
        }
        
        void operator() ( std::istream& source )
        {
            std::string         _line;
            while ( std::getline( source, _line ) )
            {
                parse( _line );
            }
        }
        
        template<typename Iterator>
        void operator() ( Iterator begin, Iterator const end )
        {
            for ( ; begin != end; ++begin )
            {
                parse( *begin );
            }
        }
    
    private:
        void parse( std::string const& line )
        {
            auto    _lambda([&]( std::string const& title, std::string const& xpath ) -> void
            {
                //ClientMethods<Client>::add_item( client_, title, path );
                client_( title, xpath );
            });
                        
            if ( parser_( line, index_, _lambda ) )
            {
                ++index_;
            }
        }
        
        Client&         client_;
        FormatParser    parser_;
        int             index_;
    };
    
    template<typename Agent>
    class AgentSet
    {
    public:
        friend class SetLoader<AgentSet<Agent> >;

        AgentSet(std::istream& source, FormatParser const& parser = FormatParser())
        : header_()
        , agents_()
        {
            SetLoader<AgentSet<Agent> >(*this, parser)( source );
        }
        
        template<typename Iterator>
        AgentSet(Iterator begin, Iterator const end, FormatParser const& parser = FormatParser())
        : header_()
        , agents_()
        {
            SetLoader<AgentSet<Agent> >(*this, parser)( begin, end );
        }
        
        template<typename Handler>
        void headers( Handler& handler ) const
        {
            handler( header_.cbegin(), header_.cend() );
        }
        
        template<typename Client>
        size_t apply( Client& client ) const
        {
            for ( auto const& agent : agents_ )
            {
                client( agent );
            }
            return agents_.size();
        }
        
        template<typename Client>
        size_t apply( Client const& client ) const
        {
            for ( auto const& agent : agents_ )
            {
                client( agent );
            }
            return agents_.size();
        }
        
        using const_iterator = typename std::vector<Agent>::const_iterator;
        
        const_iterator begin() const { return agents_.begin(); }
        const_iterator end() const { return agents_.end(); }

    private:
        void add_title( std::string const& title )
        {
            header_.push_back( title );
        }
        
        void operator() ( std::string const& title, std::string const& path )
        {
            header_.push_back( title );
            agents_.push_back( Agent(path) );
        }
        
        std::vector<std::string>    header_;
        std::vector<Agent>          agents_;
    };
    
} // namespace DocSys
