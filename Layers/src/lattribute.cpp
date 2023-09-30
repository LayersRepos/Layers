/*
 * Copyright (C) 2023 The Layers Project
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

#include <QGradientStops>
#include <QJsonArray>

#include <Layers/lapplication.h>

using Layers::LAttribute;
using Layers::LAttributeList;
using Layers::LAttributeMap;

LAttribute::LAttribute(
	const QString& name, QObject* parent) :
	QObject(parent)
{
	setObjectName(name);
}

LAttribute::LAttribute(
	const QString& name, QVariant value, QObject* parent) :
	m_value{ value }, QObject(parent)
{
	setObjectName(name);

	connect(this, &LAttribute::changed, [this]
	{
		if (this->parent())
			if (LThemeable* t = dynamic_cast<LThemeable*>(this->parent()))
				t->update();
	});
}

LAttribute::LAttribute(
	const QString& name, QJsonObject json_object, QObject* parent) :
	m_json_object{ json_object }, QObject(parent)
{
	setObjectName(name);

	if (json_object.contains("linked_to"))
	{
		set_link_path(json_object.value("linked_to").toString());
	}
	else if (json_object.contains("value"))
	{
		QJsonValue json_value = json_object.value("value");

		if (json_value.isBool())
			m_value = QVariant::fromValue(json_value.toBool());

		else if (json_value.isDouble())
			m_value = QVariant::fromValue(json_value.toDouble());

		else if (json_value.isString())
		{
			QString value_str = json_value.toString();

			if (value_str.startsWith("#"))
				m_value = QVariant::fromValue(QColor(value_str));
		}
		else if (json_value.isObject())
		{
			QJsonObject value_object = json_value.toObject();

			if (value_object.contains("gradient"))
			{
				QJsonArray gradient_array = value_object.value("gradient").toArray();

				QGradientStops gradient_stops;

				for (QJsonValueConstRef array_value : gradient_array)
				{
					QString array_string = array_value.toString();

					QStringList string_parts = array_string.split(":");

					gradient_stops.append(
						QGradientStop(
							string_parts[0].toDouble(),
							string_parts[1]
						)
					);
				}

				m_value = QVariant::fromValue(gradient_stops);
			}
		}
	}
	
	if (json_object.contains("overrides"))
	{
		QJsonObject overrides_object =
			json_object.value("overrides").toObject();

		for (const QString& key : overrides_object.keys())
		{
			LAttribute* override_attr =
				new LAttribute(key, overrides_object.value(key).toObject(), this);

			m_overrides[key] = override_attr;

			connect(override_attr, &LAttribute::changed, [this]
				{ emit changed(); });
		}
	}

	connect(this, &LAttribute::changed, [this]
		{ m_json_object = to_json_object(); });
}

LAttribute::LAttribute(const LAttribute& attribute) :
	m_link_path{ attribute.m_link_path },
	m_value{ attribute.m_value },
	QObject()
{
	setObjectName(attribute.objectName());

	if (!attribute.m_overrides.isEmpty())
		for (LAttribute* override_attr : attribute.m_overrides)
		{
			LAttribute* copy_override_attr = new LAttribute(*override_attr);

			copy_override_attr->setParent(this);

			m_overrides[override_attr->objectName()] = copy_override_attr;
		}
}

LAttribute::~LAttribute()
{
	disconnect(m_link_connection);
	disconnect(m_link_destroyed_connection);
	disconnect(m_theme_connection);

	if (m_link_attr)
	{
		if (m_link_attr->m_dependent_attrs.contains(this))
		{
			m_link_attr->m_dependent_attrs.removeOne(this);
			emit m_link_attr->link_changed();
		}

		m_link_attr = nullptr;
	}
}

void LAttribute::create_override(const QString& name, QVariant value)
{
	LAttribute* override_attr = new LAttribute(name, value, this);

	m_overrides[name] = override_attr;

	connect(override_attr, &LAttribute::changed, [this]
		{ emit changed(); });
}

void LAttribute::break_link()
{
	if (m_link_attr)
	{
		m_value = m_link_attr->value();

		disconnect(m_link_connection);
		disconnect(m_link_destroyed_connection);

		if (m_link_attr->m_dependent_attrs.contains(this))
		{
			m_link_attr->m_dependent_attrs.removeOne(this);
			emit m_link_attr->link_changed();
		}

		m_link_attr = nullptr;
		m_link_path = "";
	}

	emit_link_changed();
	emit changed();
}

void LAttribute::clear_overrides()
{
	if (!m_overrides.isEmpty())
	{
		for (LAttribute* override_attr : m_overrides)
			delete override_attr;

		m_overrides.clear();
	}
}

void LAttribute::clear_theme_attribute()
{
	m_theme_attr = nullptr;

	disconnect(m_theme_connection);
}

LAttributeList LAttribute::dependent_attributes(
	bool include_indirect_dependencies) const
{
	LAttributeList dependent_attributes = m_dependent_attrs;

	if (include_indirect_dependencies)
		for (LAttribute* dependent_attr : m_dependent_attrs)
			dependent_attributes.append(
				dependent_attr->dependent_attributes(
					include_indirect_dependencies));

	return dependent_attributes;
}

bool LAttribute::has_overrides() const
{
	return !m_overrides.isEmpty();
}

QJsonObject& LAttribute::json_object()
{
	return m_json_object;
}

LAttribute* LAttribute::link_attribute() const
{
	return m_link_attr;
}

QString LAttribute::link_path() const
{
	return m_link_path;
}

LAttribute* LAttribute::override_attribute(const QStringList& state_combo)
{
	for (LAttribute* override_attr : m_overrides)
	{
		QStringList override_states = override_attr->objectName().split(":");

		bool qualifies = true;

		for (const QString& override_state : override_states)
			if (!state_combo.contains(override_state))
				qualifies = false;

		if (qualifies)
			return override_attr;
	}

	// TODO: Handle returning override with highest number of matching
	// states. If there is a conflict (two matching overrides), just
	// return the value of this

	return nullptr;
}

LAttributeMap LAttribute::overrides() const
{
	return m_overrides;
}

QString LAttribute::path() const
{
	if (parent())
	{
		if (LAttribute* parent_attr = dynamic_cast<LAttribute*>(parent()))
			return parent_attr->path() + "." + objectName();
		else if (LThemeable* parent_themeable = dynamic_cast<LThemeable*>(parent()))
			return parent_themeable->path() + "/" + objectName();
		else if (LThemeItem* parent_theme_item = dynamic_cast<LThemeItem*>(parent()))
			return parent_theme_item->path() + "/" + objectName();
	}

	return objectName();
}

void LAttribute::set_link_attribute(LAttribute* link_attr)
{
	m_value = QVariant();

	if (m_link_attr)
	{
		if (m_link_attr->m_dependent_attrs.contains(this))
			m_link_attr->m_dependent_attrs.removeOne(this);

		m_link_attr = nullptr;
	}

	m_link_attr = link_attr;

	m_link_attr->m_dependent_attrs.append(this);
	emit m_link_attr->link_changed();

	establish_link_connections();

	emit_link_changed();
	emit changed();
}

void LAttribute::set_link_path(const QString& link_path)
{
	m_link_path = link_path;

	m_value = QVariant();
}

void LAttribute::set_theme_attribute(LAttribute* theme_attr)
{
	m_theme_attr = theme_attr;

	establish_theme_connection();

	emit changed();
}

void LAttribute::set_value(QVariant value)
{
	if (m_link_attr)
		m_link_attr->set_value(value);
	else if (m_value != value)
		m_value = value;

	emit changed();
}

QJsonObject LAttribute::to_json_object()
{
	QJsonObject json_object;

	if (!m_link_path.isEmpty())
	{
		json_object.insert("linked_to", m_link_path);
	}
	else if (m_value.isValid())
	{
		json_object.insert("value", to_json_value());
	}

	if (has_overrides())
	{
		QJsonObject overrides_json_object;

		for (LAttribute* override_attr : m_overrides)
			overrides_json_object.insert(
				override_attr->objectName(), override_attr->to_json_object());

		json_object.insert("overrides", overrides_json_object);
	}

	return json_object;
}

QJsonValue LAttribute::to_json_value()
{
	QJsonValue json_value;

	if (typeName() == "bool")
		json_value = QJsonValue(m_value.value<bool>());

	else if (typeName() == "double")
		json_value = QJsonValue(m_value.value<double>());

	else if (typeName() == "QString")
		json_value = QJsonValue(m_value.value<QString>());

	else if (typeName() == "QColor")
	{
		QColor color = m_value.value<QColor>();

		if (color.alpha() == 255)
			json_value = QJsonValue(color.name());
		else
			json_value = QJsonValue(color.name(QColor::HexArgb));
	}
	else if (typeName() == "QList<std::pair<double,QColor>>")
	{
		QJsonObject gradient_json_object;
		QJsonArray gradient_json_array;

		QGradientStops gradient_stops = m_value.value<QGradientStops>();

		for (const QGradientStop& stop : gradient_stops)
		{
			QString stop_string =
				QString::number(stop.first) + ":" + stop.second.name();

			gradient_json_array.append(QJsonValue(stop_string));
		}

		gradient_json_object.insert("gradient", gradient_json_array);

		json_value = gradient_json_object;
	}

	return json_value;
}

QString LAttribute::typeName(const QStringList& state_combo)
{
	if (m_theme_attr)
		return m_theme_attr->typeName(state_combo);

	if (!m_overrides.isEmpty() && !state_combo.isEmpty())
		if (LAttribute* override_attr = override_attribute(state_combo))
			return override_attr->typeName();

	if (m_link_attr)
		return m_link_attr->typeName(state_combo);

	return m_value.typeName();
}

QVariant LAttribute::value()
{
	if (m_link_attr)
		return m_link_attr->value();

	return m_value;
}

void LAttribute::update_json_object()
{
	if (!m_link_path.isEmpty())
	{
		m_json_object.insert("linked_to", QJsonValue(m_link_path));
	}
	else if (m_value.isValid())
	{
		m_json_object.insert("value", to_json_value());
	}
}

void LAttribute::establish_link_connections()
{
	disconnect(m_link_connection);
	disconnect(m_link_destroyed_connection);

	m_link_connection = connect(m_link_attr, &LAttribute::changed,
		[this] { emit changed(); });

	m_link_destroyed_connection = connect(m_link_attr, &QObject::destroyed,
		[this] { m_link_attr = nullptr; });
}

void LAttribute::establish_theme_connection()
{
	disconnect(m_theme_connection);

	m_theme_connection = connect(m_theme_attr, &LAttribute::changed,
		[this] { emit changed(); });
}

void LAttribute::emit_link_changed()
{
	emit link_changed();

	for (LAttribute* dependent_attr : dependent_attributes(true))
		emit dependent_attr->link_changed();
}
