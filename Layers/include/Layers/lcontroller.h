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

#ifndef LCONTROLLER_H
#define LCONTROLLER_H

#include <deque>
#include <map>
#include <filesystem>
#include <vector>

#include "layers_global.h"
#include "layers_exports.h"

LAYERS_NAMESPACE_BEGIN

class LDefinition;
class LString;
class LTheme;

class LAYERS_EXPORT LController
{
public:
    LTheme* active_theme() const;

    void add_theme(LTheme* theme);

    LDefinition* find_definition(const LString& path);

	LDefinition* find_definition(std::deque<LString> name_list);

    void include(const LString& path);

    static LController& instance();

    bool set_active_theme(LTheme* theme);

    LTheme* theme(const LString& themeId) const;

    std::map<LString, LTheme*> themes() const;

private:
    LController();
    ~LController();

    LController(const LController&) = delete;
    LController& operator=(const LController&) = delete;

    class Impl;
    Impl* pimpl;
};

#define lController (Layers::LController::instance())

LAYERS_NAMESPACE_END

#endif // LCONTROLLER_H
