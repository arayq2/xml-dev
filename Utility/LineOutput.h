
#pragma once

#include <iostream>
#include <string>

namespace Utility
{
	class PrefixedOutput
	{
	public:
		explicit
		PrefixedOutput(std::ostream& os = std::cout)
		: os_(os)
		, source_()
		, separator_(":")
		, blanks_(false)
		, only_(false)
		, notitle_(false)
		{}
		
		void operator() ( std::string const& item )
		{
			if ( blanks_ and only_ and item.length() > 0 )
			{
				return;
			}
			if ( !notitle_ and source_.length() > 0 )
			{
				os_ << source_ << separator_;
			}
			os_ << item << std::endl;
		}
		
		void operator() ()
		{
			if ( blanks_ )
			{
				(*this)( "" );
			}
		}
	
		PrefixedOutput& source( std::string const& value ) { source_.assign( value ); return *this; }
		struct Source {};
		PrefixedOutput& operator() ( std::string const& value, Source const& ) { return source( value ); }

		PrefixedOutput& separator( std::string const& value ) { separator_.assign( value ); return *this; }
		struct Separator {};
		PrefixedOutput& operator() ( std::string const& value, Separator const& ) { return separator( value ); }

		PrefixedOutput& blanks( bool value ) { blanks_ = value; return *this; }
		struct Blanks {};
		PrefixedOutput& operator() ( bool value, Blanks const& ) { return blanks( value ); }
		
		PrefixedOutput& only( bool value ) { only_ = value; return *this; }
		struct Only {};
		PrefixedOutput& operator() ( bool value, Only const& ) { return only( value ); }
		
		PrefixedOutput& notitle( bool value ) { notitle_ = value; return *this; }
		struct NoTitle {};
		PrefixedOutput& operator() ( bool value, NoTitle const& ) { return notitle( value ); }
			
	private:
		std::ostream& 	os_;
		std::string		source_;
		std::string		separator_;
		bool			blanks_;
		bool			only_;
		bool			notitle_;
	};
			
	class DelimitedOutput
	{
	public:
		explicit
		DelimitedOutput(std::ostream& os = std::cout, std::string const& separator = "\t")
		: os_(os)
		, separator_(separator)
		{}
		
		DelimitedOutput& separator( std::string const& value )
		{
			separator_.assign( value );
			return *this;
		}
		
		template<typename Iterator>
		void operator() ( Iterator begin, Iterator const end )
		{
			if ( begin != end )
			{
				(*this)( *begin, true );
				while ( ++begin != end )
				{
					(*this)( *begin );
				}
			}
		}
		
		void operator() ( std::string const& label, bool /*tag*/ ) 
		{
			os_ << label;
		}
		
		void operator() ( std::string const& item )
		{
			os_ << separator_ << item;
		}
		
		void operator() ()
		{
			os_ << std::endl;
		}
	
	private:
		std::ostream&	os_;
		std::string		separator_;
	};
	
	class QuotedOutput
	{
	public:
		explicit
		QuotedOutput(std::ostream& os = std::cout)
		: os_(os)
		{}
		
		template<typename Iterator>
		void operator() ( Iterator begin, Iterator const end )
		{
			if ( begin != end )
			{
				(*this)( *begin, true );
				while ( ++begin != end )
				{
					(*this)( *begin );
				}
			}
		}
		
		void operator() ( std::string const& label, bool /*tag*/ ) 
		{
			os_ << "\"" << label << "\"";
		}
		
		void operator() ( std::string const& item )
		{
			os_ << ",\"" << item << "\"";
		}
		
		void operator() ()
		{
			os_ << std::endl;
		}
	
	private:
		std::ostream&	os_;
	};
	
} // namespace Utility

