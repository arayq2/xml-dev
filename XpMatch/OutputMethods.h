
#pragma once

#include "Utility/LineOutput.h"
#include "Utility/Iterable.h"
#include "XmlSys/TargetMethods.h"

	/**
	 * Implementations of XmlSys::TargetMethods "interface" (static polymorphism).
	 */

namespace XmlSys
{
	template<>
	struct TargetMethods<Utility::PrefixedOutput>
	{
		using Inserter = Utility::Iterable<Utility::PrefixedOutput>;

		static void label( Utility::PrefixedOutput& output, std::string const& label )
		{
			output( label );
		}

		static void no_data( Utility::PrefixedOutput& output )
		{
			output();
		}
		
		static void end( Utility::PrefixedOutput& output )
		{
			output();
		}
	};

	template<>
	struct TargetMethods<Utility::DelimitedOutput>
	{
		using Inserter = Utility::Iterable<Utility::DelimitedOutput>;

		static void label( Utility::DelimitedOutput& output, std::string const& label )
		{
			output( label, true );
		}
		
		static void item( Utility::DelimitedOutput& output, std::string const& item )
		{
			output( item );
		}
		
		static void no_data( Utility::DelimitedOutput& output )
		{
			output( "" ); // placeholder blank
		}
		
		static void end( Utility::DelimitedOutput& output )
		{
			output();
		}
	};

	template<>
	struct TargetMethods<Utility::QuotedOutput>
	{
		using Inserter = Utility::Iterable<Utility::QuotedOutput>;

		static void label( Utility::QuotedOutput& output, std::string const& label )
		{
			output( label, true );
		}
		
		static void item( Utility::QuotedOutput& output, std::string const& item )
		{
			output( item );
		}
		
		static void no_data( Utility::QuotedOutput& output )
		{
			output( "" ); // placeholder blank
		}
		
		static void end( Utility::QuotedOutput& output )
		{
			output();
		}
	};
	
 } // namespace Utility

