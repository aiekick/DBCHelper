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

#include <ctools/ConfigAbstract.h>
#include <ctools/cTools.h>
#include <map>
#include <string>
#include <memory> // std::unique_ptr, etc..
#include <cstdint> // uint8_t, etc..
#include <thread>
#include <atomic>
#include <mutex>

class DBCModel;
class CStructModel
{
private:
	ct::cWeak<DBCModel> m_DBCModel;
	std::string m_CStructTools_Error_File;
	std::map<std::string, std::string> m_str_CStructTools_C_Structures;
	
public:
	CStructModel();
	~CStructModel();

	bool Init();
	void Unit();
	void Clear();

	void ConnectToDBCModel(ct::cWeak<DBCModel> vDBCModel);

	std::map<std::string, std::string>& GetCStructStructures();

	void ComputeCStructStructures();
};
