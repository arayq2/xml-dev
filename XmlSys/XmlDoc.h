
#pragma once

#include "XmlSys/XpathAgent.h"
#include <exception>
#include <iostream>
#include <sstream>

namespace XmlSys
{
    typedef pugi::xml_document      Xml_Doc;

    class DocBase
    {
    public:
        DocBase()
        : xmlDoc_()
        , errMsg_()
        {}
        
        DocBase(DocBase const& other)
        : xmlDoc_()
        , errMsg_(other.errMsg_)
        {
            xmlDoc_.reset( other.xmlDoc_ );
        }
        
        std::string const err_msg() const
        {
            return errMsg_;
        }
        
        template<typename Handler>
        void process_document( Handler& handler ) const
        {
            handler( xmlDoc_ );
        }
        
        template<typename Handler>
        void process_document( Handler const& handler ) const
        {
            handler( xmlDoc_ );
        }
        
        struct Throw
        : public std::exception
        {
            std::string     msg_;
            
            void on_error( std::string const& msg )
            {
                msg_.assign( msg );
                throw *this;
            }
            
            const char* what() const noexcept override
            {
                return msg_.c_str();
            }
        };
        
        struct DoNothing
        {
            void on_error( std::string const& ) {}
        };
        
    protected:
        ~DocBase() = default;
        
        Xml_Doc& document()
        {
            return xmlDoc_;
        }
        
        void assign( Xml_Doc const& other )
        {
            xmlDoc_.reset( other );
        }
        
        bool reset( std::istream& xml )
        {
            return parse( xml );
        }
        
        bool reset( std::string const& xml )
        {
            std::istringstream  _xml(xml);
            return parse( _xml );
        }
        
    private:
        bool parse( std::istream& xml )
        {
            Xml_Doc                 _tmp;
            pugi::xml_parse_result  _result(_tmp.load( xml ));
            
            if ( !_result )
            {
                std::ostringstream  _error;
                _error << "DocBase: load failure at " << _result.offset
                       << " : " << _result.description();
                errMsg_.assign( _error.str() );
                return false;
            }
            assign( _tmp );
            return true;    
        }
        
        Xml_Doc         xmlDoc_;
        std::string     errMsg_;
    };

    template<typename ErrorPolicy = DocBase::DoNothing>
    class Document
    : public DocBase
    {
    public:
        explicit
        Document(std::string const& xml)
        : DocBase()
        , root_()
        {
            init( reset( xml ) );
        }
        
        explicit
        Document(std::istream& xml)
        : DocBase()
        , root_()
        {
            init( reset( xml ) );
        }
        
        Document(Document const& other)
        : DocBase(other)
        , root_(other.root_)
        {}
        
        ~Document() throw() {}
        
        template<typename Handler>
        void process_root( Handler& handler ) const
        {
            handler( root_ );
        }
        
        template<typename Handler>
        void process_root( Handler const& handler ) const
        {
            handler( root_ );
        }
        
        std::ostream& stream( std::ostream& os ) const
        {
            root_.print( os );
            return os;
        }
        
        /**
         * wrap XpathAgent functionality
         */
        bool has_value( XpathAgent const& agent ) const 
            { return agent.probe( root_ ); }
        bool has_value( std::string const& xpath ) const
            { return root_.select_single_node( xpath.c_str() ); }
            
        std::string const get_value( XpathAgent const& agent ) const 
            { return agent( root_ ); }
        std::string const get_value( std::string const& query ) const 
            { return get_value( XpathAgent(query) ); }

        bool set_value( XpathAgent const& agent, std::string& target ) const 
            { return agent( root_, target ); }
        bool set_value( std::string const& query, std::string& target ) const 
            { return set_value( XpathAgent(query), target ); }
        
        template<typename Inserter>
        size_t into_list( XpathAgent const& agent, Inserter inserter ) const
            { return agent( root_, inserter ); }
        template<typename Inserter>
        size_t into_list( std::string const& query, Inserter inserter ) const
            { return into_list( XpathAgent(query), inserter ); }
        
        // generic operations
        template<typename Handler>
        size_t apply( XpathAgent const& agent, Handler& handler ) const
            { return agent.apply( root_, handler ); }
        template<typename Handler>
        size_t apply( std::string const& query, Handler& handler ) const
            { return apply( XpathAgent(query), handler ); }
            
        template<typename Handler>
        size_t apply( XpathAgent const& agent, Handler& handler, bool /*tag*/ ) const
            { return agent.apply( root_, handler, true ); }
        template<typename Handler>
        size_t apply( std::string const& query, Handler& handler, bool /*tag*/  ) const
            { return apply( XpathAgent(query), handler, true ); }
        
        template<typename Handler>
        size_t apply_raw( XpathAgent const& agent, Handler& handler ) const
            { return agent.apply_raw( root_, handler ); }
        template<typename Handler>
        size_t apply_raw( std::string const& query, Handler& handler ) const
            { return apply_raw( XpathAgent(query), handler ); }
        
        // const handler variants       
        template<typename Handler>
        size_t apply( XpathAgent const& agent, Handler const& handler ) const
            { return agent.apply( root_, handler ); }
        template<typename Handler>
        size_t apply( std::string const& query, Handler const& handler ) const
            { return apply( XpathAgent(query), handler ); }
            
        template<typename Handler>
        size_t apply( XpathAgent const& agent, Handler const& handler, bool /*tag*/ ) const
            { return agent.apply( root_, handler, true ); }
        template<typename Handler>
        size_t apply( std::string const& query, Handler const& handler, bool /*tag*/  ) const
            { return apply( XpathAgent(query), handler, true ); }
        
        template<typename Handler>
        size_t apply_raw( XpathAgent const& agent, Handler const& handler ) const
            { return agent.apply_raw( root_, handler ); }
        template<typename Handler>
        size_t apply_raw( std::string const& query, Handler const& handler ) const
            { return apply_raw( XpathAgent(query), handler ); }
            
    private:
        void init( bool parseOK )
        {
            if ( parseOK )
            {
                root_ = document().document_element();
            }
            else
            {
                ErrorPolicy().on_error( err_msg() );
            }
        }

        Xml_Node    root_;
    };
    
    typedef Document<DocBase::Throw>    XmlDoc;
    
} // namespace XmlSys

template <typename T>
std::ostream& operator<< ( std::ostream& os, XmlSys::Document<T> const& doc )
{
    return doc.stream( os );
}