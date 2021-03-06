
The story, in short:

[1059:~/Projects/xml/XpMatch]> ./xpmatch

        Nothing to do, exiting. Try -h option for help.
[1060:~/Projects/xml/XpMatch]> ./xpmatch -h

Usage:
        ./xpmatch options [ file ... | (read list from STDIN) ]
Options:

Help:
  -h [ --help ]         show options

Table mode options [Note: -t and -c are mutually exclusive]:
  -t [ --table ] arg    column-specs filename
  -c [ --column ] arg   inline column-spec (repeatable)
  -i [ --initial ] arg  initial context xpath

Grep mode options:
  -x [ --xpath ] arg    xpath pattern
  -b [ --blanks ]       include blanks (with -x option)
  -o [ --only ]         blanks only (with -b option)

Source (input) options:
  -l [ --listfile ] arg  list of files filename
  -d [ --directory ] arg directory for files (default .)
  -r [ --readxml ]       read xml content from STDIN

Format (output) options [Note: -q and -s are mutually exclusive]:
  -n [ --noheader ]      suppress header row (or no titles in grep mode)
  -q [ --quoted ]        CSV-style output for Excel (in table mode)
  -s [ --separator ] arg delimiter in table mode output (default TAB)

[1061:~/Projects/xml/XpMatch]>

Longer version:

xpmatch evaluates string-valued xpath expressions - i.e., element text 
content or attribute values - in XML files and prints the results to 
STDOUT.  

In "grep" mode, it evaluates a single expression and prints one result 
per line.  

In "table" mode, it evaluates a set of expressions relative to every 
instance of an initial context (which defaults to the document root), 
and prints only the first evaluation of each expression, to produce  
output in a flat file format.  For columnar fidelity, empty strings 
are output when expressions are not matched at all.

The "column" expressions can be specified in two ways: using the -c 
option (repeated, if needed) with an argument; or using the -t option 
to specify a file with the same arguments, one per line.  The format 
of an argument is: '<title><whitespace><expression>', where the title 
will be output in a header row; or just '<expression>', in which case 
the header row will have a synthesized title of the form '[N]' where 
N is the column number.

XML files to be processed can be specified as command line arguments;
or as the contents of a file, one name per line, with the -l option; 
or be passed in through STDIN, one name per line.  To save typing, the 
-d option can specify a directory relative to which the file names will 
be interpreted as file paths.  A single source xml file can also be 
read in through STDIN with the -r option.

The Xpath expressions handled are not fully general.  In particular, 
disjunctions of the form this-element-text-or-that-attribute-value 
are NOT supported.

Also, XML namespaces are not recognized, as the underlying parsing 
engine (pugixml) does not support them in the version used.  Some may 
deem this a flaw or even a bug; I consider it a feature.




