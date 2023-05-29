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

#include "DBCModel.h"
#include <ctools/FileHelper.h>
#include <Modules/Dbc/DBCFile.h>
#include <Modules/Dbc/DBCExporter.h>
#include <fstream>

////////////////////////////////////////////////////
//// STATIC ////////////////////////////////////////
////////////////////////////////////////////////////

std::shared_ptr<DBCModel> DBCModel::Create()
{
	auto res = std::make_shared<DBCModel>();
	res->m_This = res;
	if (!res->Init())
	{
		res.reset();
	}
	return res;
}

DBCErrorTypeFlags DBCModel::s_DBCErrorTypeFlags = DBCErrorTypeFlags::DBC_ERROR_TYPE_DEFAULT_ALL;

////////////////////////////////////////////////////
//// CTOR/DTOR /////////////////////////////////////
////////////////////////////////////////////////////

DBCModel::DBCModel()
{

}

DBCModel::~DBCModel()
{

}

////////////////////////////////////////////////////
//// INIT/UNIT /////////////////////////////////////
////////////////////////////////////////////////////

bool DBCModel::Init()
{
	return true;
}

void DBCModel::Unit()
{	
	Clear();

	m_MergedDBCFilePtr.reset();
	m_MergedDBCFilePtr = nullptr;
}

void DBCModel::Clear()
{
	m_BaseDBCFilePtr.reset();
	m_BaseDBCFilePtr = nullptr;

	m_MergedDBCFiles.clear();

	m_MergedDBCFilePtr.reset();
	m_MergedDBCFilePtr = nullptr;
}

////////////////////////////////////////////////////
//// LOADER / WRITER ///////////////////////////////
////////////////////////////////////////////////////

bool DBCModel::OpenDBCFile(const std::string& vFilePathName)
{
	if (!vFilePathName.empty())
	{
		auto ps = FileHelper::Instance()->ParsePathFileName(vFilePathName);
		if (ps.isOk)
		{
			m_MergedDBCFiles.clear();

			m_BaseDBCFilePtr = DBCFile::Create(vFilePathName);
			m_BaseDBCFilePtr->LoadDBCFile(vFilePathName);

			m_MergedDBCFilePtr = DBCFile::Create(vFilePathName);
			m_MergedDBCFilePtr->LoadDBCFile(vFilePathName);

			CheckForErrorsInDBC();
		}
	}

	return IsLoaded();
}

bool DBCModel::MergeDBCFiles(const std::vector<std::string>& vFilePathNames)
{
	if (!vFilePathNames.empty() && m_MergedDBCFilePtr)
	{
		for (const auto& filePathName : vFilePathNames)
		{
			auto ps = FileHelper::Instance()->ParsePathFileName(filePathName);
			if (ps.isOk)
			{
				if (ps.name != m_BaseDBCFilePtr->fileName)
				{
					auto dbcFilePtr = DBCFile::Create(filePathName);
					m_MergedDBCFiles[ps.name] = dbcFilePtr;
					dbcFilePtr->LoadDBCFile(filePathName);

					m_MergedDBCFilePtr->MergeDBCFile(dbcFilePtr);
				}
			}
		}

		CheckForErrorsInDBC();
	}

	return IsLoaded();
}

bool DBCModel::IsLoaded() const
{
	return (m_BaseDBCFilePtr.use_count() != 0);
}

void DBCModel::Reload()
{
	auto main_file = m_BaseDBCFilePtr->filePathName;
	std::vector<std::string> merged_files;
	for (auto file : m_MergedDBCFiles)
	{
		if (file.second)
		{
			merged_files.push_back(file.second->filePathName);
		}
	}

	Clear();

	OpenDBCFile(main_file);
	MergeDBCFiles(merged_files);
}

void DBCModel::RemoveDBC(const std::string& vDBCName)
{
	if (m_MergedDBCFiles.find(vDBCName) != m_MergedDBCFiles.end()) // Found
	{
		m_MergedDBCFiles[vDBCName].reset();
		m_MergedDBCFiles.erase(vDBCName);

		Reload();
	}
}

void DBCModel::CheckForErrorsInDBC()
{
	if (m_BaseDBCFilePtr)
	{
		m_BaseDBCFilePtr->CheckForErrors();
	}

	if (m_MergedDBCFilePtr)
	{
		m_MergedDBCFilePtr->CheckForErrors();
	}

	for (auto dbc : m_MergedDBCFiles)
	{
		if (dbc.second)
		{
			dbc.second->CheckForErrors();
		}
	}
}

////////////////////////////////////////////////////
//// DBC : GETTER //////////////////////////////////
////////////////////////////////////////////////////

std::shared_ptr<DBCFile> DBCModel::GetBaseDBCFile()
{
	return m_BaseDBCFilePtr;
}

std::shared_ptr<DBCFile> DBCModel::GetMergedDBCFile()
{
	return m_MergedDBCFilePtr;
}

std::map<std::string, std::shared_ptr<DBCFile>>& DBCModel::GetContainterMergedDBCFiles()
{
	return m_MergedDBCFiles;
}

////////////////////////////////////////////////////
//// DBC : EXPORT //////////////////////////////////
////////////////////////////////////////////////////

void DBCModel::ExportToFile(const std::string& vFilePathName, const DBCFile_ExportTypeEnum& vDBCFile_ExportTypeEnum)
{
	if (!vFilePathName.empty())
	{
		auto exporter = std::make_shared<DBCExporter>();
		if (exporter)
		{
			bool _can_we_export = exporter->Load(m_BaseDBCFilePtr);

			for (auto dbc : m_MergedDBCFiles)
			{
				_can_we_export &= exporter->Merge(dbc.second);
			}

			if (_can_we_export)
			{
				if (exporter->ExportToFile(vFilePathName, vDBCFile_ExportTypeEnum))
				{
					// ce connard dans dbcppp fait des uniques partout du coup obligé de faire des move partout
					// du coup il va falloir faire un reload derriere. pour recrer les objects qui du coup n'existe plus
					//Reload();
				}
			}

			exporter.reset();
		}
	}
}