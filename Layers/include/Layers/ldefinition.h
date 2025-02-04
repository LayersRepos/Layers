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

#ifndef LDEFINITION_H
#define LDEFINITION_H

#include <deque>
#include <set>

#include "layers_global.h"
#include "layers_exports.h"

//#include "lattribute.h"
#include "ljsonvalue.h"
#include "lstring.h"
#include "lobject.h"

LAYERS_NAMESPACE_BEGIN

class LAttribute;

class LAYERS_EXPORT LDefinition : public LObject
{
public:
	LDefinition();

	LDefinition(
		const LString& name,
		const LJsonValue& value,
		const std::filesystem::path& file_path,
		LDefinition* parent = nullptr);

	~LDefinition();

	void append_child(LDefinition* child);

	void apply_style(LDefinition* style_def);

	std::vector<LString> attribute_group_names() const;

	std::map<LString, LAttribute*> attributes(int type_index = -1);

	LDefinition* base() const;

	LString base_name() const;

	LDefinition* child(int index) const;

	size_t child_count() const;

	std::map<LString, LDefinition*> children();

	void clear_style();

	std::set<LDefinition*> dependencies();

	LString file_name() const;

	void finalize();

	LAttribute* find_attribute(const LString& attr_name);

	LDefinition* find_item(const LString& path);

	LDefinition* find_item(std::deque<LString> name_list);

	bool has_unresolved_base() const;

	int index() const;

	bool is_overridable() const;

	LString path() const;

	void resolve_links();

	void set_base(LDefinition* base_def);

	LJsonObject to_json_object() const;

private:
	class Impl;
	Impl* pimpl;
};

LAYERS_NAMESPACE_END

#endif // LDEFINITION_H