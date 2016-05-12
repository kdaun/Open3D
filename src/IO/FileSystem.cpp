// ----------------------------------------------------------------------------
// -                        Open3D: www.open3d.org                            -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2015 Qianyi Zhou <Qianyi.Zhou@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "FileSystem.h"

#include <algorithm>
#ifdef WINDOWS
#include <dirent/dirent.h>
#include <direct.h>
#else
#include <sys/stat.h>
#include <dirent.h>
#endif

namespace three{

namespace filesystem {

std::string GetFileExtensionInLowerCase(const std::string &filename)
{
	size_t dot_pos = filename.find_last_of(".");
	if (dot_pos == std::string::npos || dot_pos == filename.length() - 1) {
		return "";
	}
	std::string filename_ext = filename.substr(dot_pos + 1);
	if (filename_ext.find_first_of("/\\") != std::string::npos) {
		return "";
	}
	std::transform(filename_ext.begin(), filename_ext.end(), 
			filename_ext.begin(), ::tolower);
	return filename_ext;
}

bool DirectoryExists(const std::string &directory)
{
	struct stat info;
	if (stat(directory.c_str(), &info) == -1)
		return false;
	return S_ISDIR(info.st_mode);
}

bool MakeDirectory(const std::string &directory)
{
#ifdef WINDOWS
	return (_mkdir(directory.c_str()) == 0);
#else
	return (mkdir(directory.c_str(), S_IRWXU) == 0);
#endif
}

bool DeleteDirectory(const std::string &directory)
{
#ifdef WINDOWS
	return (_rmdir(directory.c_str()) == 0);
#else
	return (rmdir(directory.c_str()) == 0);
#endif
}

bool FileExists(const std::string &filename)
{
	struct stat info;
	if (stat(filename.c_str(), &info) == -1)
		return false;
	return S_ISREG(info.st_mode);
}

bool ListFilesInDirectory(const std::string &directory,
	std::vector<std::string> &filenames)
{
	if (directory.empty()) {
		return false;
	}
	DIR *dir;
	struct dirent *ent;
	struct stat st;
	dir = opendir(directory.c_str());
	if (!dir) {
		return false;
	}
	while ((ent = readdir(dir)) != NULL) {
		const std::string file_name = ent->d_name;
		if (file_name[0] == '.')
			continue;
		std::string full_file_name;
		if (directory.back() == '/' || directory.back() == '\\') {
			full_file_name = directory + file_name;
		} else {
			full_file_name = directory + "/" + file_name;
		}
		if (stat(full_file_name.c_str(), &st) == -1)
			continue;
		if (S_ISREG(st.st_mode))
			filenames.push_back(full_file_name);
	}
	closedir(dir);
	return true;
}

}	// namespace three::filesystem

}	// namespace three