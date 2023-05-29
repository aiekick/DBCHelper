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

#include <vector>
#include <map>
#include <string>
#include <memory> // std::unique_ptr, etc..
#include <cstdint> // uint8_t, etc..
#include <thread>
#include <atomic>
#include <mutex>
#include <ctools/cTools.h>
#include <Headers/Globals.h>
#include <dbcppp/include/dbcppp/CApi.h>
#include <dbcppp/include/dbcppp/Network.h>

class DBCNode;
class DBCSignal;
class DBCMessage;
class DBCNetwork;
class DBCFile
{
public: // Static
	static uintptr_t GetBitsFrom64(uintptr_t vOrig64BitWord, unsigned vStartBit, unsigned vBitCount);
	static uintptr_t SetBitsInto64(uintptr_t vOrig64BitWord, unsigned vStartBit, unsigned vBitCount, uintptr_t vValueToInsert);
	static void printfBin(const char* vLabel, uintptr_t vNumber);
	static std::shared_ptr<DBCFile> Create(const std::string& vFilePathName);

private:
	DBCFile_Weak m_This;
	std::shared_ptr<DBCNetwork> m_MainNetworkPtr = nullptr;
	
	DBCNetworksNames m_DBCNetworks_by_name;
	DBCNodesNames m_DBCNodes_by_name;
	DBCMessagesIds m_DBCMessages_by_id;
	DBCMessagesNames m_DBCMessages_by_name;
	DBCSignalsNames m_DBCSignals_by_name;
	
	// database containers
	std::set<std::shared_ptr<DBCNetwork>> m_DB_Networks;
	std::set<std::shared_ptr<DBCNode>> m_DB_Nodes;
	std::set<std::shared_ptr<DBCMessage>> m_DB_Messages;
	std::set<std::shared_ptr<DBCSignal>> m_DB_Signals;

	uint32_t m_Network_Errors_Count = 0U;
	uint32_t m_Node_Errors_Count = 0U;
	uint32_t m_Message_Errors_Count = 0U;
	uint32_t m_Signal_Errors_Count = 0U;

	std::map<DBCErrorTypeFlags, uint32_t> m_ErrorsCountByTypes;

public:
	std::string fileName;
	std::string filePathName;
	std::unique_ptr<dbcppp::INetwork> instance;

public:
	bool Init(const std::string& vFilePathName);
	void Unit();
	void Clear();

	bool LoadDBCFile(const std::string& vFilePathName);
	bool MergeDBCFile(std::shared_ptr<DBCFile> vDBCFilePtr);

	std::shared_ptr<DBCNetwork> GetMainNetwork();
	DBCNetwork_Weak GetNetworksByName(const std::string& vName);
	DBCNode_Weak GetNodesByName(const std::string& vName);
	DBCMessage_Weak GetMessagesByID(const uint32_t& vID);
	DBCMessage_Weak GetMessagesByName(const std::string& vName);
	DBCSignal_Weak GetSignalsByName(const std::string& vName);
	
	const DBCNetworksNames& GetContainerNetworksByNames();
	const DBCNodesNames& GetContainerNodesByNames();
	const DBCMessagesNames& GetContainerMessagesByNames();
	const DBCMessagesIds& GetContainerMessagesByIds();
	const DBCSignalsNames& GetContainerSignalsByNames();

	void AddNetwork(std::shared_ptr<DBCNetwork> vNetwork);
	void AddNode(std::shared_ptr<DBCNode> vNode);
	void AddMessage(std::shared_ptr<DBCMessage> vMsg);
	void AddSignal(std::shared_ptr<DBCSignal> vSignal);

	void CheckForErrors();

	const uint32_t& GetNetworkErrorsCount() const;
	const uint32_t& GetNodeErrorsCount() const;
	const uint32_t& GetMessageErrorsCount() const;
	const uint32_t& GetSignalErrorsCount() const;

	uint32_t GetErrorCountForType(const DBCErrorTypeFlags& vErrorType) const;
};
