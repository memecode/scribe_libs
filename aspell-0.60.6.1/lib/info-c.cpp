/* This file is part of The New Aspell
 * Copyright (C) 2001-2002 by Kevin Atkinson under the GNU LGPL
 * license version 2.0 or 2.1.  You should have received a copy of the
 * LGPL license along with this library if you did not you can find it
 * at http://www.gnu.org/.                                              */

#include "info.hpp"
#include "settings.h"

namespace acommon {

class Config;
struct DictInfo;
class DictInfoEnumeration;
class DictInfoList;
struct ModuleInfo;
class ModuleInfoEnumeration;
class ModuleInfoList;

C_EXPORT ModuleInfoList * get_aspell_module_info_list(Config * config)
{
  return const_cast<ModuleInfoList *>(get_module_info_list(config));
}

C_EXPORT void aspell_clear_all_lists(Config * config)
{
	clear_all_lists(config);
}

C_EXPORT int aspell_module_info_list_empty(const ModuleInfoList * ths)
{
  return ths->empty();
}

C_EXPORT unsigned int aspell_module_info_list_size(const ModuleInfoList * ths)
{
  return ths->size();
}

C_EXPORT ModuleInfoEnumeration * aspell_module_info_list_elements(const ModuleInfoList * ths)
{
  return ths->elements();
}

C_EXPORT DictInfoList * get_aspell_dict_info_list(Config * config)
{
  return const_cast<DictInfoList *>(get_dict_info_list(config));
}

C_EXPORT int aspell_dict_info_list_empty(const DictInfoList * ths)
{
  return ths->empty();
}

C_EXPORT unsigned int aspell_dict_info_list_size(const DictInfoList * ths)
{
  return ths->size();
}

C_EXPORT DictInfoEnumeration * aspell_dict_info_list_elements(const DictInfoList * ths)
{
  return ths->elements();
}

C_EXPORT int aspell_module_info_enumeration_at_end(const ModuleInfoEnumeration * ths)
{
  return ths->at_end();
}

C_EXPORT const ModuleInfo * aspell_module_info_enumeration_next(ModuleInfoEnumeration * ths)
{
  return ths->next();
}

C_EXPORT void delete_aspell_module_info_enumeration(ModuleInfoEnumeration * ths)
{
  delete ths;
}

C_EXPORT ModuleInfoEnumeration * aspell_module_info_enumeration_clone(const ModuleInfoEnumeration * ths)
{
  return ths->clone();
}

C_EXPORT void aspell_module_info_enumeration_assign(ModuleInfoEnumeration * ths, const ModuleInfoEnumeration * other)
{
  ths->assign(other);
}

C_EXPORT int aspell_dict_info_enumeration_at_end(const DictInfoEnumeration * ths)
{
  return ths->at_end();
}

C_EXPORT const DictInfo * aspell_dict_info_enumeration_next(DictInfoEnumeration * ths)
{
  return ths->next();
}

C_EXPORT void delete_aspell_dict_info_enumeration(DictInfoEnumeration * ths)
{
  delete ths;
}

C_EXPORT DictInfoEnumeration * aspell_dict_info_enumeration_clone(const DictInfoEnumeration * ths)
{
  return ths->clone();
}

C_EXPORT void aspell_dict_info_enumeration_assign(DictInfoEnumeration * ths, const DictInfoEnumeration * other)
{
  ths->assign(other);
}



}

