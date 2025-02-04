/*
 * Copyright (C) 2025 The Layers Project
 *
 * This file is part of Layers.
 *
 * Layers is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Layers is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Layers. If not, see <https://www.gnu.org/licenses/>.
 */

#include <Layers/llink.h>

#include <Layers/lalgorithms.h>
#include <Layers/lattribute.h>
#include <Layers/ldefinition.h>

using Layers::LAttribute;
using Layers::LDefinition;
using Layers::LLink;
using Layers::LString;

class LLink::Impl
{
public:
    LString path;
    LString relative_path;

	LAttribute* attribute{ nullptr };
	LAttribute* relative_attribute{ nullptr };

    Impl(const LString& path, const LString& relative_path) :
        path{ path }, relative_path{ relative_path } {}

    Impl(LAttribute* attribute) :
        attribute{ attribute } {}

    bool resolve(LAttribute* attr)
    {
        if (!relative_path.empty())
        {
            LStringList relative_path_parts =
                split<LStringList>(relative_path, '/');

            /*
                NOTE:
                The following assumes that dynamic links always begin with double-
                dot notation and that there is always only 1 of them. Other cases
                have not yet been considered.
            */
            if (relative_path_parts[0] == "..")
            {
                if (attr->parent())
                {
                    if (LDefinition* def = dynamic_cast<LDefinition*>(attr->parent()))
                    {
                        if (def->parent())
                        {
                            if (LDefinition* def_parent = dynamic_cast<LDefinition*>(def->parent()))
                            {
                                for (const auto& [key, attr] : def_parent->attributes())
                                    if (attr->object_name() == relative_path_parts[1])
                                    {
                                        relative_attribute = attr;

                                        //return true;
                                        //return attr->as<T>(parent_parent_as_def);
                                    }
                            }
                        }
                    }
                }
            }
        }
        
        if (!path.empty())
        {
            auto path_parts = split<std::deque<LString>>(path, '/');
            LString absolute_attr_name = path_parts.back();
            path_parts.pop_back();

            if (path_parts.empty())
            {
                if (attr->parent())
                {
                    if (LDefinition* def = dynamic_cast<LDefinition*>(attr->parent()))
                    {
                        for (const auto& [key, attr] : def->attributes())
                            if (attr->object_name() == absolute_attr_name)
                            {
                                attribute = attr;

                                //return true;
                                //return attr->as<T>(parent_parent_as_def);
                            }
                    }
                }
            }

            else if (LDefinition* def = lController.find_definition(path_parts))
                for (const auto& [attr_name, attr] : def->attributes())
                    if (attr_name == absolute_attr_name)
                    {
                        attribute = attr;

                        return true;
                        //return attr->as<T>();
                    }
        }

        return true;
    }
};

LLink::LLink(const LString& absolute_path, const LString& relative_path) :
    pimpl{ new Impl(absolute_path, relative_path) } {}

LLink::LLink(LAttribute* attribute) :
	pimpl{ new Impl(attribute) } {}

LLink::LLink(const LLink& l) :
    pimpl{ new Impl(l.pimpl->path, l.pimpl->relative_path) }
{
	pimpl->attribute = l.pimpl->attribute;
	pimpl->relative_attribute = l.pimpl->relative_attribute;
}

 LAttribute* LLink::attribute() const
{
    return pimpl->attribute;
}

 LAttribute* LLink::relative_attribute() const
 {
     return pimpl->relative_attribute;
 }

 LString LLink::path() const
 {
     return pimpl->path;
 }

bool LLink::resolve(LAttribute* attr)
{
    return pimpl->resolve(attr);
}
