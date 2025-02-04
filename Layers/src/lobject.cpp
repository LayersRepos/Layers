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

#include <Layers/lobject.h>

using Layers::LConnectionID;
using Layers::LConnections;
using Layers::LObject;
using Layers::LString;

class LObject::Impl
{
public:
	~Impl()
	{
		for (auto& destroyed_connection : destroyed_connections)
		{
			destroyed_connection.second();
		}

		for (LObject* child : std::vector<LObject*>(children))
		{
			delete child;
		}
	}

	void add_child(LObject* child)
	{
		children.push_back(child);
	}

	void disconnect_destroyed(const LConnectionID& connection)
	{
		destroyed_connections.erase(connection);
	}

	LConnectionID on_destroyed(std::function<void()> callback)
	{
		destroyed_connections[destroyed_connections_next_id++] = callback;
		return std::prev(destroyed_connections.end())->first;
	}

	void remove_child(LObject* child)
	{
		children.erase(
			std::remove(children.begin(), children.end(), child),
			children.end());
	}

	void set_object_name(const LString& new_name)
	{
		object_name = new_name;
	}

	LString object_name;

	LObject* parent{ nullptr };
	std::vector<LObject*> children;

	LConnections destroyed_connections;
	LConnectionID destroyed_connections_next_id;
};

LObject::LObject(LObject* parent) :
	pimpl{ new Impl() }
{
	set_parent(parent);
}

LObject::~LObject()
{
	if (pimpl->parent)
	{
		pimpl->parent->remove_child(this);
	}

	delete pimpl;
}

void LObject::add_child(LObject* child)
{
	pimpl->add_child(child);
}

std::vector<LObject*>& LObject::children()
{
	return pimpl->children;
}

void LObject::disconnect_destroyed(const LConnectionID& connection)
{
	pimpl->disconnect_destroyed(connection);
}

//template<typename T>
//inline std::vector<T*> LObject::find_children(bool recursive)
//{
//	return pimpl->find_children<T>(recursive);
//}

LString LObject::object_name() const
{
	return pimpl->object_name;
}

LConnectionID LObject::on_destroyed(std::function<void()> callback)
{
	return pimpl->on_destroyed(callback);
}

LObject* LObject::parent() const
{
	return pimpl->parent;
}

void LObject::remove_child(LObject* child)
{
	pimpl->remove_child(child);
}

void LObject::set_object_name(const LString& object_name)
{
	pimpl->set_object_name(object_name);
}

void LObject::set_parent(LObject* parent)
{
	if (pimpl->parent != nullptr)
	{
		pimpl->parent->remove_child(this);
	}

	pimpl->parent = parent;

	if (pimpl->parent != nullptr)
	{
		pimpl->parent->add_child(this);
	}
}
