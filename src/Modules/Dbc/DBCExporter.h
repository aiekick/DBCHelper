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
#include <memory> // shared_ptr, etc..
#include <string>
#include <vector>

#include <dbcppp/include/dbcppp/CApi.h>
#include <dbcppp/include/dbcppp/Network.h>

/// <summary>
/// The export alwasy succeed with DBCPPP
/// but the resulting file can be impossibile to read in CANDB++ due to some collision on names
/// can be node names, message names, and definitions attribute.
/// this calss will extract all these params, will unique them and create a good export than dbcppp cant do
/// Unfortunalty DBCPPP is the only lib i seen on internet who is free and can write DBC File. 
/// And i have no time to write my writing lib at this moment..
/// </summary>
class DBCExporter
{
private:
	std::string m_Version;
	std::vector<std::string> m_NewSymbols;
	std::unique_ptr<dbcppp::IBitTiming> m_BitTiming;
	std::vector<std::unique_ptr<dbcppp::INode>> m_Nodes;
	std::vector<std::unique_ptr<dbcppp::IValueTable>> m_ValueTables;
	std::vector<std::unique_ptr<dbcppp::IMessage>> m_Messages;
	std::vector<std::unique_ptr<dbcppp::IEnvironmentVariable>> m_EnvironmentVariables;
	std::vector<std::unique_ptr<dbcppp::IAttributeDefinition>> m_AttributeDefinitions;
	std::vector<std::unique_ptr<dbcppp::IAttribute>> m_AttributeDefaults;
	std::vector<std::unique_ptr<dbcppp::IAttribute>> m_AttributeValues;
	std::string m_Comment;

	std::unique_ptr<dbcppp::INetwork> m_TmpNetworkPtr = nullptr;

public:
	/// <summary>
	/// clear all varaiables
	/// </summary>
	void Clear();

	/// <summary>
	/// will load the base file for the merge
	/// </summary>
	/// <param name="vDBCFilePtr"></param>
	/// <returns></returns>
	bool Load(std::shared_ptr<DBCFile> vDBCFilePtr);

	/// <summary>
	/// will merge the file into the base
	/// </summary>
	/// <param name="vDBCFilePtr"></param>
	/// <returns></returns>
	bool Merge(std::shared_ptr<DBCFile> vDBCFilePtr);

	/// <summary>
	/// will export to file accroding to export types
	/// </summary>
	/// <param name="vFilePathName"></param>
	/// <param name="vDBCFile_ExportTypeEnum"></param>
	/// <returns></returns>
	bool ExportToFile(const std::string& vFilePathName, const DBCFile_ExportTypeEnum& vDBCFile_ExportTypeEnum);

private:
	/// <summary>
	/// export to DBC file Format
	/// </summary>
	/// <param name="vFilePathName"></param>
	/// <returns></returns>
	bool ExportToDBCFile(const std::string& vFilePathName);

	/// <summary>
	/// Export to C lang File (not very usefull, in our case, its just a possibility covered by dbcppp) 
	/// </summary>
	/// <param name="vFilePathName"></param>
	/// <returns></returns>
	bool ExportToCFile(const std::string& vFilePathName);

	/// <summary>
	/// Export to Human Readable File (seem interresting in some cases, and its a possibility covered by dbcppp) 
	/// </summary>
	/// <param name="vFilePathName"></param>
	/// <returns></returns>
	bool ExportToHumanFile(const std::string& vFilePathName);
};