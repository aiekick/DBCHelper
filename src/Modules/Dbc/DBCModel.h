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

#include <Headers/Globals.h>
#include <ctools/cTools.h>
#include <map>
#include <string>
#include <memory> // std::unique_ptr, etc..
#include <cstdint> // uint8_t, etc..
#include <thread>
#include <atomic>
#include <mutex>

class DBCExporter;
class DBCFile;
class DBCModel
{
public: // Static
	static std::shared_ptr<DBCModel> Create();
	static DBCErrorTypeFlags s_DBCErrorTypeFlags;

private:
	ct::cWeak<DBCModel> m_This;
	std::shared_ptr<DBCFile> m_BaseDBCFilePtr = nullptr;
	std::map<std::string, std::shared_ptr<DBCFile>> m_MergedDBCFiles; // file name, dbc
	std::shared_ptr<DBCFile> m_MergedDBCFilePtr = nullptr; // the merge of all dbc files

public:
	DBCModel();
	~DBCModel();

	bool Init();
	void Unit();
	void Clear();

	bool OpenDBCFile(const std::string& vFilePathName);
	bool MergeDBCFiles(const std::vector<std::string>& vFilePathNames);
	bool IsLoaded() const;

	// will relaod all files
	void Reload();

	void RemoveDBC(const std::string& vDBCName);

	void CheckForErrorsInDBC();

	std::shared_ptr<DBCFile> GetBaseDBCFile();
	std::shared_ptr<DBCFile> GetMergedDBCFile();
	std::map<std::string, std::shared_ptr<DBCFile>>& GetContainterMergedDBCFiles();

	void ExportToFile(const std::string& vFilePathName, const DBCFile_ExportTypeEnum& vDBCFile_ExportTypeEnum);
};
