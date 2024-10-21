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

#include <Layers/ldefinition.h>

#include <string>
#include <vector>

#include <Layers/lalgorithms.h>
#include <Layers/lattribute.h>
#include <Layers/lpaths.h>

using Layers::LAttribute;
using Layers::LJsonObject;
using Layers::LString;
using Layers::LDefinition;

class LDefinition::Impl
{
public:
	LDefinition* base{ nullptr };
	std::filesystem::path base_path;
	LString base_name;

	LJsonValue value;
	std::filesystem::path file_path;

	std::map<LString, LDefinition*> m_children;
	std::map<LString, LAttribute*> m_attributes;
	//LString m_file_name;
	bool m_is_overridable{ false };

	Impl() {}

	Impl(
		const LJsonValue& value,
		const std::filesystem::path& file_path) :
		file_path{ file_path },
		value{ value }
	{
		// Lambda to handle dependency parsing
		auto parse_include = [&](const LString& include_str)
			-> std::pair<std::filesystem::path, LString>
			{
				std::string include(include_str.c_str());
				size_t delim_pos = include.find("::");
				std::string file_part = include.substr(0, delim_pos);
				std::string widget_name = include.substr(delim_pos + 2);
				std::pair<std::filesystem::path, LString> dep_data;

				// Check if the path contains a slash character
				if (file_part.find('/') != std::string::npos ||
					file_part.find('\\') != std::string::npos)
				{
					// External dependency - append definitions_path() to it
					dep_data.first = definitions_path() / file_part;
				}
				else
				{
					// File is a relative path within the same project directory
					dep_data.first = file_path.parent_path() / file_part;
				}

				// Store the widget name in the pair's second value
				dep_data.second = LString(widget_name.c_str());

				return dep_data;
			};

		if (value.is_string())
		{
			// String-based inheritance (e.g., "Widget": "qlbox.json::Box")
			std::pair<std::filesystem::path, LString> base_data = 
				parse_include(value.to_string());

				base_path = base_data.first;
				base_name = base_data.second;
		}
		else if (value.is_object())
		{
			LJsonObject object = value.to_object();

			if (object.find("_include") != object.end())
			{
				// Object-based inheritance (e.g., "Dialog": { "_include": "qlbox.json::Box", ... })
				std::pair<std::filesystem::path, LString> base_data = 
					parse_include(object["_include"].to_string());

				base_path = base_data.first;
				base_name = base_data.second;
			}

			if (object.find("attributes") != object.end())
			{
				LJsonObject attrs = object["attributes"].to_object();

				for (const auto& [key, attr] : attrs)
					m_attributes[key] = new LAttribute(key, attrs[key]);
			}

			if (object.find("children") != object.end())
				for (const auto& [key, value] : object["children"].to_object())
					append_child(new LDefinition(key, value, file_path));
		}
	}

	void append_child(LDefinition* child)
	{
		m_children[child->object_name()] = child;
	}

	std::vector<LString> attribute_group_names() const
	{
		std::vector<LString> attribute_group_names;

		for (const auto& [key, attr] : m_attributes)
		{
			LString attr_name = attr->object_name();

			if (std::find(attr_name.begin(), attr_name.end(),
				'.') != attr_name.end())
			{
				auto group_name = split<std::vector<LString>>(
					attr_name, '.').front();

				if (std::find(attribute_group_names.begin(),
					attribute_group_names.end(),
					group_name) == attribute_group_names.end())
				{
					attribute_group_names.push_back(group_name);
				}
			}
		}

		return attribute_group_names;
	}

	std::map<LString, LAttribute*> attributes(int type_index = -1)
	{
		// TODO: Gather base attributes and consider any owned by this
		//       as overriding the base.

		LAttributeMap attrs;
		attrs.insert(m_attributes.begin(), m_attributes.end());

		if (base)
		{
			LAttributeMap base_attrs = base->attributes();
			attrs.insert(base_attrs.begin(), base_attrs.end());
		}
		
		if (type_index < 0)
			return attrs;

		LAttributeMap filtered_attrs;

		for (const auto& [key, attr] : attrs)
			if (attr->type_index() == type_index)
				filtered_attrs[key] = attr;

		return filtered_attrs;
	}

	LDefinition* child(int index) const
	{
		if (index < 0 || index >= m_children.size())
			return nullptr;

		auto it = m_children.begin();
		std::advance(it, index);

		return it->second;
	}

	size_t child_count() const
	{
		return m_children.size();
	}

	std::map<LString, LDefinition*> children()
	{
		std::map<LString, LDefinition*> children;

		children.insert(m_children.begin(), m_children.end());

		if (base)
		{
			std::map<LString, LDefinition*> base_children = base->children();
			children.insert(base_children.begin(), base_children.end());
		}

		return children;
	}

	std::set<std::filesystem::path> dependencies()
	{
		std::set<std::filesystem::path> dependencies;

		if (!base_path.empty())
			dependencies.insert(base_path);

		for (const auto& [name, child_def]: children())
			for (const std::filesystem::path& child_dep : child_def->dependencies())
				if (!child_dep.empty())
					dependencies.insert(child_dep);

		return dependencies;
	}

	LString file_name() const
	{
		return LString(file_path.filename().string().c_str());
	}

	void finalize_attributes()
	{
		// for (auto [attr_name, attr] : m_attributes)
		// {
		// 	/*
		// 		1. Check if attr_name matches the name of any base attributes

		// 		- This should probably check attributes going up the
		// 		  inheritance hierarchy.
		// 	*/
		// }

		// for (auto [child_name, child] : children())
		// 	child->finalize_attributes();
	}

	LAttribute* find_attribute(const LString& name)
	{
		for (auto [attr_name, attr] : m_attributes)
		{
			if (attr_name == name)
				return attr;
		}

		if (base)
			return base->find_attribute(name);
		
		return nullptr;
	}

	LDefinition* find_item(std::deque<LString> name_list)
	{
		if (!name_list.empty())
		{
			std::deque<LString> new_name_list = name_list;

			LString name = *new_name_list.begin();
			new_name_list.pop_front();

			// Check children
			for (const auto& [key, child_item] : m_children)
			{
				if (child_item->object_name() == name)
				{
					if (new_name_list.empty())
						return child_item;
					else
						return child_item->find_item(new_name_list);
				}
			}

			// Check base
			if (base)
				return base->find_item(name_list);
		}

		return nullptr;
	}

	bool is_overridable() const
	{
		return m_is_overridable;
	}

	LJsonObject to_json_object() const
	{
		LJsonObject item_object;
		LJsonObject attributes_object;
		LJsonObject children_object;

		//for (const auto& [key, attr] : m_attributes)
		//	attributes_object[key] = attr->json_object();

		for (const auto& [key, child] : m_children)
			if (child->pimpl->file_name() == file_name())
				children_object[child->object_name()] =
				child->to_json_object();

		if (!attributes_object.empty())
			item_object["attributes"] = attributes_object;

		if (!children_object.empty())
			item_object["children"] = children_object;

		if (m_is_overridable)
			item_object["is_overridable"] = m_is_overridable;

		return item_object;
	}
};

//LDefinition::LDefinition(
//	const LString& name,
//	const LAttributeMap& attributes,
//	bool is_overridable,
//	const LString& file_name,
//	LDefinition* parent
//) :
//	pimpl{ new Impl(attributes, is_overridable, file_name) },
//	LObject(parent)
//{
//	set_object_name(name);
//
//	for (const auto& [key, attr] : attributes)
//		attr->set_parent(this);
//}

LDefinition::LDefinition() :
	pimpl{ new Impl() },
	LObject() {}

LDefinition::LDefinition(
	const LString& name,
	const LJsonValue& value,
	const std::filesystem::path& file_path,
	LDefinition* parent
) :
	pimpl{ new Impl(value, file_path) },
	LObject(parent)
{
	set_object_name(name);

	for (const auto& [key, attr] : attributes())
		attr->set_parent(this);

	for (const auto& [name, definition] : children())
		definition->set_parent(this);
}

LDefinition::~LDefinition()
{
	delete pimpl;
}

void LDefinition::append_child(LDefinition* child)
{
	pimpl->append_child(child);
}

std::vector<LString> LDefinition::attribute_group_names() const
{
	return pimpl->attribute_group_names();
}

std::map<LString, LAttribute*> LDefinition::attributes(int type_index)
{
	return pimpl->attributes(type_index);
}

LDefinition *LDefinition::base() const
{
    return pimpl->base;
}
LString LDefinition::base_name() const
{
	return pimpl->base_name;
}

LDefinition* LDefinition::child(int index) const
{
	return pimpl->child(index);
}

size_t LDefinition::child_count() const
{
	return pimpl->child_count();
}

std::map<LString, LDefinition*> LDefinition::children()
{
	return pimpl->children();
}

std::set<std::filesystem::path> LDefinition::dependencies()
{
	return pimpl->dependencies();
}

LString LDefinition::file_name() const
{
	return pimpl->file_name();
}

void LDefinition::finalize_attributes()
{
	pimpl->finalize_attributes();
}

LAttribute* LDefinition::find_attribute(const LString& attr_name)
{
	return pimpl->find_attribute(attr_name);
}

LDefinition* LDefinition::find_item(const LString& path)
{
	return find_item(split<std::deque<LString>>(path, '/'));
}

LDefinition* LDefinition::find_item(std::deque<LString> name_list)
{
	return pimpl->find_item(name_list);
}

bool LDefinition::has_unresolved_base() const
{
	if (!pimpl->base && !pimpl->base_path.empty() && !pimpl->base_name.empty())
		return true;
	
	return false;
}

int LDefinition::index() const
{
	if (LDefinition* parent_item = dynamic_cast<LDefinition*>(parent()))
	{
		int i = 0;
		for (auto it = parent_item->pimpl->m_children.begin();
			it != parent_item->pimpl->m_children.end(); ++it, ++i)
		{
			if (it->second == this)
				return i;
		}
	}
	return 0;
}

bool LDefinition::is_overridable() const
{
	return pimpl->is_overridable();
}

LString LDefinition::path() const
{
	std::vector<LString> path_names;

	path_names.push_back(object_name());

	LDefinition* theme_item = dynamic_cast<LDefinition*>(parent());

	while (theme_item)
	{
		LString name = theme_item->object_name();
		if (!name.empty())
			path_names.insert(path_names.begin(), name);

		theme_item = dynamic_cast<LDefinition*>(theme_item->parent());
	}

	std::ostringstream joined_names;
	std::copy(path_names.begin(), path_names.end(),
		std::ostream_iterator<LString>(joined_names, "/"));
	std::string result = joined_names.str();

	return result.substr(0, result.length() - 1).c_str();
}

void LDefinition::resolve_links()
{
	for (const auto& [attr_name, attr] : attributes())
		attr->resolve_links(this);

	for (const auto& [def_name, def] : children())
		def->resolve_links();
}

void LDefinition::set_base(LDefinition* base_def)
{
	pimpl->base = base_def;
}

LJsonObject LDefinition::to_json_object() const
{
	return pimpl->to_json_object();
}
