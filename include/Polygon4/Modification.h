/*
 * Polygon-4 Engine
 * Copyright (C) 2015 lzwdgc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <Polygon4/DataManager/Types.h>

namespace polygon4
{

class P4_ENGINE_API Modification : public detail::Modification
{
    using Base = detail::Modification;

public:
    Modification(const Base &);
    ~Modification();

    virtual bool newGame() override final;
    virtual bool loadGame(const String &filename) override final;

    virtual void spawnMechanoids() override final;
    void spawnCurrentPlayer();

    virtual ScriptEngine *getScriptEngine() const override final { return scriptEngine.get(); }

    bool operator<(const Modification &rhs) const;

protected:
    detail::Map *currentMap = nullptr;
    detail::ModificationPlayer *currentPlayer = nullptr;
    std::unique_ptr<ScriptEngine> scriptEngine;
};

} // namespace polygon4

