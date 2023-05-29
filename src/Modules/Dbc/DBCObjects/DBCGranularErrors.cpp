/*
Copyright 2021-2023 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "DBCGranularErrors.h"

#include <Modules/Dbc/DBCModel.h>

void DBCGranularErrors::AddError(const DBCErrorTypeFlags& vType, const std::string& vError)
{
	if (!vError.empty())
	{
		//auto wstr = ct::string_to_wstring(vError);
		errors.push_back(DBCErrorTypeString(vType, vError));
	}
		
}

void DBCGranularErrors::Clear()
{
	errors.clear();
}

uint32_t DBCGranularErrors::GetCountErrors()
{
	uint32_t res = 0U;

	for (auto err : errors)
	{
		if ((int32_t)err.first & (int32_t)DBCModel::s_DBCErrorTypeFlags)
		{
			res++;
		}
	}

	return res;
}

void DBCGranularErrors::DrawErrors()
{
	for (auto err : errors)
	{
		if ((int32_t)err.first & (int32_t)DBCModel::s_DBCErrorTypeFlags)
		{
			DrawOneError(err.second);
		}
	}
}