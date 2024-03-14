#pragma once

#include "allocator_malloc.hh"
#include <string>


using mstring = std::basic_string<char, std::char_traits<char>, AllocatorMalloc<char> >;
