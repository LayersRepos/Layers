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

#include <Layers/lstyle.h>

using Layers::LString;
using Layers::LStyle;

class LStyle::Impl
{
public:
	LString publisher = "";
};

LStyle::LStyle(
	const LString& name,
	const LJsonValue& value,
	const std::filesystem::path& file_path,
	LDefinition* parent) :
	pimpl{ new Impl() },
	LDefinition(name, value, file_path, parent)
{
	if (value.is_object())
	{
		LJsonObject object = value.to_object();

		if (object.find("_meta") != object.end())
		{
			LJsonObject meta_object = object["_meta"].to_object();

			if (meta_object.find("publisher") != meta_object.end())
				set_publisher(meta_object["publisher"].to_string());
		}
	}
}

LStyle::~LStyle()
{
	delete pimpl;
}

LString LStyle::publisher() const
{
	return pimpl->publisher;
}

void LStyle::set_publisher(const LString& publisher)
{
	pimpl->publisher = publisher;
}
