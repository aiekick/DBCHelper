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

#pragma once

#include <ctools/cTools.h>
#include <map>
#include <string>
#include <memory> // std::unique_ptr, etc..
#include <cstdint> // uint8_t, etc..
#include <thread>
#include <atomic>
#include <mutex>

#include <dbcppp/include/dbcppp/CApi.h>
#include <dbcppp/include/dbcppp/Network.h>

class DBCModel;
class MatlabModel
{
private:
	ct::cWeak<DBCModel> m_DBCModel;
	
private:
	std::string m_BitUnpacking_Error_File;
	std::vector<std::pair<std::string, std::string>> m_str_BitUnpacking_BitPatterns;
	std::string m_str_BitUnpacking_InputPort;
	std::vector<std::pair<std::string, std::string>> m_str_BitUnpacking_OutputPort_datatype;
	std::vector<std::pair<std::string, std::string>> m_str_BitUnpacking_OutputPort_cellarray;
	char m_Delimiter_Buffer[1+1] = ",";

public:
	MatlabModel();
	~MatlabModel();

	bool Init();
	void Unit();
	void Clear();

	void ConnectToDBCModel(ct::cWeak<DBCModel> vDBCModel);

	void ComputeButUnpacking();

	std::string& GetBitUnpackingErrors();
	std::vector<std::pair<std::string, std::string>>& GetBitUnpackingBitPatterns();
	std::string& GetBitUnpackingInputPort();
	std::vector<std::pair<std::string, std::string>>& GetBitUnpackingOutputPortDataType();
	std::vector<std::pair<std::string, std::string>>& GetBitUnpackingOutputPortCellArray();
	char* GetDelimiterBuffer();

	std::string Get_Compact_String(
		const std::vector<std::pair<std::string, std::string>>& vArray);

private:
	std::string Get_Simulink_DataType(
		const std::string& vMessageName,
		const std::string& vSignalName,
		const dbcppp::ISignal::EValueType& vSign,
		const dbcppp::ISignal::EExtendedValueType& vExtSign,
		const uint64_t& vBitCount);
	
};