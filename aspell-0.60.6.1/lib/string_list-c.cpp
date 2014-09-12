/* Automatically generated file.  Do not edit directly. */

/* This file is part of The New Aspell
 * Copyright (C) 2001-2002 by Kevin Atkinson under the GNU LGPL
 * license version 2.0 or 2.1.  You should have received a copy of the
 * LGPL license along with this library if you did not you can find it
 * at http://www.gnu.org/.                                              */

#include "posib_err.hpp"
#include "string_list.hpp"
#include "settings.h"

namespace acommon {

class MutableContainer;
class StringEnumeration;
class StringList;

C_EXPORT StringList * new_aspell_string_list()
{
  return new_string_list();
}

C_EXPORT int aspell_string_list_empty(const StringList * ths)
{
  return ths->empty();
}

C_EXPORT unsigned int aspell_string_list_size(const StringList * ths)
{
  return ths->size();
}

C_EXPORT StringEnumeration * aspell_string_list_elements(const StringList * ths)
{
  return ths->elements();
}

C_EXPORT int aspell_string_list_add(StringList * ths, const char * to_add)
{
  return ths->add(to_add);
}

C_EXPORT int aspell_string_list_remove(StringList * ths, const char * to_rem)
{
  return ths->remove(to_rem);
}

C_EXPORT void aspell_string_list_clear(StringList * ths)
{
  ths->clear();
}

C_EXPORT MutableContainer * aspell_string_list_to_mutable_container(StringList * ths)
{
  return ths;
}

C_EXPORT void delete_aspell_string_list(StringList * ths)
{
  delete ths;
}

C_EXPORT StringList * aspell_string_list_clone(const StringList * ths)
{
  return ths->clone();
}

C_EXPORT void aspell_string_list_assign(StringList * ths, const StringList * other)
{
  ths->assign(other);
}



}

