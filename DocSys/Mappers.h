
#pragma once

#include "DocSys/AgentSet.h"
#include "DocSys/TargetMethods.h"

#include <iostream>
#include <fstream>
#include <iterator>

    /**
     * Mappers.h.
     * Associates an Agent or a collection of Agents with a target or 
     * sink for the values found in an input document stream (e.g. XML). 
     * The target is addressed via a policy class (i.e. an interface)
     * to minimize assumptions about implementation.
     */

namespace DocSys
{   
    namespace 
    {
        struct LoadError
        {
            void on_error( std::string const& msg ) const
            {
                std::cerr << msg << std::endl;
            }
        };
        
        /**
         * Writer.  Local class to encapsulate TargetMethods and 
         * pair "begin, end" operations in RAII fashion.
         */
        template<typename Target>
        class Writer
        {
        public:
            using Methods = TargetMethods<Target>;
 
            Writer(Target& target, std::string const& label)
            : target_(target)
            {
                Methods::label( target_, label );
            }
            
            ~Writer()
            {
                Methods::end( target_ );
            }
            
            void operator() ( std::string const& item ) const
            {
                Methods::item( target_, item );
            }
            
            void operator() () const
            {
                Methods::no_data( target_ );
            }
        
        private:
            Target&     target_;        
        };

        /**
         * Inserter.  (Minimal) adapter for algorithms such as std::transform.
         */
        template<typename Writer>
        class Inserter
        : public std::iterator<std::output_iterator_tag, void, void, void, void >
        {
        public:
            Inserter(Writer& writer)
            : writer_(writer)
            {}
            
            Inserter& operator*  ()    { return *this; }
            Inserter& operator++ ()    { return *this; }
            Inserter& operator++ (int) { return *this; }
            
            template<typename T>
            Inserter& operator= ( T& item )
            {
                writer_( item );
                return *this; 
            }
            
        private:
            Writer&    writer_;
        };

        template<typename Document, typename ErrorPolicy = LoadError>
        class SimpleLoader
        {
        public:
            SimpleLoader(std::string const& label)
            : label_(label)
            {}
            
            template<typename Handler>
            bool operator()( std::istream& input, Handler const& handler ) const
            {
                try
                {
                    handler( Document(input), label_ );
                    return true;
                }
                catch ( typename Document::Throw& ex )
                {
                    ErrorPolicy().on_error( label_ + ": " + ex.what() );
                    return false;
                }
            }
        
        private:
            std::string const   label_;
        };
    }
    
    template<typename Document, typename Agent, typename Target, typename ErrorPolicy = LoadError>
    class AgentMapper
    {
    public:     
        AgentMapper(Agent const& agent, Target& target)
        : agent_(agent)
        , target_(target)
        {}

        bool operator()( std::istream& input, std::string const& label )
        {
            return SimpleLoader<Document, ErrorPolicy>(label)( input, *this );
        }
        
        void operator()( Document const& doc, std::string const& label )  const
        {
            using Inserter = Inserter<Writer<Target> >;

            Writer<Target>      _writer(target_, label);
            if ( doc.into_list( agent_, Inserter(_writer) ) == 0 )
            {
                _writer(); // no data
            }
        }
        
    private:
        Agent const agent_;
        Target&     target_;
    };
    
    template<typename Document, typename Agent, typename Target, typename ErrorPolicy = LoadError>
    class AgentSetMapper
    {
    public:
        AgentSetMapper(AgentSet<Agent> const& agents, Target& target)
        : mapper_(agents, target)
        {}
        
        AgentSetMapper const& header() const
        {
            mapper_.header();
            return *this;
        }
        
        void operator()( Document const& doc, std::string const& label, std::string const& context ) const
        {
            using Node = typename Document::Node;
            doc.apply( context, [&]( Node const& root ) -> void { mapper_( root, label ); } );
        }
        
        void operator()( Document const& doc, std::string const& label ) const
        {
            using Node = typename Document::Node;
            doc.process_root( [&]( Node const& root ) -> void { mapper_( root, label ); } );
        }

        using Loader = SimpleLoader<Document, ErrorPolicy>;

        bool operator()( std::istream& input, std::string const& label, std::string const& context ) const
        {
            return Loader(label)( input, [&]( Document const& doc, std::string const& label ) -> void
            {
                operator()( doc, label, context );
            } );
        }
        
        bool operator()( std::istream& input, std::string const& label ) const
        {
            return Loader(label)( input, *this );
        }
        
   private:
        struct Mapper
        {
            Mapper(AgentSet<Agent> const& agents, Target& target)
            : agents_(agents)
            , target_(target)
            {}
            
            // used to generate header row
            template<typename Iterator>
            void operator() ( Iterator begin, Iterator const end ) const
            {
                if ( begin != end )
                {
                    Writer<Target>      _writer(target_, *begin);
                    while ( ++begin != end )
                    {
                        _writer( *begin );
                    }
                }
            }

            void header() const
            {
                agents_.headers( *this );
            }
            
            template<typename Node>
            void operator() ( Node const& node, std::string const& label ) const
            {
                Writer<Target>  _writer(target_, label);
                for ( auto const& agent : agents_ )
                {
                    // this Agent interface should select at most one value. 
                    if ( !agent.value( node, _writer ) )
                    {
                        _writer(); // no data
                    }
                }
            }
            
            AgentSet<Agent> const&  agents_;
            Target&                 target_;
        }               mapper_;
    };
} // namespace DocSys
    
