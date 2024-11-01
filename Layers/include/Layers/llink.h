/*
 * Copyright (C) 2024 The Layers Project
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

#ifndef LLINK_H
#define LLINK_H

#include "layers_global.h"
#include "layers_exports.h"

#include <map>
#include <vector>

#include "lstring.h"

LAYERS_NAMESPACE_BEGIN

class LAttribute;
class LDefinition;

class LLink;
using LLinkList = std::vector<LLink*>;

class LAYERS_EXPORT LLink
{
public:
	LLink(const LString& path, const LString& relative_path = "");

    LLink(LAttribute* attribute);

	LLink(const LLink& l);

    LAttribute* attribute() const;

	LAttribute* relative_attribute() const;

    bool resolve(LAttribute* attr);

private:
	class Impl;
	Impl* pimpl;
};

LAYERS_NAMESPACE_END

#endif // LLINK_H
