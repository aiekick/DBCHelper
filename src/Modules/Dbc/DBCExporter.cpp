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

#include <Modules/Dbc/DBCExporter.h>
#include <Modules/Dbc/DBCFile.h>
#include <ctools/FileHelper.h>
#include <dbcppp/include/dbcppp/Network2Functions.h>
#include <dbcppp/src/libdbcppp/NetworkImpl.h>
#include <dbcppp/src/libdbcppp/NodeImpl.h>
#include <fstream>

////////////////////////////////////////////////////
//// CLEAR/ ////////////////////////////////////////
////////////////////////////////////////////////////

void DBCExporter::Clear()
{
	m_Version.clear();
	m_NewSymbols.clear();
	m_BitTiming.reset();
	m_Nodes.clear();
	m_ValueTables.clear();
	m_Messages.clear();
	m_EnvironmentVariables.clear();
	m_AttributeDefinitions.clear();
	m_AttributeDefaults.clear();
	m_AttributeValues.clear();
	m_Comment.clear();
}

////////////////////////////////////////////////////
//// INIT //////////////////////////////////////////
////////////////////////////////////////////////////

bool DBCExporter::Load(std::shared_ptr<DBCFile> vDBCFilePtr)
{
	// the export issue is on :
	// common names for nodes        => OK    : covered by this soft
	// common names for messages     => OK    : covered by this soft
	// commons attibute definitions  => TO DO : not covnered for the moment

	if (!vDBCFilePtr)
		return false;

	if (!vDBCFilePtr->instance)
		return false;

	std::ifstream idbc(vDBCFilePtr->filePathName);
	if (idbc.good())
	{
		m_TmpNetworkPtr = dbcppp::INetwork::LoadDBCFromIs(idbc);
		idbc.close();
	}
	
	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	/*
	for (auto& new_symbol : vDBCFilePtr->instance->NewSymbols())
	{
		//m_NewSymbols.push_back(new_symbol);
	}
	for (auto node : vDBCFilePtr->instance->Nodes())
	{
		
	}
	for (auto& vt : o.valueTables())
	{
		self.valueTables().push_back(std::move(vt));
	}
	for (auto& m : o.messages())
	{
		self.messages().push_back(std::move(m));
	}
	for (auto& ev : o.environmentVariables())
	{
		self.environmentVariables().push_back(std::move(ev));
	}
	for (auto& ad : o.attributeDefinitions())
	{
		self.attributeDefinitions().push_back(std::move(ad));
	}
	for (auto& ad : o.attributeDefaults())
	{
		self.attributeDefaults().push_back(std::move(ad));
	}
	for (auto& av : o.attributeValues())
	{
		self.attributeValues().push_back(std::move(av));
	}
	*/

	///////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////

	return true;
}

////////////////////////////////////////////////////
//// MERGE /////////////////////////////////////////
////////////////////////////////////////////////////

bool DBCExporter::Merge(std::shared_ptr<DBCFile> vDBCFilePtr)
{
	if (!vDBCFilePtr)
		return false;

	if (!vDBCFilePtr->instance)
		return false;

	if (!m_TmpNetworkPtr)
		return false;

	std::ifstream idbc(vDBCFilePtr->filePathName);
	if (idbc.good())
	{
		m_TmpNetworkPtr->Merge(dbcppp::INetwork::LoadDBCFromIs(idbc));
		idbc.close();

		return true;
	}

	return false;
}

////////////////////////////////////////////////////
//// EXPORT ////////////////////////////////////////
////////////////////////////////////////////////////

bool DBCExporter::ExportToFile(const std::string& vFilePathName, const DBCFile_ExportTypeEnum& vDBCFile_ExportTypeEnum)
{
	bool res = false;

	if (vFilePathName.empty())
		return res;

	switch (vDBCFile_ExportTypeEnum)
	{
	case DBCFile_ExportTypeEnum::EXPORT_TYPE_DBC:
		res = ExportToDBCFile(vFilePathName);
		break;
	case DBCFile_ExportTypeEnum::EXPORT_TYPE_HUMAN:
		res = ExportToHumanFile(vFilePathName);
		break;
	case DBCFile_ExportTypeEnum::EXPORT_TYPE_C:
		res = ExportToCFile(vFilePathName);
		break;
	default:
		break;
	}

	return res;
}

////////////////////////////////////////////////////
//// EXPORT : PRIVATE //////////////////////////////
////////////////////////////////////////////////////

bool DBCExporter::ExportToDBCFile(const std::string& vFilePathName)
{
	bool res = false;

	auto ps = FileHelper::Instance()->ParsePathFileName(vFilePathName);
	if (ps.isOk)
	{
		auto fpn = ps.GetFPNE_WithExt("dbc");

		/*
		auto instance = dbcppp::INetwork::Create(
			std::move(m_Version),
			std::move(m_NewSymbols),
			std::move(m_BitTiming),
			std::move(m_Nodes),
			std::move(m_ValueTables),
			std::move(m_Messages),
			std::move(m_EnvironmentVariables),
			std::move(m_AttributeDefinitions),
			std::move(m_AttributeDefaults),
			std::move(m_AttributeValues),
			std::move(m_Comment));
		*/

		if (m_TmpNetworkPtr)
		{
			using namespace dbcppp::Network2DBC;

			std::ofstream fileWriter(fpn, std::ios::out);
			if (!fileWriter.bad())
			{
				fileWriter << *m_TmpNetworkPtr;
				fileWriter.close();

				res = true;

				FileHelper::Instance()->OpenFile(fpn);
			}

			m_TmpNetworkPtr.reset();

			// clear due to the moves
			Clear();
		}
	}

	return res;
}

bool DBCExporter::ExportToCFile(const std::string& vFilePathName)
{
	bool res = false;

	auto ps = FileHelper::Instance()->ParsePathFileName(vFilePathName);
	if (ps.isOk)
	{
		auto fpn = ps.GetFPNE_WithExt("c");

		/*
		auto instance = dbcppp::INetwork::Create(
			std::move(m_Version),
			std::move(m_NewSymbols),
			std::move(m_BitTiming),
			std::move(m_Nodes),
			std::move(m_ValueTables),
			std::move(m_Messages),
			std::move(m_EnvironmentVariables),
			std::move(m_AttributeDefinitions),
			std::move(m_AttributeDefaults),
			std::move(m_AttributeValues),
			std::move(m_Comment));
		*/

		if (m_TmpNetworkPtr)
		{
			using namespace dbcppp::Network2C;

			std::ofstream fileWriter(fpn, std::ios::out);
			if (!fileWriter.bad())
			{
				fileWriter << *m_TmpNetworkPtr;
				fileWriter.close();

				res = true;

				FileHelper::Instance()->OpenFile(fpn);
			}

			m_TmpNetworkPtr.reset();

			// clear due to the moves
			Clear();
		}
	}

	return res;
}

bool DBCExporter::ExportToHumanFile(const std::string& vFilePathName)
{
	bool res = false;

	auto ps = FileHelper::Instance()->ParsePathFileName(vFilePathName);
	if (ps.isOk)
	{
		auto fpn = ps.GetFPNE_WithExt("txt");

		/*
		auto instance = dbcppp::INetwork::Create(
			std::move(m_Version),
			std::move(m_NewSymbols),
			std::move(m_BitTiming),
			std::move(m_Nodes),
			std::move(m_ValueTables),
			std::move(m_Messages),
			std::move(m_EnvironmentVariables),
			std::move(m_AttributeDefinitions),
			std::move(m_AttributeDefaults),
			std::move(m_AttributeValues),
			std::move(m_Comment));
		*/

		if (m_TmpNetworkPtr)
		{
			using namespace dbcppp::Network2Human;

			std::ofstream fileWriter(fpn, std::ios::out);
			if (!fileWriter.bad())
			{
				fileWriter << *m_TmpNetworkPtr;
				fileWriter.close();

				res = true;

				FileHelper::Instance()->OpenFile(fpn);
			}

			m_TmpNetworkPtr.reset();

			// clear due to the moves
			Clear();
		}
	}

	return res;
}
