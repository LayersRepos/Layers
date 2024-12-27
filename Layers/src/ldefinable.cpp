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

#include <Layers/ldefinable.h>

#include <Layers/lattribute.h>

using Layers::LAttribute;
using Layers::LDefinition;
using Layers::LDefinable;

class LDefinable::Impl
{
public:
	Impl() = default;

	Impl(const Impl& other) {}

	void add_share_definable(LDefinable* themeable)
	{
		if (themeable)
		{
			if (m_definition)
				themeable->apply_definition(m_definition);

			m_share_themeables.push_back(themeable);
		}
	}

	void apply_definition(LDefinable* t, LDefinition* definition)
	{
		if (definition)
		{
			if (t->object_name() == definition->object_name())
			{
				m_definition = definition;

				const auto& attributes_map = definition->attributes();
				if (!attributes_map.empty())
				{
					for (LAttribute* attr : t->find_children<LAttribute>())
					{
						auto it = attributes_map.find(attr->object_name());
						if (it != attributes_map.end())
						{
							attr->set_definition_attribute(it->second);
						}
					}
				}

				const auto& children_map = definition->children();
				if (!children_map.empty())
				{
					for (LDefinable* child_t : t->child_definables())
					{
						auto it = children_map.find(child_t->object_name());
						if (it != children_map.end())
						{
							child_t->apply_definition(it->second);
						}
					}
				}

				for (LDefinable* themeable : m_share_themeables)
				{
					themeable->apply_definition(m_definition);
				}
			}
		}
		else
		{
			m_definition = nullptr;

			for (LAttribute* attr : t->find_children<LAttribute>())
			{
				attr->clear_definition_attribute();
			}

			for (LDefinable* child_t : t->find_children<LDefinable>())
			{
				child_t->apply_definition(nullptr);
			}
		}
	}

	LDefinition* definition() const
	{
		return m_definition;
	}

	LDefinition* m_definition{ nullptr };

	std::vector<LDefinable*> m_share_themeables;
};

LDefinable::LDefinable() :
	pimpl{ new Impl() } {}

LDefinable::LDefinable(const LDefinable& other) :
	pimpl{ new Impl(*(other.pimpl)) } {}

LDefinable::~LDefinable()
{
	if (pimpl)
	{
		delete pimpl;
		pimpl = nullptr;
	}
}

void LDefinable::add_share_definable(LDefinable* themeable)
{
	pimpl->add_share_definable(themeable);
}

void LDefinable::apply_definition(LDefinition* theme_item)
{
	pimpl->apply_definition(this, theme_item);
}

LDefinition* LDefinable::definition() const
{
	return pimpl->definition();
}
