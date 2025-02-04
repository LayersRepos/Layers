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

#include <Layers/ltheme.h>

#include <fstream>

#include <Layers/lalgorithms.h>
#include <Layers/lattribute.h>
#include <Layers/lgenerate.h>
#include <Layers/lcontroller.h>

using Layers::LAttribute;
using Layers::LAttributeMap;
using Layers::LString;
using Layers::LTheme;
using Layers::LController;
using Layers::LDefinition;

class LTheme::Impl
{
public:
	Impl() {}

	Impl(const std::filesystem::path& directory) :
		m_path{ directory }
	{
		std::ifstream meta_file(m_path / "meta.json");

		if (meta_file.is_open())
		{
			std::stringstream buffer;
			buffer << meta_file.rdbuf();
			std::string meta_data = buffer.str();
			meta_file.close();

			meta_data = remove_whitespace(meta_data);

			LJsonLexer meta_lexer = LJsonLexer(meta_data);
			LJsonParser meta_parser = LJsonParser(meta_lexer);
			LJsonObject meta_obj = meta_parser.parse_object();

			if (meta_obj.find("uuid") != meta_obj.end())
				m_uuid = meta_obj["uuid"].to_string();

			if (meta_obj.find("publisher") != meta_obj.end())
				publisher = meta_obj["publisher"].to_string();

			if (meta_obj.find("editable") != meta_obj.end())
				m_editable = meta_obj["editable"].to_bool();

			if (meta_obj.find("lineage") != meta_obj.end())
				for (LJsonValue lineage_value : meta_obj["lineage"].to_array())
					append_to_lineage(lineage_value.to_string());
		}
		else
		{
			std::cerr << "Could not read theme 'meta.json' file" << std::endl;
		}
	}

	Impl(const LString& name, bool editable = true) :
		m_uuid{ generate_uuid() },
		m_editable{ editable } {}

	Impl(const LString& name, const LString& uuid, bool editable) :
		m_uuid{ uuid },
		m_editable{ editable } {}

	void append_to_lineage(const LString& theme_id)
	{
		m_lineage.push_back(theme_id);
	}

	std::filesystem::path directory() const
	{
		return m_path;
	}

	bool editable() const
	{
		return m_editable;
	}

	bool has_implementation(const LString& app_display_id) const
	{
		std::filesystem::path dir_path = m_path / app_display_id.c_str();
		return std::filesystem::exists(dir_path);
	}

	std::vector<LString> lineage() const
	{
		return m_lineage;
	}

	void save_meta_file()
	{
		LJsonObject json_object;

		LJsonArray lineage_array;

		for (const auto& theme_id : m_lineage)
		{
			lineage_array.emplace_back(theme_id);
		}

		json_object["lineage"] = lineage_array;
		json_object["uuid"] = m_uuid;

		if (!publisher.empty())
		{
			json_object["publisher"] = publisher;
		}

		LJsonValue json_value(json_object);

		std::ofstream meta_file(m_path / "meta.json");

		if (!meta_file.is_open())
		{
			std::cerr << "Could not create theme 'meta.json' file" << std::endl;
			return;
		}

		meta_file << json_value.to_output();
		meta_file.close();
	}

	void set_dir(const std::filesystem::path& directory)
	{
		m_path = directory;
	}

	void set_edit_mode_enabled(bool enabled)
	{
		m_edit_mode_enabled = enabled;
	}

	LString uuid() const
	{
		return m_uuid;
	}

	std::filesystem::path m_path;

	bool m_editable{ true };
	bool m_edit_mode_enabled{ false };

	std::vector<LString> m_lineage;

	LString publisher;
	LString m_uuid;
};

LTheme::LTheme() :
	pimpl{ new Impl() }, LDefinition() {}

LTheme::LTheme(
	const LString& name,
	const LJsonValue& value,
	const std::filesystem::path& file_path,
	LDefinition* parent) :
	pimpl{ new Impl() },
	LDefinition(name, value.to_object(), file_path, parent)
{
	if (value.is_object())
	{
		LJsonObject object = value.to_object();

		if (object.find("_meta") != object.end())
		{
			LJsonObject meta_object = object["_meta"].to_object();

			if (meta_object.find("publisher") != meta_object.end())
				set_publisher(meta_object["publisher"].to_string());
			
			if (meta_object.find("uuid") != meta_object.end())
				pimpl->m_uuid = meta_object["uuid"].to_string();
		}
	}
}

LTheme::~LTheme()
{
	delete pimpl;
}

void LTheme::append_to_lineage(const LString& theme_id)
{
	pimpl->append_to_lineage(theme_id);
}

std::filesystem::path LTheme::directory() const
{
	return pimpl->directory();
}

LString LTheme::display_id() const
{
	if (!pimpl->publisher.empty())
	{
		return object_name() + " (" + pimpl->publisher + ")";
	}
	else if (!pimpl->m_uuid.empty())
	{
		return object_name() + " (" + pimpl->m_uuid + ")";
	}

	return object_name();
}

bool LTheme::editable() const
{
	return pimpl->editable();
}

bool LTheme::has_implementation(const LString& app_display_id) const
{
	return pimpl->has_implementation(app_display_id);
}

std::vector<LString> LTheme::lineage() const
{
	return pimpl->lineage();
}

LString LTheme::publisher() const
{
	return pimpl->publisher;
}

void LTheme::set_publisher(const LString& publisher)
{
	pimpl->publisher = publisher;
}

void LTheme::save_meta_file()
{
	pimpl->save_meta_file();
}

void LTheme::set_dir(const std::filesystem::path& directory)
{
	pimpl->set_dir(directory);
}

void LTheme::set_edit_mode_enabled(bool enabled)
{
	pimpl->set_edit_mode_enabled(enabled);
}

LString LTheme::uuid() const
{
	return pimpl->uuid();
}
