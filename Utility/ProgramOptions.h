#pragma once

#include <boost/program_options.hpp>

#define OPTION_PRESENT(V,S)     (V.count( S ) != 0)
#define OPTION_ABSENT(V,S)      (V.count( S ) == 0)
#define OPTION_DEFAULTED(V,S)	(V[S].defaulted())

namespace po = boost::program_options;
