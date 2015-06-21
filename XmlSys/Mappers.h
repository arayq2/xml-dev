
#pragma once

#include "XmlSys/XmlDoc.h"
#include "XmlSys/AgentSet.h"
#include "XmlSys/TargetMethods.h"

#include <iostream>
#include <fstream>

	/**
	 * Mappers.h.
	 * Associates an XpathAgent or a collection of XpathAgents with
	 * a target or sink for the values found in an input XML stream. 
	 * The target is addressed via a policy class (i.e. an interface)
	 * to minimize assumptions about implementation.
	 */

namespace XmlSys
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
	}
	
	template<typename Target, typename ErrorPolicy = LoadError>
	class AgentMapper
	{
	public:		
		AgentMapper(XpathAgent const& agent, Target& target)
		: agent_(agent)
		, target_(target)
		{}
		
		bool operator() ( std::istream& input, std::string const& label )
		{
			using Throw = DocBase::Throw;
			using Document = Document<Throw>;
			using Methods = TargetMethods<Target>; 
			using Inserter = typename TargetMethods<Target>::Inserter;
			try
			{
				Document			_doc(input);
				Inserter			_inserter(target_);
				
				Methods::label( target_, label );
				if ( _doc.into_list( agent_, _inserter ) == 0 )
				{
					Methods::no_data( target_ );
				}
				return true;
			}
			catch ( Throw& ex )
			{
				ErrorPolicy().on_error( label + ": " + ex.what() );
				return false;
			}
		}
			
	private:
		XpathAgent const	agent_;
		Target&				target_;
	};
	
	template<typename Target, typename ErrorPolicy = LoadError>
	class AgentSetMapper
	{
	public:
		AgentSetMapper(AgentSet const& agents, Target& target)
		: mapper_(agents, target)
		{}
		
		AgentSetMapper const& header() const
		{
			mapper_.header();
			return *this;
		}
		
		bool operator() ( std::istream& input, std::string const& label, std::string const& context ) const
		{
			using Throw = DocBase::Throw;
			using Document = Document<Throw>;
			try
			{
				using namespace std::placeholders;
				auto	_lambda(std::bind( mapper_, _1, label ));
				
				Document(input).apply( context, _lambda );
				return true;
			}
			catch ( Throw& ex )
			{
				ErrorPolicy().on_error( label + ": " + ex.what() );
				return false;
			}
		}
		
		bool operator() ( std::istream& input, std::string const& label ) const
		{
			using Throw = DocBase::Throw;
			using Document = Document<Throw>;
			try
			{
				using namespace std::placeholders;
				auto	_lambda(std::bind( mapper_, _1, label ));
				
				Document(input).process_root( _lambda );
				return true;
			}
			catch ( Throw& ex )
			{
				ErrorPolicy().on_error( label + ": " + ex.what() );
				return false;
			}
		}
		
	private:
		struct Mapper
		{
			Mapper(AgentSet const& agents, Target& target)
			: agents_(agents)
			, target_(target)
			{}
			
			using Methods = TargetMethods<Target>;

			void header() const
			{
				agents_.headers( target_ );
				Methods::end( target_ );
			}
			
			void operator() ( std::string const& item ) const
			{
				Methods::item( target_, item );
			}

			void operator() ( Xml_Node const& node, std::string const& label ) const
			{
				auto	_lambda([&]( XpathAgent const& agent ) -> void 
				{
					// this XpathAgent interface selects at most one value. 
					if ( !agent.value( node, *this ) )
					{
						Methods::no_data( target_ );
					}
				});

				Methods::label( target_, label );
				agents_.apply( _lambda );
				Methods::end( target_ );
			}
			
			AgentSet const&		agents_;
			Target&				target_;
		}				mapper_;
	};
} // namespace XmlSys
	
