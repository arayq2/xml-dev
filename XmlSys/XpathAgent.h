
#pragma once

#include <pugixml.hpp>
#include <algorithm>
#include <functional>
#include <string>

namespace XmlSys
{
	typedef	pugi::xml_node			Xml_Node;
	typedef	pugi::xml_attribute		Xml_Att;
	typedef	pugi::xpath_node		Xpath_Node;
	typedef	pugi::xpath_node_set	Xpath_NodeSet;
	
	/**
	 * XpathAgent.  Class to simplify processing of xpath queries applied
	 * to nodes in a document.
	 * Compound queries of the form this-node-or-that-attribute are not 
	 * supported.  The intent can be achieved by using two separate queries.
	 */
	class XpathAgent
	{
	public:
		// these heuristics are not fool-proof: the idea is to check whether
		// the query string ends with '@' followed by name characters only.
		static bool selectsAttribute( std::string const& xpath )
		{
			size_t		_lastPos(xpath.rfind( "@" ));
			return (_lastPos == std::string::npos)
			? false
			// inverse check for punctuation as substitute for "only name chars" 
			: xpath.find_first_of( "[]/=", _lastPos ) == std::string::npos
			;
		}
		
		static std::string const fromAttribute( Xpath_Node const& xpnode )
		{
			return xpnode.attribute().as_string();
		}
		
		static std::string const fromNode( Xpath_Node const& xpnode )
		{
			return xpnode.node().child_value();
		}
		
		explicit
		XpathAgent(std::string const& xpath)
		: xpath_(xpath)
		, fromAtt_(selectsAttribute( xpath ))
		{}
		
		XpathAgent(std::string const& xpath, bool fromAtt)
		: xpath_(xpath)
		, fromAtt_(fromAtt)
		{}

		bool probe( Xml_Node const& root ) const 
		{
			return root.select_single_node( xpath_.c_str() );
		}
		
		// canonical operation: extract string from first eligible node.
		std::string const operator () ( Xml_Node const& root ) const
		{
			Xpath_Node		_xpnode(root.select_single_node( xpath_.c_str() ));
			return _xpnode ? extract_( _xpnode ) : "";
		}
		// COM style
		bool operator () ( Xml_Node const& root, std::string& target ) const
		{
			Xpath_Node		_xpnode(root.select_single_node( xpath_.c_str() ));
			if ( _xpnode )
			{
				target.assign( extract_( _xpnode ) );
				return true;
			}
			return false;
		}
		
		template<typename Handler>
		bool value( Xml_Node const& root, Handler& handler ) const
		{
			Xpath_Node		_xpnode(root.select_single_node( xpath_.c_str() ));
			if ( _xpnode )
			{
				handler( extract_( _xpnode ) );
				return true;
			}
			return false;
			
		}

		template<typename Handler>
		void node( Xml_Node const& root, Handler& handler ) const
		{
			Xpath_Node		_xpnode(root.select_single_node( xpath_.c_str() ));
			if ( _xpnode )
			{
				handler( _xpnode.node() );
				return true;
			}
			return false;
			
		}

		template<typename Handler>
		void attribute( Xml_Node const& root, Handler& handler ) const
		{
			Xpath_Node		_xpnode(root.select_single_node( xpath_.c_str() ));
			if ( _xpnode )
			{
				handler( _xpnode.attribute() );
				return true;
			}
			return false;
			
		}

		// pass extracted strings from all eligible nodes to an inserter
		template <typename Inserter>
		size_t operator () ( Xml_Node const& root, Inserter inserter ) const
		{
			Xpath_NodeSet		_xpset(root.select_nodes( xpath_.c_str() ));
			if ( _xpset.size() > 0 )
			{
				if ( fromAtt_ )
				{
					std::transform( _xpset.begin(), _xpset.end(), inserter, fromAttribute );
				}
				else
				{
					std::transform( _xpset.begin(), _xpset.end(), inserter, fromNode ); 
				}
			}
			return _xpset.size();
			 
		}
		
		// generic operations on members of result sets.
		template <typename Handler>
		size_t apply( Xml_Node const& root, Handler& handler ) const
		{
			Xpath_NodeSet		_xpset(root.select_nodes( xpath_.c_str() ));
			for ( auto const& xpnode : _xpset )
			{
				handler( xpnode.node() );
			}
			return _xpset.size();			
		}

		template <typename Handler>
		size_t apply( Xml_Node const& root, Handler& handler, bool /* discriminator */ ) const
		{
			Xpath_NodeSet		_xpset(root.select_nodes( xpath_.c_str() ));
			for ( auto const& xpnode : _xpset )
			{
				handler( xpnode.attribute() );
			}
			return _xpset.size();			
		}

		template <typename Handler>
		size_t apply_raw( Xml_Node const& root, Handler& handler ) const
		{
			Xpath_NodeSet		_xpset(root.select_nodes( xpath_.c_str() ));
			for ( auto const& xpnode : _xpset )
			{
				handler( xpnode );
			}
			return _xpset.size();			
		}

	private:
		std::string const extract_( Xpath_Node const& xpnode ) const
		{
			return fromAtt_ ? xpnode.attribute().as_string() : xpnode.node().child_value(); 
		}
		
		std::string		xpath_;
		bool			fromAtt_;
	};
	
} // namespace XmlSys
