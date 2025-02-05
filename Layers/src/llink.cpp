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

    Impl(const LString& path, const LString& relative_path) :
        path{ path }, relative_path{ relative_path } {}

    Impl(LAttribute* attribute) :
        attribute{ attribute } {}

    bool resolve(LAttribute* attr)
    {
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
            {
                for (const auto& [attr_name, attr] : def->attributes())
                    if (attr_name == absolute_attr_name)
                    {
                        attribute = attr;

                        return true;
                        //return attr->as<T>();
                    }
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
}

LAttribute* LLink::attribute() const
{
    return pimpl->attribute;
}

LString LLink::path() const
{
    return pimpl->path;
}

LString LLink::relative_path() const
{
    return pimpl->relative_path;
}

bool LLink::resolve(LAttribute* attr)
{
    return pimpl->resolve(attr);
}

LAttribute* LLink::resolve(LDefinition* starting_context)
{
    if (!starting_context) {
        // Cannot resolve without a valid starting context.
        return nullptr;
    }

    // Tokenize the path by '/'.
    // (Assume you have some tokenization function available.)
    std::vector<LString> tokens = split<std::vector<LString>>(pimpl->relative_path, '/');

    // Begin with the provided derivative definition.
    LDefinition* context = starting_context;

    // Walk up the hierarchy for each ".." token.
    size_t tokenIndex = 0;
    while (tokenIndex < tokens.size() && tokens[tokenIndex] == "..") {
        // Move one level up in the definition hierarchy.
        LDefinition* parent_context = context->parent();
        if (!parent_context) {
            // You could choose to handle this as an error or simply break out.
            // For now, we break out or return nullptr.
            return nullptr;
        }
        context = parent_context;
        tokenIndex++;
    }

    // Optionally, skip over any current directory tokens (".").
    while (tokenIndex < tokens.size() && tokens[tokenIndex] == ".") {
        tokenIndex++;
    }

    // The remaining tokens (if any) specify the path to the attribute relative to currentContext.
    // For simplicity, if there’s only one token left, treat that as the attribute name.
    // If there are multiple tokens, you might need to recursively walk sub-containers,
    // or join them into one key, depending on your data structure.
    if (tokenIndex < tokens.size()) {
        // For illustration, let’s assume a simple attribute lookup using a joined key.
        // (Alternatively, you might iterate over the tokens and do a hierarchical lookup.)
        LString remainingKey;
        // Join remaining tokens with your delimiter (for example, using a simple loop)
        for (size_t i = tokenIndex; i < tokens.size(); ++i) {
            if (i != tokenIndex)
                remainingKey = remainingKey + "/";
            remainingKey = remainingKey + tokens[i];
        }

        // Now, ask the current context to return the attribute with this name.
        return context->find_attribute(remainingKey);
    }

    // If no tokens remain, you might want to return the whole context as an attribute,
    // or simply return nullptr depending on what “../” alone means in your system.
    return nullptr;
}
