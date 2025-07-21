/*
The MIT License (MIT)

Copyright (c) 2014

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef __SATYA_UTILS_H__
#define __SATYA_UTILS_H__

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <string>

//////////////////////////////////////////////////////////////////////////

template <typename stringType>
inline size_t num_of_same_starting_chars(const stringType& s1, const stringType& s2)
{
   if(s1.length() > s2.length()) return num_of_same_starting_chars(s2, s1);
   if(s1.empty()) return 0;
   stringType::size_type i = 0;
   for(; i < s1.length(); ++i)
      if(s1[i] != s2[i]) break;
   return i;
}
/**
 * Assumes both paths uses preferred slashes.
 * \warning If the relative path cannot be computed then it returns empty path.
 */
inline boost::filesystem::path relative_path(const boost::filesystem::path& from, const boost::filesystem::path& to)
{
   if(from.is_relative() && !to.is_relative()) return relative_path(boost::filesystem::absolute(from), to);
   if(to.is_relative() && !from.is_relative()) return relative_path(from, boost::filesystem::absolute(to));
   if(boost::filesystem::is_regular_file(from)) return relative_path(from.parent_path(), to);
   typedef boost::filesystem::path::string_type bfs_string;
   bfs_string::size_type n = num_of_same_starting_chars(from.native(), to.native());
   if(n == 0) return boost::filesystem::path();
   bfs_string f = from.native().substr(n);
   bfs_string::size_type nUp = std::count(f.begin(), f.end(), from.preferred_separator);
   boost::filesystem::path relPath;
   for(bfs_string::size_type i = 0; i < nUp; ++i) {
      relPath /= "..";
   }
   relPath /= to.native().substr(n);
   return relPath;
}

/// Wraps string inside double quotes.
inline std::string strLit(const std::string& str)
{
	return '"' + str + '"';
}

#endif //__SATYA_UTILS_H__
