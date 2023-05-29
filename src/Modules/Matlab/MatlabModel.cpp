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

#include "MatlabModel.h"

#include <Headers/Globals.h>
#include <Modules/Dbc/DBCModel.h>
#include <Modules/Dbc/DBCFile.h>

#include <Modules/Dbc/DBCObjects/DBCSignal.h>
#include <Modules/Dbc/DBCObjects/DBCMessage.h>

////////////////////////////////////////////////////
//// CTOR/DTOR /////////////////////////////////////
////////////////////////////////////////////////////

MatlabModel::MatlabModel()
{

}

MatlabModel::~MatlabModel()
{

}

////////////////////////////////////////////////////
//// INIT/UNIT /////////////////////////////////////
////////////////////////////////////////////////////

bool MatlabModel::Init()
{
	return true;
}

void MatlabModel::Unit()
{

}

void MatlabModel::Clear()
{
	m_str_BitUnpacking_BitPatterns.clear();
	m_str_BitUnpacking_InputPort.clear();
	m_str_BitUnpacking_OutputPort_datatype.clear();
	m_str_BitUnpacking_OutputPort_cellarray.clear();
}

////////////////////////////////////////////////////
//// CONNECTION TO THE MODEL ///////////////////////
////////////////////////////////////////////////////

void MatlabModel::ConnectToDBCModel(ct::cWeak<DBCModel> vDBCModel)
{
	m_DBCModel = vDBCModel;
}

////////////////////////////////////////////////////
//// GET CONTAINERS ////////////////////////////////
////////////////////////////////////////////////////

std::string& MatlabModel::GetBitUnpackingErrors()
{
	return m_BitUnpacking_Error_File;
}

std::vector<std::pair<std::string, std::string>>& MatlabModel::GetBitUnpackingBitPatterns()
{
	return m_str_BitUnpacking_BitPatterns;
}

std::string& MatlabModel::GetBitUnpackingInputPort()
{
	return m_str_BitUnpacking_InputPort;
}

std::vector<std::pair<std::string, std::string>>& MatlabModel::GetBitUnpackingOutputPortDataType()
{
	return m_str_BitUnpacking_OutputPort_datatype;
}

std::vector<std::pair<std::string, std::string>>& MatlabModel::GetBitUnpackingOutputPortCellArray()
{
	return m_str_BitUnpacking_OutputPort_cellarray;
}

char* MatlabModel::GetDelimiterBuffer()
{
	return m_Delimiter_Buffer;
}

////////////////////////////////////////////////////
//// COMPUTATION ///////////////////////////////////
////////////////////////////////////////////////////

std::string MatlabModel::Get_Simulink_DataType(
	const std::string& vMessageName,
	const std::string& vSignalName,
	const dbcppp::ISignal::EValueType& vSign,
	const dbcppp::ISignal::EExtendedValueType& vExtSign,
	const uint64_t& vBitCount)
{
	// https://fr.mathworks.com/help/simulink/ug/data-types-supported-by-simulink.html?searchHighlight=simulink%20data%20types&s_tid=srchtitle_simulink%20data%20types_1
	// possibles type : uint32 (default) | double | single | int8 | uint8 | int16 | uint16 | int32 | boolean

	if (vExtSign == dbcppp::ISignal::EExtendedValueType::Double)
	{
		return "double";
	}
	else if (vExtSign == dbcppp::ISignal::EExtendedValueType::Float)
	{
		return "single";
	}
	else if (vExtSign == dbcppp::ISignal::EExtendedValueType::Integer)
	{
		if (vSign == dbcppp::ISignal::EValueType::Unsigned)
		{
			if (vBitCount <= 1) return "boolean";
			if (vBitCount <= 8) return "uint8";
			if (vBitCount <= 16) return "uint16";
			if (vBitCount <= 32) return "uint32";
			if (vBitCount <= 64) return "uint64";
		}
		else
		{
			if (vBitCount <= 1)
			{
				m_BitUnpacking_Error_File += ct::toStr("ERROR : Message %s Signal %s : Bit count of 1 signed is not possible\n", vMessageName.c_str(), vSignalName.c_str());
				return ct::toStr("%s:%s:size 1 signed => error", vMessageName.c_str(), vSignalName.c_str());
			}
			if (vBitCount <= 8) return "int8";
			if (vBitCount <= 16) return "int16";
			if (vBitCount <= 32) return "int32";
			if (vBitCount <= 64) return "int64";
		}
	}

	m_BitUnpacking_Error_File += ct::toStr("ERROR : Message %s Signal %s : No data type found\n", vMessageName.c_str(), vSignalName.c_str());
	return ct::toStr("%s:%s:no data type found", vMessageName.c_str(), vSignalName.c_str());
}


std::string MatlabModel::Get_Compact_String(const std::vector<std::pair<std::string, std::string>>& vArray)
{
	std::string res;

	for (const auto it : vArray)
	{
		res += it.second;
	}

	if (!res.empty())
	{
		if (strlen(m_Delimiter_Buffer))
		{
			res = res.substr(0U, res.size() - 2U);
			res += "}";
		}
	}

	return res;
}

void MatlabModel::ComputeButUnpacking()
{
#ifdef USE_MATLAB_MODULES
	auto dbcModulePtr = m_DBCModel.getValidShared();
	if (!dbcModulePtr)
		return;
			
	std::string res;

	// on va transformer les messages et les signaux en pos bit absolu
	// https://fr.mathworks.com/help/slrealtime/io_ref/bitunpacking.html?searchHighlight=bit%20unpack&s_tid=srchtitle_bit%20unpack_1
	// possibles type : uint32 (default) | double | single | int8 | uint8 | int16 | uint16 | int32 | boolean
	// Bit Patterns : {[0:7][8:15][16:23]}
	// Input port : uint8
	// Input Port : [1]
	// Output Port data types : {'uint8', 'uint8','uint8'}
	// Output Por array size : {[1][1][1]}

	uint64_t start_bit = 0U;
	uint64_t end_bit = 0U;
	uint64_t end_bit_msg = 0U;
	uint64_t full_input_pack_size = 0U;

	size_t vector_idx = 0U;

	m_BitUnpacking_Error_File.clear();

	m_str_BitUnpacking_BitPatterns.clear();
	m_str_BitUnpacking_InputPort.clear();
	m_str_BitUnpacking_OutputPort_datatype.clear();
	m_str_BitUnpacking_OutputPort_cellarray.clear();

	m_str_BitUnpacking_BitPatterns.push_back(std::pair<std::string, std::string>());
	m_str_BitUnpacking_OutputPort_datatype.push_back(std::pair<std::string, std::string>());
	m_str_BitUnpacking_OutputPort_cellarray.push_back(std::pair<std::string, std::string>());

	m_str_BitUnpacking_BitPatterns[vector_idx].second = "{";
	m_str_BitUnpacking_OutputPort_datatype[vector_idx].second = "{";
	m_str_BitUnpacking_OutputPort_cellarray[vector_idx].second = "{";

	for (const auto& msgIter : dbcModulePtr->GetMergedDBCFile()->GetContainerMessagesByIds())
	{
		auto msgPtr = msgIter.second.getValidShared();
		if (msgPtr)
		{
			vector_idx = m_str_BitUnpacking_BitPatterns.size();
			m_str_BitUnpacking_BitPatterns.push_back(std::pair<std::string, std::string>(msgPtr->name, ""));
			m_str_BitUnpacking_OutputPort_datatype.push_back(std::pair<std::string, std::string>(msgPtr->name, ""));
			m_str_BitUnpacking_OutputPort_cellarray.push_back(std::pair<std::string, std::string>(msgPtr->name, ""));

			for (const auto& sigIter : msgPtr->signals)
			{
				auto sigPtr = sigIter.second.getValidShared();
				if (sigPtr)
				{
					const dbcppp::ISignal* sig = sigPtr->m_Signal;
					if (sig)
					{
						start_bit = end_bit_msg + sig->StartBit();
						end_bit = start_bit + sig->BitSize() - 1; // -1 because its not a count but an absolute bit pos

						m_str_BitUnpacking_BitPatterns[vector_idx].second += ct::toStr("[%u:%u]%s", (uint32_t)start_bit, (uint32_t)end_bit, m_Delimiter_Buffer);
						m_str_BitUnpacking_OutputPort_datatype[vector_idx].second += ct::toStr("'%s'%s",
							Get_Simulink_DataType(msgPtr->name, sig->Name(), sig->ValueType(),
								sig->ExtendedValueType(), sig->BitSize()).c_str(), m_Delimiter_Buffer);
						m_str_BitUnpacking_OutputPort_cellarray[vector_idx].second += ct::toStr("[1]%s", m_Delimiter_Buffer);
					}
				}
			}

			end_bit_msg = end_bit + 1;
			end_bit_msg = (uint64_t)ct::ceil(end_bit_msg / 8.0) * 8U;

			full_input_pack_size += msgPtr->full_size;
		}
	}

	vector_idx = m_str_BitUnpacking_BitPatterns.size();
	m_str_BitUnpacking_BitPatterns.push_back(std::pair<std::string, std::string>());
	m_str_BitUnpacking_OutputPort_datatype.push_back(std::pair<std::string, std::string>());
	m_str_BitUnpacking_OutputPort_cellarray.push_back(std::pair<std::string, std::string>());

	m_str_BitUnpacking_BitPatterns[vector_idx].second += "}";
	m_str_BitUnpacking_InputPort = ct::toStr("[%u]", (uint32_t)full_input_pack_size);

	m_str_BitUnpacking_OutputPort_datatype[vector_idx].second += "}";

	m_str_BitUnpacking_OutputPort_cellarray[vector_idx].second += "}";
#endif
}
