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

#ifndef LSTYLE_H
#define LSTYLE_H

#include "layers_global.h"
#include "layers_exports.h"

#include "ldefinition.h"

LAYERS_NAMESPACE_BEGIN

class LStyle;
using LStyleList = std::vector<LStyle*>;

class LAYERS_EXPORT LStyle : public LDefinition
{
public:
	LStyle(
		const LString& name,
		const LJsonValue& value,
		const std::filesystem::path& file_path,
		LDefinition* parent = nullptr);

	~LStyle();

	LString publisher() const;

	void set_publisher(const LString& publisher);

private:
	class Impl;
	Impl* pimpl;
};
LAYERS_NAMESPACE_END

#endif // LSTYLE_H