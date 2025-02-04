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

#include <Layers/lpaths.h>

std::filesystem::path Layers::app_path(const std::string& app_name)
{
	return local_app_data_path() / app_name;
}

std::filesystem::path Layers::latest_T_version_path()
{
	return T2_path();
}

std::filesystem::path Layers::layers_path()
{
	return local_app_data_path() / "Layers";
}

std::filesystem::path Layers::definitions_path()
{
	return layers_path() / "Definitions";
}

std::filesystem::path Layers::themes_path()
{
	return layers_path() / "Themes";
}

std::filesystem::path Layers::styles_path()
{
	return layers_path() / "Styles";
}

std::filesystem::path Layers::T1_path()
{
	return themes_path() / "T1";
}

std::filesystem::path Layers::T2_path()
{
	return themes_path() / "T2";
}

std::filesystem::path Layers::local_app_data_path()
{
#ifdef _WIN32
	char* app_data_ptr = nullptr;
	size_t app_data_size = 0;
	if (_dupenv_s(&app_data_ptr, &app_data_size, "LOCALAPPDATA") != 0 ||
		app_data_ptr == nullptr)
	{
		throw std::runtime_error(
			"Failed to retrieve LOCALAPPDATA environment variable.");
	}

	std::string app_data_str = app_data_ptr;
	delete[] app_data_ptr;
	app_data_ptr = nullptr;

	return std::filesystem::path(app_data_str);
#else
    const char* home_dir = getenv("HOME");
    if (home_dir == nullptr)
    {
        throw std::runtime_error(
            "Failed to retrieve HOME environment variable.");
    }
    return std::filesystem::path(home_dir);
#endif
}
