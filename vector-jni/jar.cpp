/*
 * Copyright (C) 2015 Andrey Rychkov <wholegroup@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "stdafx.h"
#include "jar.h"

#define STRINGIFY(x) L ## #x
#define TOSTRING(x) STRINGIFY(x)

::std::wstring const com::wholegroup::vector::jni::VECTOR_CORE_JAR(
	L"vector-core-" TOSTRING( PROJECT_VERSION ) L".jar");

#undef TOSTRING
#undef STRINGIFY

