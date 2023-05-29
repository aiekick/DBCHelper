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

#include "CStructModel.h"

#include <Headers/Globals.h>
#include <Modules/Dbc/DBCModel.h>
#include <Modules/Dbc/DBCFile.h>

#include <Modules/Dbc/DBCObjects/DBCSignal.h>
#include <Modules/Dbc/DBCObjects/DBCMessage.h>

////////////////////////////////////////////////////
//// CTOR/DTOR /////////////////////////////////////
////////////////////////////////////////////////////

CStructModel::CStructModel()
{

}

CStructModel::~CStructModel()
{

}

////////////////////////////////////////////////////
//// INIT/UNIT /////////////////////////////////////
////////////////////////////////////////////////////

bool CStructModel::Init()
{
	return true;
}

void CStructModel::Unit()
{

}

void CStructModel::Clear()
{

}

////////////////////////////////////////////////////
//// CONNECTION TO THE MODEL ///////////////////////
////////////////////////////////////////////////////

void CStructModel::ConnectToDBCModel(ct::cWeak<DBCModel> vDBCModel)
{
	m_DBCModel = vDBCModel;
}

////////////////////////////////////////////////////
//// GET CONTAINER /////////////////////////////////
////////////////////////////////////////////////////

std::map<std::string, std::string>& CStructModel::GetCStructStructures()
{
	return m_str_CStructTools_C_Structures;
}

////////////////////////////////////////////////////
//// COMPUTATION ///////////////////////////////////
////////////////////////////////////////////////////

void CStructModel::ComputeCStructStructures()
{
#ifdef USE_CSTRUCT_MODULES
	auto dbcModulePtr = m_DBCModel.getValidShared();
	if (!dbcModulePtr)
		return;

	m_CStructTools_Error_File.clear();
	m_str_CStructTools_C_Structures.clear();

	for (const auto& msgIter : dbcModulePtr->GetMergedDBCFile()->GetContainerMessagesByIds())
	{
		auto msgPtr = msgIter.second.getValidShared();
		if (msgPtr)
		{
			const auto& upper_name = ct::toUpper(msgPtr->name);
			std::string str = ct::toStr("#define CAN_MSG_ID_%s 0x%02hhx\n", upper_name.c_str(), msgPtr->id);
			str += ct::toStr("typedef struct _%s_Struct {\n", msgPtr->name.c_str());

			int32_t last_bit = 0;
			int32_t count_pads = 0;
			for (const auto& sigIter : msgPtr->signals)
			{
				auto sigPtr = sigIter.second.getValidShared();
				if (sigPtr)
				{
					const dbcppp::ISignal* sig = sigPtr->m_Signal;
					if (sig)
					{
						const int32_t& sb = (int32_t)sig->StartBit();
						const int32_t& bs = (int32_t)sig->BitSize();
						const auto& vt = sig->ValueType();

						if (sb - last_bit)
						{
							str += ct::toStr("\tint _pad%i : %i;\n", count_pads, (sb - last_bit));
							count_pads++;
						}

						if (vt == dbcppp::ISignal::EValueType::Signed)
						{
							str += ct::toStr("\tint %s : %i;\n", sig->Name().c_str(), bs);
						}
						else if (vt == dbcppp::ISignal::EValueType::Unsigned)
						{
							str += ct::toStr("\tunsigned int %s : %i;\n", sig->Name().c_str(), bs);
						}

						last_bit = sb + bs;
					}
				}
			}

			str += ct::toStr("} %s_Struct;\n", msgPtr->name.c_str());

			m_str_CStructTools_C_Structures[msgPtr->name] = str;
		}
	}
#endif
}