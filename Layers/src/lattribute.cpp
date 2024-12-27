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

#include <Layers/lattribute.h>
#include <Layers/ldefinition.h>

using Layers::LAttribute;
using Layers::LAttributeList;
using Layers::LAttributeMap;
using Layers::LConnectionID;
using Layers::LJsonObject;
using Layers::LJsonValue;
using Layers::LLink;
using Layers::LObject;
using Layers::LString;
using Layers::LStringList;
using Layers::LVariant;

template double LAttribute::as<double>(const LStringList&);
template bool LAttribute::as<bool>(const LStringList&);
template LString LAttribute::as<LString>(const LStringList&);
template LStringList LAttribute::as<LStringList>(const LStringList&);

class LAttribute::Impl
{
public:
	LConnectionID m_link_destroyed_connection;
	LConnectionID def_connection;

	LAttributeList m_dependent_attrs;

	LAttribute* def_attr{ nullptr };

	LLink* link{ nullptr };

	LAttributeMap states;

	LVariant value;

	LJsonValue json_value;

	LConnections m_change_connections;
	LConnectionID m_change_connections_next_id;

	LConnections m_link_change_connections;
	LConnectionID m_link_change_connections_next_id;

	Impl(const LString& name) {}

	Impl(const LString& name, double value) :
		value{ value } {}

	Impl(const LString& name, const char* value) :
		value{ LString(value) } {}

	Impl(const LString& name, const LVariant& value) :
		value{ value } {}

	Impl(const LString& name, LJsonValue json_value) :
		json_value{ json_value }
	{
		if (json_value.is_object())
		{
			LJsonObject obj = json_value.to_object();

			LString absolute_link_path = "";
			LString relative_link_path = "";

			if (obj.count("link"))
				absolute_link_path = obj["link"].to_string().remove("L:");

			if (obj.count("link_relative"))
				relative_link_path = obj["link_relative"].to_string().remove("L:");

			if (!absolute_link_path.empty() || !relative_link_path.empty())
				link = new LLink(absolute_link_path, relative_link_path);

			if (obj.count("value"))
				init_value(obj["value"]);
		}
		else
			init_value(json_value);
	}

	~Impl()
	{
		if (def_attr)
			def_attr->disconnect_change(def_connection);

		if (link && link->attribute())
		{
			link->attribute()->disconnect_destroyed(m_link_destroyed_connection);

			 for (auto dep_attr = link->attribute()->pimpl->m_dependent_attrs.begin();
			 	dep_attr != link->attribute()->pimpl->m_dependent_attrs.end(); dep_attr++)
			 {
			 	if ((*dep_attr)->pimpl == this)
			 	{
			 		dep_attr = link->attribute()->pimpl->m_dependent_attrs.erase(dep_attr);
					link->attribute()->pimpl->update_link_dependencies();
			 		break;
			 	}
			 }

			link = nullptr;
		}
	}

	void init_value(const LJsonValue json_value)
	{
		if (json_value.is_double())
		{
			value = json_value.to_double();
		}
		else if (json_value.is_string())
		{
			LString str_val = json_value.to_string();

			if (str_val.starts_with("L:"))
				link = new LLink(str_val.remove("L:"));

			else
				value = json_value.to_string();
		}
		else if (json_value.is_array())
		{
			LJsonArray array = json_value.to_array();

			if (!array.empty() && array[0].is_string())
			{
				std::vector<LString> gradient_stops;

				for (const auto& val : array)
					gradient_stops.push_back(val.to_string());

				value = gradient_stops;
			}
		}
	}

	void break_link(LObject* parent)
	{
		 if (link && link->attribute())
		 {
		 	value = link->attribute()->value();

			link->attribute()->disconnect_destroyed(m_link_destroyed_connection);

		 	for (auto dep_attr = link->attribute()->pimpl->m_dependent_attrs.begin();
		 		dep_attr != link->attribute()->pimpl->m_dependent_attrs.end(); dep_attr++)
		 	{
		 		if ((*dep_attr)->pimpl == this)
		 		{
		 			dep_attr = link->attribute()->pimpl->m_dependent_attrs.erase(dep_attr);
					link->attribute()->pimpl->update_link_dependencies();
		 			break;
		 		}
		 	}

			delete link;
			link = nullptr;

			update_link_dependencies();
			update_dependencies(parent);
		 }
	}

	void clear_definition_attribute()
	{
		if (def_attr)
		{
			def_attr->disconnect_change(def_connection);
			def_attr = nullptr;
		}
	}

	void create_link(LObject* parent, LAttribute* link_attr)
	{
		value = LVariant();

		break_link(parent);

		link = new LLink(link_attr);

		m_link_destroyed_connection = link_attr->on_destroyed(
			[this] {
				delete link;
				link = nullptr;
			}
		);

		update_link_dependencies();
		update_dependencies(parent);
	}

	void create_state(
		const LString& name, LAttribute* state_attr)
	{
		states[name] = state_attr;
	}

	LAttributeList dependent_attributes(
		bool include_indirect_dependencies) const
	{
		LAttributeList dependent_attributes = m_dependent_attrs;

		if (include_indirect_dependencies)
			for (LAttribute* dependent_attr : m_dependent_attrs)
			{
				LAttributeList attrs =
					dependent_attr->dependent_attributes(
		 				include_indirect_dependencies);

				std::copy(attrs.begin(), attrs.end(),
					std::back_inserter(dependent_attributes));
			}

		return dependent_attributes;
	}

	void disconnect_change(
		const LConnectionID& connection)
	{
		m_change_connections.erase(connection);
	}

	void disconnect_link_change(
		const LConnectionID& connection)
	{
		m_link_change_connections.erase(connection);
	}

	bool has_states() const
	{
		return !states.empty();
	}

	bool is_link(const LString& str) const
	{
		if (str.starts_with("L:"))
			return true;
		
		return false;
	}

	LConnectionID on_change(std::function<void()> callback)
	{
		m_change_connections[m_change_connections_next_id++] = callback;
		return std::prev(m_change_connections.end())->first;
	}

	LConnectionID on_link_change(std::function<void()> callback)
	{
		m_link_change_connections[m_link_change_connections_next_id++] = callback;
		return std::prev(m_link_change_connections.end())->first;
	}

	LAttribute* state(const LStringList& state_combo)
	{
		for (const auto& [key, state_attr] : states)
		{
			auto state_names =
				split<LStringList>(state_attr->object_name(), ':');

			bool qualifies = true;

			for (const auto& state_name : state_names)
			{
				if (std::find(state_combo.begin(), state_combo.end(),
					state_name) == state_combo.end())
				{
					qualifies = false;
				}
			}

			if (qualifies)
				return state_attr;
		}

		// TODO: Handle returning override with highest number of matching
		// states. If there is a conflict (two matching overrides), just
		// return the value of this

		return nullptr;
	}

	void set_definition_attribute(
		LObject* parent, LAttribute* definition_attribute)
	{
		def_attr = definition_attribute;

		def_connection = def_attr->on_change(
			[this, parent] {
				update_dependencies(parent);
			}
		);

		update_dependencies(parent);
	}

	void set_value(LObject* parent, const LVariant& v)
	{
		if (link && link->attribute())
		{
			link->attribute()->set_value(v);
			update_dependencies(parent);
		}

		if (!link && value.index() == v.index())
		{
			bool same_value = false;

			switch (v.index())
			{
			case 0:
				same_value = true;
				break;
			case 1:
				if (std::get<double>(value) == std::get<double>(v))
					same_value = true;
				break;
			case 2:
				if (std::get<bool>(value) == std::get<bool>(v))
					same_value = true;
				break;
			case 3:
				if (std::get<LString>(value) == std::get<LString>(v))
					same_value = true;
				break;
			case 4:
				if (std::get<std::vector<LString>>(value) == std::get<std::vector<LString>>(v))
					same_value = true;
				break;
			}

			if (same_value)
				return;
		}

		value = v;
		update_dependencies(parent);
	}

	LJsonObject to_json_object()
	{
		LJsonObject json_object;

		// if (!m_link_path.empty())
		// {
		// 	json_object["linked_to"] = m_link_path;
		// }
		// else if (m_value.index() > 0)
		// {
		// 	json_object["value"] = to_json_value();
		// }

		// if (has_overrides())
		// {
		// 	LJsonObject overrides_json_object;

		// 	for (const auto& [key, override_attr] : m_overrides)
		// 		overrides_json_object[override_attr->object_name()] =
		// 		override_attr->to_json_object();

		// 	json_object["overrides"] = overrides_json_object;
		// }

		return json_object;
	}

	LJsonValue to_json_value()
	{
		LJsonValue json_value;

		// if (const auto& bool_val = std::get_if<bool>(&m_value))
		// 	json_value = *bool_val;

		// else if (const auto& double_val = std::get_if<double>(&m_value))
		// 	json_value = *double_val;

		// else if (const auto& string_val = std::get_if<LString>(&m_value))
		// 	json_value = *string_val;

		// else if (const auto& gradient_stops_val =
		// 	std::get_if<std::vector<LString>>(&m_value))
		// {
		// 	LJsonArray gradient;

		// 	for (auto stop : *gradient_stops_val)
		// 		gradient.push_back(stop);

		// 	json_value = LJsonObject({ {"gradient", gradient } });
		// }

		return json_value;
	}

	size_t type_index() const
	{
		if (def_attr)
			return def_attr->type_index();

		if (!states.empty())
			return (*states.begin()).second->type_index();

		// if (link && link->attribute())
		// 	return link->attribute()->type_index();

		return value.index();
	}

	void update_dependencies(LObject* parent)
	{
		 for (auto& change_function : m_change_connections)
		 	change_function.second();

		 if (!m_dependent_attrs.empty())
		 {
		 	for (const auto& dependent_attr : m_dependent_attrs)
		 	{
		 		dependent_attr->update_dependencies();
		 	}
		 }

		if (parent)
		{
			if (LDefinable* d = dynamic_cast<LDefinable*>(parent))
			{
				d->update();
			}
			else if (LAttribute* a = dynamic_cast<LAttribute*>(parent))
			{
				a->update_dependencies();
			}
		}
	}

	void update_link_dependencies()
	{
		 for (auto& link_change_function : m_link_change_connections)
		 {
		 	link_change_function.second();
		 }

		 for (LAttribute* dependent_attr : dependent_attributes(true))
		 {
		 	dependent_attr->pimpl->update_link_dependencies();
		 }
	}

	void update_json_object()
	{
		// if (!m_link_path.empty())
		// {
		// 	m_json_object["linked_to"] = m_link_path;
		// }
		// else if (m_value.index() > 0)
		// {
		// 	m_json_object["value"] = to_json_value();
		// }
	}
};

LAttribute::LAttribute(
	const LString& name, LObject* parent) :
	pimpl{ new Impl(name) }, LObject(parent)
{
	set_object_name(name);
}

LAttribute::LAttribute(
	const LString& name, double value, LObject* parent) :
	pimpl{ new Impl(name, value) }, LObject(parent)
{
	set_object_name(name);
}

LAttribute::LAttribute(
	const LString& name, const char* value, LObject* parent) :
	pimpl{ new Impl(name, value) }, LObject(parent)
{
	set_object_name(name);
}

LAttribute::LAttribute(
	const LString& name, const LVariant& value, LObject* parent) :
	pimpl{ new Impl(name, value) }, LObject(parent)
{
	set_object_name(name);
}

LAttribute::LAttribute(
	const LString& name, LJsonValue value, LObject* parent) :
	pimpl{ new Impl(name, value) }, LObject(parent)
{
	set_object_name(name);

	if (value.is_object())
	{
		LJsonObject json_object = value.to_object();

		if (json_object.find("states") != json_object.end())
		{
			LJsonObject states_obj = json_object["states"].to_object();

			for (const auto& [key, state_val] : states_obj)
			{
				LAttribute* state_attr =
					new LAttribute(key, state_val, this);

				pimpl->states[key] = state_attr;
			}
		}
	}
}

LAttribute::~LAttribute()
{
	delete pimpl;
}

void LAttribute::create_link(LAttribute* link_attr)
{
	pimpl->create_link(parent(), link_attr);

	link_attr->pimpl->m_dependent_attrs.push_back(this);
	link_attr->pimpl->update_link_dependencies();
}

void LAttribute::create_link(LLink* link)
{
	if (link)
		pimpl->link = new LLink(*link);
}

void LAttribute::create_state(const LString& name, const char* value)
{
	create_state(name, LString(value));
}

void LAttribute::create_state(const LString& name, LVariant value)
{
	pimpl->create_state(name, new LAttribute(name, value, this));
}

void LAttribute::break_link()
{
	pimpl->break_link(parent());
}

void LAttribute::clear_states()
{
	if (!pimpl->states.empty())
	{
		for (const auto& [key, state] : pimpl->states)
		{
			remove_child(state);
			delete state;
		}

		pimpl->states.clear();
	}
}

void LAttribute::clear_definition_attribute()
{
	pimpl->clear_definition_attribute();
}

LAttributeList LAttribute::dependent_attributes(
	bool include_indirect_dependencies) const
{
	return pimpl->dependent_attributes(include_indirect_dependencies);
}

void LAttribute::disconnect_change(const LConnectionID& connection)
{
	pimpl->disconnect_change(connection);
}

void LAttribute::disconnect_link_change(const LConnectionID& connection)
{
	pimpl->disconnect_link_change(connection);
}

bool LAttribute::has_states() const
{
	return pimpl->has_states();
}

LConnectionID LAttribute::on_change(std::function<void()> callback)
{
	return pimpl->on_change(callback);
}

LConnectionID LAttribute::on_link_change(std::function<void()> callback)
{
	return pimpl->on_link_change(callback);
}

LAttribute* LAttribute::state(const LStringList& state_combo)
{
	return pimpl->state(state_combo);
}

LAttributeMap LAttribute::states() const
{
	LAttributeMap states;

	states.insert(pimpl->states.begin(), pimpl->states.end());

	if (parent())
	{
		if (LDefinition* parent_as_def = dynamic_cast<LDefinition*>(parent()))
		{
			if (parent_as_def->base())
			{
				if (LAttribute* base_attr = parent_as_def->base()->find_attribute(object_name()))
				{
					LAttributeMap base_states = base_attr->states();

					states.insert(base_states.begin(), base_states.end());
				}
			}
		}
	}

	return states;
}

LString LAttribute::path() const
{
	if (parent())
	{
		if (LAttribute* parent_attr = dynamic_cast<LAttribute*>(parent()))
			return parent_attr->path() + "." + object_name();
		else if (LDefinable* parent_themeable = dynamic_cast<LDefinable*>(parent()))
			return parent_themeable->path() + "/" + object_name();
		else if (LDefinition* parent_theme_item = dynamic_cast<LDefinition*>(parent()))
			return parent_theme_item->path() + "/" + object_name();
	}

	return object_name();
}

void LAttribute::resolve_links()
{
	if (pimpl->link)
	{
		if (!pimpl->link->resolve(this));
		// TODO: Handle link resolution failure

		if (pimpl->link->attribute())
			pimpl->link->attribute()->pimpl->m_dependent_attrs.push_back(this);
			//link_attr->pimpl->update_link_dependencies();
	}

	for (const auto& [key, state] : pimpl->states)
		state->resolve_links();

	update_dependencies();
}

void LAttribute::set_definition_attribute(LAttribute* definition_attribute)
{
	pimpl->set_definition_attribute(parent(), definition_attribute);
}

void LAttribute::set_value(const char* value)
{
	set_value(LString(value));
}

void LAttribute::set_value(const LVariant& value)
{
	pimpl->set_value(parent(), value);
}

LLink* LAttribute::link() const
{
	return pimpl->link;
}

LAttribute* LAttribute::definition_attribute() const
{
	return pimpl->def_attr;
}

LJsonObject LAttribute::to_json_object()
{
	return pimpl->to_json_object();
}

LJsonValue LAttribute::to_json_value()
{
	return pimpl->to_json_value();
}

size_t LAttribute::type_index() const
{
	return pimpl->type_index();
}

const LVariant& LAttribute::value()
{
	 if (pimpl->link && pimpl->link->attribute())
	 	return pimpl->link->attribute()->value();

	return pimpl->value;
}

void LAttribute::update_dependencies()
{
	pimpl->update_dependencies(parent());
}

LAYERS_EXPORT LAttributeMap Layers::attributes_from_json(const LJsonValue& json_val, LObject* parent)
{
	LAttributeMap attributes;

	LJsonObject json_obj = json_val.to_object();

	for (const auto& [key, attr] : json_obj)
		attributes[key] = new LAttribute(key, json_obj[key]);

	return attributes;
}
