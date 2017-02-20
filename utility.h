#ifndef UTILITY_H
#define UTILITY_H

#include <boost/filesystem/path.hpp>

namespace Utility {
	bool copy_file(boost::filesystem::path from, boost::filesystem::path to);
}

#endif // UTILITY_H