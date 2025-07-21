#pragma once

#include <boost/filesystem.hpp>

template <typename stringType>
inline size_t num_of_same_chars_at_start(const stringType& s1, const stringType& s2)
{
  if (s1.length() > s2.length()) return num_of_same_chars_at_start(s2, s1);
  if (s1.empty()) return 0;
  typename stringType::size_type i = 0;
  for (; i < s1.length(); ++i)
    if (s1[i] != s2[i]) break;
  return i;
}

/**
* Makes sure \a path uses preferred slash and it ends with slash
*/
inline void make_preferred_dir_format(boost::filesystem::path& path)
{
  path.make_preferred();
  if (path.native().back() != path.preferred_separator)
    path = path.native() + path.preferred_separator;
}

/**
* Assumes both paths uses preferred slashes.
* \warning If the relative path cannot be computed then it returns empty path.
*/
inline boost::filesystem::path relative_path(boost::filesystem::path from, boost::filesystem::path to)
{
  if (from.is_relative() && !to.is_relative()) return relative_path(boost::filesystem::absolute(from), to);
  if (to.is_relative() && !from.is_relative()) return relative_path(from, boost::filesystem::absolute(to));
  if (boost::filesystem::is_regular_file(from)) return relative_path(from.parent_path(), to);

  auto n = num_of_same_chars_at_start(from.string(), to.string());
  if (n == 0) return boost::filesystem::path();
  auto f = from.string().substr(n);
  auto nUp = std::count(f.begin(), f.end(), '/');
  std::string relPath = (!f.empty()) ? "../" : "";
  for (size_t i = 0; i < (size_t) nUp; ++i)
  {
    relPath += "../";
  }
  auto t = to.string().substr(n);
  if (!t.empty() && t[0] == '/')
    t = t.substr(1);
  relPath += t;
  return relPath;
}
