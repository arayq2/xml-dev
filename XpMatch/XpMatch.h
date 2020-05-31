
#pragma once

#include "XmlSys/XmlDoc.h"
#include "DocSys/OutputMethods.h"
#include "DocSys/Mappers.h"
#include "Utility/FileListProcessor.h"
#include "Utility/ProgramOptions.h"

namespace XmlSys
{
    class XpMatch
    {
    public: 
        XpMatch() = default;
        ~XpMatch() = default;
        
        void run( int ac, char *av[] )
        {
            if ( parse( ac, av ) )
            {
                execute();
            }
        }
        
    private:
        XpMatch(XpMatch const&) = delete;
        XpMatch& operator= ( XpMatch const& ) = delete;

        po::variables_map           vm_;
        std::string                 table_;
        std::vector<std::string>    columns_;
        std::string                 initial_;
        std::string                 xpath_;
        std::string                 listfile_;
        std::string                 directory_;
        std::string                 separator_;
        std::vector<std::string>    clafiles_;
        
        bool parse( int ac, char *av[] )
        {       
            po::options_description         _help("Help");
            _help.add_options()
                ( "help,h", "show options" )
                ;
            po::options_description         _table("Table mode options [Note: -t and -c are mutually exclusive]");
            _table.add_options()
                ( "table,t", po::value<std::string>(&table_), "column-specs filename" )
                ( "column,c", po::value<std::vector<std::string> >(&columns_), "inline column-spec (repeatable)" )
                ( "initial,i", po::value<std::string>(&initial_), "initial context xpath" )
                ;
            po::options_description         _grep("Grep mode options");
            _grep.add_options()
                ( "xpath,x", po::value<std::string>(&xpath_), "xpath pattern" )
                ( "blanks,b", "include blanks (with -x option)" )
                ( "only,o", "blanks only (with -b option)" )
                ;
            po::options_description         _input("Source (input) options");
            _input.add_options()
                ( "listfile,l", po::value<std::string>(&listfile_), "list of files filename" )
                ( "directory,d", po::value<std::string>(&directory_), "directory for files (default .)" )
                ( "readxml,r", "read xml content from STDIN")
                ;
            po::options_description         _output("Format (output) options [Note: -q and -s are mutually exclusive]");
            _output.add_options()
                ( "noheader,n", "suppress header row (or no titles in grep mode)" )
                ( "quoted,q", "CSV-style output for Excel (in table mode)" )
                ( "separator,s", po::value<std::string>(&separator_), "delimiter in table mode output (default TAB)" )
                ;
            po::options_description         _hidden("Command line file list");
            _hidden.add_options()
                ( "clafiles", po::value<std::vector<std::string> >(&clafiles_), "[list of files ...]" )
                ;
            po::positional_options_description  _pod;
            _pod.add( "clafiles", -1 );
            
            po::options_description         _visible("Options");
            _visible.add( _help )
                .add( _table ).add( _grep )
                .add( _input ).add( _output );
            po::options_description         _all("All options");
            _all.add( _help )
                .add( _table ).add( _grep )
                .add( _input ).add( _output )
                .add( _hidden );
            
            po::store( po::command_line_parser( ac, av )
                .options( _all ).positional( _pod ).run(), vm_ );
            if ( OPTION_PRESENT(vm_, "help") )
            {
                std::cerr 
                    << "\nUsage: \n\t"
                    << av[0] << " options [ file ... | (read list from STDIN) ]\n"
                    << _visible
                    << std::endl;
                return false;
            }
            po::notify( vm_ );
            return true;
        }
        
        // handle mode
        void execute() const
        {
            if ( OPTION_PRESENT(vm_, "column") )
            {
                do_output( DocSys::AgentSet<XpathAgent>(columns_.begin(), columns_.end()) );
            }
            else
            if ( OPTION_PRESENT(vm_, "table") )
            {
                std::ifstream       _specs(table_.c_str());
                if ( _specs )
                {
                    do_output( DocSys::AgentSet<XpathAgent>(_specs) );
                }
                else
                {
                    std::cerr << "Problem with column specification file [" << table_ << "]!" << std::endl;
                }
            }
            else
            if ( OPTION_PRESENT(vm_, "xpath") )
            {
                do_grep();
            }
            else
            {
                std::cerr << "\n\tNothing to do, exiting. Try -h option for help." << std::endl;
            }
        }
        
        // set output style for table mode
        void do_output( DocSys::AgentSet<XpathAgent> const& agents ) const
        {
            if ( OPTION_PRESENT(vm_, "quoted") )
            {
                Utility::QuotedOutput       _output;
                do_table( agents, _output );
            }
            else
            {
                Utility::DelimitedOutput    _output;
                if ( OPTION_PRESENT(vm_, "separator") )
                {
                    _output.separator( separator_ );
                }
                do_table( agents, _output );
            }
        }
        
        template<typename Output>
        void do_table( DocSys::AgentSet<XpathAgent> const& agents, Output& output ) const
        {
            // associate agent set with output method
            DocSys::AgentSetMapper<XmlDoc, Output>  _mapper(agents, output);
            
            if ( OPTION_ABSENT(vm_, "noheader") )
            {
                _mapper.header();
            }
            // run
            if ( initial_.length() > 0 )  // this could be more robust
            {
                using namespace std::placeholders;
                auto    _lambda(std::bind( _mapper, _1, _2, initial_ ));
                dispatch( _lambda );
            }
            else
            {
                dispatch( _mapper );
            }
        }
        
        // handle input options
        template<typename Client>
        void dispatch( Client& client ) const
        {
            if ( OPTION_PRESENT(vm_, "readxml") )
            {
                client( std::cin, "STDIN" );
                return;
            }
            
            Utility::FileListProcessor<Client>      _reader(client, directory_);
            
            if ( OPTION_PRESENT(vm_, "listfile") )
            {
                _reader.do_file( listfile_ );
            }
            else
            if ( OPTION_PRESENT(vm_, "clafiles") )
            {
                _reader.do_list( clafiles_.begin(), clafiles_.end() );
            }
            else
            {
                //std::cerr << "[Reading <STDIN> for list of files...]" << std::endl;
                _reader.do_stream( std::cin );
            }
        }
        
        void do_grep() const
        {
            // configure output options
            Utility::PrefixedOutput     _output;
            _output
                .blanks( OPTION_PRESENT(vm_, "blanks") )
                .only( OPTION_PRESENT(vm_, "only") )
                .notitle( OPTION_PRESENT(vm_, "noheader") )
                ;
            // configure agent
            XpathAgent                  _agent(xpath_);
            // associate agent with output method
            DocSys::AgentMapper<XmlDoc, Utility::PrefixedOutput>
                                        _mapper(_agent, _output);
            // run for input options
            dispatch( _mapper );
        }
    };

} // namespace XmlSys
