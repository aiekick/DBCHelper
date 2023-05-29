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

#include "DBCFile.h"

#include <ctools/FileHelper.h>

#include <dbcppp/include/dbcppp/Network2Functions.h>
#include <Modules/Dbc/DBCModel.h>
#include <fstream>
#include <istream>

#include <Modules/Dbc/DBCObjects/DBCNode.h>
#include <Modules/Dbc/DBCObjects/DBCSignal.h>
#include <Modules/Dbc/DBCObjects/DBCMessage.h>
#include <Modules/Dbc/DBCObjects/DBCNetwork.h>

#include <Helper/TranslationHelper.h>

///////////////////////////////////////////////////////////////
//// MACRO'S //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

#define CHECK_DBC_FLAGS(FLAG) (int32_t)DBCModel::s_DBCErrorTypeFlags & (int32_t)DBCErrorTypeFlags::FLAG

///////////////////////////////////////////////////////////////
//// STATIC ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

uintptr_t DBCFile::GetBitsFrom64(uintptr_t vOrig64BitWord, unsigned vStartBit, unsigned vBitCount)
{
	const uintptr_t mask = (uintptr_t)((1 << vBitCount) - 1U) << vStartBit;
	return (uintptr_t)((vOrig64BitWord & mask) >> vStartBit);
}

uintptr_t DBCFile::SetBitsInto64(uintptr_t vOrig64BitWord, unsigned vStartBit, unsigned vBitCount, uintptr_t vBitsToSet)
{
	const uintptr_t mask = (uintptr_t)((1 << vBitCount) - 1U);
	const uintptr_t masked_value = (uintptr_t)(vBitsToSet & mask);
	const uintptr_t offseted_value = (uintptr_t)(masked_value << vStartBit);
	return (uintptr_t)(vOrig64BitWord | offseted_value);
}

void DBCFile::printfBin(const char* vLabel, uintptr_t vNumber)
{
	printf("%s ", vLabel);

	void* ptr = &vNumber;
	int size = (int)sizeof(uintptr_t);
	unsigned char* b = (unsigned char*)ptr;
	unsigned char byte;
	int i, j;

	for (i = size - 1; i >= 0; i--)
	{
		printf(" ");
		for (j = 7; j >= 0; j--)
		{
			byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
	}

	printf("\n");
}

///////////////////////////////////////////////////////////////
//// STATIC : FACTORY /////////////////////////////////////////
///////////////////////////////////////////////////////////////

std::shared_ptr<DBCFile> DBCFile::Create(const std::string& vFilePathName)
{
	auto res = std::make_shared<DBCFile>();
	res->m_This = res;
	if (!res->Init(vFilePathName))
	{
		res.reset();
	}
	return res;
}

///////////////////////////////////////////////////////////////
//// INIT / UNIT //////////////////////////////////////////////
///////////////////////////////////////////////////////////////

bool DBCFile::Init(const std::string& vFilePathName)
{
	if (!vFilePathName.empty())
	{
		auto ps = FileHelper::Instance()->ParsePathFileName(vFilePathName);
		if (ps.isOk)
		{
			this->filePathName = ps.GetFPNE();
			this->fileName = ps.name;

			return true;
		}
	}
	else
	{
		return true;
	}

	// catched when ps.isOk is false
	CTOOL_DEBUG_BREAK;

	return false;
}

void DBCFile::Unit()
{
	Clear();
}

///////////////////////////////////////////////////////////////
//// CLEAR ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

void DBCFile::Clear()
{
	m_DBCNetworks_by_name.clear();
	m_DBCNodes_by_name.clear();
	m_DBCMessages_by_id.clear();
	m_DBCMessages_by_name.clear();
	m_DBCSignals_by_name.clear();

	m_DB_Networks.clear();
	m_DB_Nodes.clear();
	m_DB_Messages.clear();
	m_DB_Signals.clear();

	m_MainNetworkPtr.reset();

	filePathName.clear();
	fileName.clear();
	instance.reset();
}

///////////////////////////////////////////////////////////////
//// LOAD / MERGE DBC's ///////////////////////////////////////
///////////////////////////////////////////////////////////////

bool DBCFile::LoadDBCFile(const std::string& vFilePathName)
{
	std::ifstream idbc(filePathName);
	if (idbc.good())
	{
		instance = dbcppp::INetwork::LoadDBCFromIs(idbc);
		idbc.close();

		if (instance)
		{
			auto ps = FileHelper::Instance()->ParsePathFileName(filePathName);
			if (ps.isOk)
			{
				auto _network = std::make_shared<DBCNetwork>(instance.get());
				_network->name = ps.name;
				_network->parent_dbc = m_This;
				_network->isMain = true;
				m_MainNetworkPtr = _network;
				m_DB_Networks.insert(_network);
				AddNetwork(_network);

				for (const auto& node : instance->Nodes())
				{
					if (node.Name() != "Vector__XXX")
					{
						auto _node = std::make_shared<DBCNode>(&node);
						_node->name = node.Name();
						_node->parent_network = m_MainNetworkPtr;
						m_MainNetworkPtr->nodes[_node->name] = _node;
						m_DB_Nodes.insert(_node);
						AddNode(_node);
					}
				}

				for (const auto& msg : instance->Messages())
				{
					// create message
					std::shared_ptr<DBCMessage> _msgPtr = nullptr; 
					
					if (msg.Name() != "VECTOR__INDEPENDENT_SIG_MSG")
					{
						_msgPtr = std::make_shared<DBCMessage>(&msg);
						_msgPtr->name = msg.Name();
						_msgPtr->id = msg.Id();
						_msgPtr->parent_network = m_MainNetworkPtr;
						_msgPtr->full_size = msg.MessageSize();
						m_DB_Messages.insert(_msgPtr);

						// transmitters
						{
							auto parent_node_Ptr = m_MainNetworkPtr->GetNode(msg.Transmitter()).getValidShared();
							if (parent_node_Ptr)
							{
								// define his parent node and add him as child
								parent_node_Ptr->messages_by_name[_msgPtr->name] = _msgPtr;
								parent_node_Ptr->messages_by_id[_msgPtr->id] = _msgPtr;
								_msgPtr->parent_nodes.emplace(parent_node_Ptr);
							}

							for (auto node : msg.MessageTransmitters())
							{
								auto parent_node_Ptr = m_MainNetworkPtr->GetNode(node).getValidShared();
								if (parent_node_Ptr)
								{
									// define his parent node and add him as child
									parent_node_Ptr->messages_by_name[_msgPtr->name] = _msgPtr;
									parent_node_Ptr->messages_by_id[_msgPtr->id] = _msgPtr;
									_msgPtr->parent_nodes.emplace(parent_node_Ptr);
								}
							}
						}

						AddMessage(_msgPtr);
					}

					// add signals
					for (const dbcppp::ISignal& sig : msg.Signals())
					{
						auto _sig = std::make_shared<DBCSignal>(&sig);
						_sig->name = sig.Name();
						m_DB_Signals.insert(_sig);

						if (_msgPtr)
						{
							_sig->parent_nodes = _msgPtr->parent_nodes;
							_sig->parent_messages.emplace(_msgPtr);		// define his parent message
							_msgPtr->signals[sig.StartBit()] = _sig;		// add him as a message child
						}

						_sig->parent_network = m_MainNetworkPtr;

						AddSignal(_sig);
					}
				}

				return true;
			}
		}
	}

	return false;
}

bool DBCFile::MergeDBCFile(std::shared_ptr<DBCFile> vDBCFilePtr)
{
	if (!vDBCFilePtr)
		return false;

	auto networkPtr = vDBCFilePtr->GetMainNetwork();
	if (!networkPtr)
		return false;

	networkPtr->isMain = false;

	AddNetwork(networkPtr);

	for (auto node : vDBCFilePtr->GetContainerNodesByNames())
	{
		AddNode(node.second.getValidShared());
	}

	for (auto msg : vDBCFilePtr->GetContainerMessagesByIds())
	{
		AddMessage(msg.second.getValidShared());
	}

	for (auto sig : vDBCFilePtr->GetContainerSignalsByNames())
	{
		AddSignal(sig.second.getValidShared());
	}

	/*
	for (auto node : networkPtr->nodes)
	{
		auto nodePtr = node.second.getValidShared();
		if (nodePtr)
		{
			AddNode(nodePtr);

			for (auto msg : nodePtr->messages_by_id)
			{
				auto msgPtr = msg.second.getValidShared();
				if (msgPtr)
				{
					AddMessage(msgPtr);

					for (auto sig : msgPtr->signals)
					{
						auto sigPtr = sig.second.getValidShared();
						if (sigPtr)
						{
							AddSignal(sigPtr);
						}
					}
				}
			}
		}
	}
	*/

	return true;
}

///////////////////////////////////////////////////////////////
//// ADD : NETWORKS / NODES / MESSAGES / SIGNALS //////////////
///////////////////////////////////////////////////////////////

void DBCFile::AddNetwork(std::shared_ptr<DBCNetwork> vNetworkPtr)
{
	if (m_DBCNetworks_by_name.find(vNetworkPtr->name) == m_DBCNetworks_by_name.end())
	{
		m_DBCNetworks_by_name[vNetworkPtr->name] = vNetworkPtr;
	}
	else
	{
		auto ptr = m_DBCNetworks_by_name[vNetworkPtr->name].getValidShared();
		if (ptr && ptr != vNetworkPtr)
		{
			ptr->networks_with_same_name.emplace(vNetworkPtr);
		}
	}
}

void DBCFile::AddNode(std::shared_ptr<DBCNode> vNodePtr)
{
	if (m_DBCNodes_by_name.find(vNodePtr->name) == m_DBCNodes_by_name.end())
	{
		m_DBCNodes_by_name[vNodePtr->name] = vNodePtr;
	}
	else
	{
		auto ptr = m_DBCNodes_by_name[vNodePtr->name].getValidShared();
		if (ptr && ptr != vNodePtr)
		{
			ptr->nodes_with_same_name.emplace(vNodePtr); 
		}
	}
}

void DBCFile::AddMessage(std::shared_ptr<DBCMessage> vMsgPtr)
{
	if (m_DBCMessages_by_id.find(vMsgPtr->id) == m_DBCMessages_by_id.end())
	{
		m_DBCMessages_by_id[vMsgPtr->id] = vMsgPtr;
	}
	else
	{
		auto ptr = m_DBCMessages_by_id[vMsgPtr->id].getValidShared();
		if (ptr && ptr != vMsgPtr)
		{
			ptr->messages_with_same_id.emplace(vMsgPtr);
		}
	}

	if (m_DBCMessages_by_name.find(vMsgPtr->name) == m_DBCMessages_by_name.end())
	{
		m_DBCMessages_by_name[vMsgPtr->name] = vMsgPtr;
	}
	else
	{
		auto ptr = m_DBCMessages_by_name[vMsgPtr->name].getValidShared();
		if (ptr && ptr != vMsgPtr)
		{
			ptr->messages_with_same_name.emplace(vMsgPtr);
		}
	}
}

void DBCFile::AddSignal(std::shared_ptr<DBCSignal> vSignalPtr)
{
	if (m_DBCSignals_by_name.find(vSignalPtr->name) == m_DBCSignals_by_name.end())
	{
		m_DBCSignals_by_name[vSignalPtr->name] = vSignalPtr;
	}
	else
	{
		auto ptr = m_DBCSignals_by_name[vSignalPtr->name].getValidShared();
		if (ptr && ptr != vSignalPtr)
		{
			ptr->signals_with_same_name.emplace(vSignalPtr);
		}
	}
}

///////////////////////////////////////////////////////////////
//// GETTERS : NETWORKS / NODES / MESSAGES / SIGNALS //////////
///////////////////////////////////////////////////////////////

std::shared_ptr<DBCNetwork> DBCFile::GetMainNetwork()
{
	return m_MainNetworkPtr;
}

DBCNetwork_Weak DBCFile::GetNetworksByName(const std::string& vName)
{
	DBCNetwork_Weak res;

	if (m_DBCNetworks_by_name.find(vName) != m_DBCNetworks_by_name.end())
	{
		return m_DBCNetworks_by_name[vName];
	}

	return res;
}

DBCNode_Weak DBCFile::GetNodesByName(const std::string& vName)
{
	DBCNode_Weak res;

	if (m_DBCNodes_by_name.find(vName) != m_DBCNodes_by_name.end())
	{
		return m_DBCNodes_by_name[vName];
	}

	return res;
}

DBCMessage_Weak DBCFile::GetMessagesByID(const uint32_t& vID)
{
	DBCMessage_Weak res;

	if (m_DBCMessages_by_id.find(vID) != m_DBCMessages_by_id.end())
	{
		return m_DBCMessages_by_id[vID];
	}

	return res;
}

DBCMessage_Weak DBCFile::GetMessagesByName(const std::string& vName)
{
	DBCMessage_Weak res;
	
	if (m_DBCMessages_by_name.find(vName) != m_DBCMessages_by_name.end())
	{
		return m_DBCMessages_by_name[vName];
	}

	return res;
}


DBCSignal_Weak DBCFile::GetSignalsByName(const std::string& vName)
{
	DBCSignal_Weak res;

	if (m_DBCSignals_by_name.find(vName) != m_DBCSignals_by_name.end())
	{
		return m_DBCSignals_by_name[vName];
	}

	return res;
}

///////////////////////////////////////////////////////////////
//// GETTERS : CONTAINERS /////////////////////////////////////
///////////////////////////////////////////////////////////////

const DBCNetworksNames& DBCFile::GetContainerNetworksByNames()
{
	return m_DBCNetworks_by_name;
}

const DBCNodesNames& DBCFile::GetContainerNodesByNames()
{
	return m_DBCNodes_by_name;
}

const DBCMessagesIds& DBCFile::GetContainerMessagesByIds()
{
	return m_DBCMessages_by_id;
}

const DBCMessagesNames& DBCFile::GetContainerMessagesByNames()
{
	return m_DBCMessages_by_name;
}

const DBCSignalsNames& DBCFile::GetContainerSignalsByNames()
{
	return m_DBCSignals_by_name;
}

///////////////////////////////////////////////////////////////
//// DBC : ANALYSERS //////////////////////////////////////////
///////////////////////////////////////////////////////////////

/// <summary>
/// will compute the minimal bit size need for a min/max couple and data types
/// </summary>
/// <param name="vMinValue"></param>
/// <param name="vMaxValue"></param>
/// <param name="vValueType"></param>
/// <param name="vExtValueType"></param>
/// <returns>return the bit size</returns>
inline uint32_t GetBitSizeForMinMax(
	const double& vMinValue, 
	const double& vMaxValue, 
	const dbcppp::ISignal::EValueType& vValueType,
	const dbcppp::ISignal::EExtendedValueType& vExtValueType)
{
	std::string res;

	uint32_t bitSize = 0U;

	double range_inf = 0.0;
	double range_sup = 0.0;

	if (vExtValueType == dbcppp::ISignal::EExtendedValueType::Integer)
	{
		bool is_ok = false;
		while (!is_ok)
		{
			bitSize++;

			if (vValueType == dbcppp::ISignal::EValueType::Signed)
			{
				range_inf = -pow(2, bitSize - 1);
				range_sup = pow(2, bitSize - 1) - 1;
			}
			else if (vValueType == dbcppp::ISignal::EValueType::Unsigned)
			{
				if (vMinValue < 0.0)
				{
					CTOOL_DEBUG_BREAK;
					return 0U;
				}

				range_inf = 0;
				range_sup = pow(2, bitSize) - 1;
			}

			//is_ok = (vMinValue >= range_inf && vMaxValue <= range_sup);
			// the next code is faster in some cases
			// statistically i guess we have more chance to have min value to zero 
			// so better to start the check with the max_value
			is_ok = !(vMaxValue > range_sup || vMinValue < range_inf);
		}
	}
	else if (vExtValueType == dbcppp::ISignal::EExtendedValueType::Float)
	{
		bitSize = 32U;
	}
	else if (vExtValueType == dbcppp::ISignal::EExtendedValueType::Double)
	{
		bitSize = 64U;
	}
	
	return bitSize;
}

void DBCFile::CheckForErrors()
{
	/*
	les erreurs a checker sont :
	*/

	/*
	DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE								=> OK
	DBC_ERROR_TYPE_LOCAL_MSG_SIZE_OUT_OF_RANGE								=> OK
	DBC_ERROR_TYPE_LOCAL_SIG_END_OUT_OF_MSG_SIZE_RANGE						=> OK
	DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT									=> OK
	DBC_ERROR_TYPE_LOCAL_SIG_IS_NOT_MAPPED_TO_A_MSG							=> TODO
	DBC_ERROR_TYPE_MERGE_NOD_HAVE_SAME_NAME_AS_OTHER_NOD					=> PARTIAL only for count here , but erros msg update need reload
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_AS_OTHER_MSG					=> PARTIAL only for count here , but erros msg update need reload
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_AS_OTHER_MSG						=> PARTIAL only for count here , but erros msg update need reload
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_BUT_NOT_SAME_NAME_AS_OTHER_MSG	=> OK
	DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_BUT_NOT_SAME_ID_AS_OTHER_MSG	=> OK
	DBC_ERROR_TYPE_MERGE_SIG_HAVE_SAME_NAME_AS_OTHER_SIG					=> PARTIAL only for count here , but erros msg update need reload
	*/

	m_Network_Errors_Count = 0U;
	m_Node_Errors_Count = 0U;
	m_Message_Errors_Count = 0U;
	m_Signal_Errors_Count = 0U;
	m_ErrorsCountByTypes.clear();

	if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_BUT_NOT_SAME_ID_AS_OTHER_MSG))
	{
		// message : same id but different name
		// for each message id, we will search a message with the associated name
		// and check if this new message id is different
		for (const auto& msgIter : m_DBCMessages_by_id)
		{
			auto base_msgPtr = msgIter.second.getValidShared();
			if (!base_msgPtr)
				continue;

			auto new_msgPtr = GetMessagesByName(base_msgPtr->name).getValidShared();
			if (new_msgPtr && new_msgPtr->id != base_msgPtr->id)
			{
				// error
				base_msgPtr->messages_with_same_name_and_different_id.emplace(new_msgPtr);
				new_msgPtr->messages_with_same_name_and_different_id.emplace(base_msgPtr);
			}
		}
	}

	if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_BUT_NOT_SAME_NAME_AS_OTHER_MSG))
	{
		// message : same name but different id
		// for each message name, we will search a message with the associated id
		// and check if this new message name is different
		for (const auto& msgIter : m_DBCMessages_by_name)
		{
			auto base_msgPtr = msgIter.second.getValidShared();
			if (!base_msgPtr)
				continue;

			auto new_msgPtr = GetMessagesByID((uint32_t)base_msgPtr->id).getValidShared();
			if (new_msgPtr && new_msgPtr->name != base_msgPtr->name)
			{
				// error
				base_msgPtr->messages_with_same_id_and_different_name.emplace(new_msgPtr);
				new_msgPtr->messages_with_same_id_and_different_name.emplace(base_msgPtr);
			}
		}
	}

	for (const auto& nodeIter : m_DBCNodes_by_name)
	{
		auto nodePtr = nodeIter.second.getValidShared();
		if (!nodePtr)
			continue;

		nodePtr->ClearErrors();

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_MERGE_NOD_HAVE_SAME_NAME_AS_OTHER_NOD))
		{
			if (!nodePtr->nodes_with_same_name.empty())
			{
				const char* err_msg = "Error : The Node %s from the Network %s, have the same name %s";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : Le noeud %s du Reseau %s, possede le meme nom %s";

				for (auto node_same_name : nodePtr->nodes_with_same_name)
				{
					auto node_same_name_Ptr = node_same_name.getValidShared();
					if (node_same_name_Ptr)
					{
						auto parentNetworkPtr = node_same_name_Ptr->parent_network.getValidShared();
						const char* parent_network_name = "Unknown";
						if (parentNetworkPtr)
							parent_network_name = parentNetworkPtr->name.c_str();

						nodePtr->merge_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_NOD_HAVE_SAME_NAME_AS_OTHER_NOD,
							ct::toStr(err_msg,
								node_same_name_Ptr->name.c_str(), 
								parent_network_name,
								nodePtr->name.c_str()));
					}
				}

				m_Network_Errors_Count++;
				m_Node_Errors_Count++;
				//m_Message_Errors_Count++;
				//m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_NOD_HAVE_SAME_NAME_AS_OTHER_NOD]++;
			}
		}

		nodePtr->local_count_errors = nodePtr->local_errors.GetCountErrors();
		nodePtr->merge_count_errors = nodePtr->merge_errors.GetCountErrors();
	}

	// we must choose one db, m_DBCMessages_by_id or m_DBCMessages_by_name is ok for a functionnal point of view
	// but for a perf point of view, m_DBCMessages_by_id will be faster since the key in just a uint64_t and not a string
	for (const auto& msgIter : m_DBCMessages_by_id)
	{
		auto msgPtr = msgIter.second.getValidShared();
		if (!msgPtr)
			continue;

		msgPtr->ClearErrors();

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_MSG_IS_NOT_MAPPED_TO_A_NODE))
		{
			bool _is_any_parents = false;
			for (auto pa : msgPtr->parent_nodes)
			{
				if (!pa.expired())
				{
					_is_any_parents = true;
					break;
				}
			}
			if (!_is_any_parents && !msgPtr->parent_nodes.empty())
				CTOOL_DEBUG_BREAK;

			if (msgPtr->parent_nodes.empty())
			{
				const char* err_msg = "Error : The Message is not mapped to a node";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : Le message n'est associe a aucun noeud";

				msgPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_IS_NOT_MAPPED_TO_A_NODE,
					err_msg);
				m_Network_Errors_Count++;
				//m_Node_Errors_Count++;
				m_Message_Errors_Count++;
				//m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_IS_NOT_MAPPED_TO_A_NODE]++;
			}
		}

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_AS_OTHER_MSG))
		{
			if (!msgPtr->messages_with_same_id.empty())
			{
				const char* err_msg = "Error : The Message %s from the Network %s, have the same id 0x%X";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : Le message %s du reseau %s, possede le meme id 0x%X";

				for (auto msg_same_id : msgPtr->messages_with_same_id)
				{
					auto msg_same_id_Ptr = msg_same_id.getValidShared();
					if (msg_same_id_Ptr)
					{
						auto parentNetworkPtr = msg_same_id_Ptr->parent_network.getValidShared();
						const char* parent_network_name = "Unknown";
						if (parentNetworkPtr)
							parent_network_name = parentNetworkPtr->name.c_str();

						msgPtr->merge_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_AS_OTHER_MSG,
							ct::toStr(err_msg,
								msg_same_id_Ptr->name.c_str(), 
								parent_network_name,
								(uint32_t)msgPtr->id));
					}
				}

				m_Network_Errors_Count++;
				//m_Node_Errors_Count++;
				m_Message_Errors_Count++;
				//m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_AS_OTHER_MSG]++;
			}
		}

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_AS_OTHER_MSG))
		{
			if (!msgPtr->messages_with_same_name.empty())
			{
				const char* err_msg = "Error : The Message %s from the Network %s, have the same name %s";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : Le message %s du reseau %s, possede le meme nom %s";

				for (auto msg_same_name : msgPtr->messages_with_same_name)
				{
					auto msg_same_name_Ptr = msg_same_name.getValidShared();
					if (msg_same_name_Ptr)
					{
						auto parentNetworkPtr = msg_same_name_Ptr->parent_network.getValidShared();
						const char* parent_network_name = "Unknown";
						if (parentNetworkPtr)
							parent_network_name = parentNetworkPtr->name.c_str();

						msgPtr->merge_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_AS_OTHER_MSG,
							ct::toStr(err_msg,
								msg_same_name_Ptr->name.c_str(), 
								parent_network_name, 
								msgPtr->name.c_str()));
					}
				}

				m_Network_Errors_Count++;
				//m_Node_Errors_Count++;
				m_Message_Errors_Count++;
				//m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_AS_OTHER_MSG]++;
			}
		}

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_BUT_NOT_SAME_ID_AS_OTHER_MSG))
		{
			if (!msgPtr->messages_with_same_name_and_different_id.empty())
			{
				const char* err_msg = "Error : The Message %s from the Network %s, have the same name %s but a different id 0x%X";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : Le message %s du reseau %s, possede le meme nom %s mais un id different 0x%X";

				for (auto msg_with_with_same_name_and_different_id : msgPtr->messages_with_same_name_and_different_id)
				{
					auto msg_with_with_same_name_and_different_id_Ptr = msg_with_with_same_name_and_different_id.getValidShared();
					if (msg_with_with_same_name_and_different_id_Ptr)
					{
						auto parentNetworkPtr = msg_with_with_same_name_and_different_id_Ptr->parent_network.getValidShared();
						const char* parent_network_name = "Unknown";
						if (parentNetworkPtr)
							parent_network_name = parentNetworkPtr->name.c_str();

						msgPtr->merge_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_BUT_NOT_SAME_ID_AS_OTHER_MSG,
							ct::toStr(err_msg,
								msg_with_with_same_name_and_different_id_Ptr->name.c_str(),
								parent_network_name,
								msgPtr->name.c_str(),
								(uint32_t)msgPtr->id));
					}
				}

				m_Network_Errors_Count++;
				//m_Node_Errors_Count++;
				m_Message_Errors_Count++;
				//m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_NAME_BUT_NOT_SAME_ID_AS_OTHER_MSG]++;
			}
		}

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_BUT_NOT_SAME_NAME_AS_OTHER_MSG))
		{
			if (!msgPtr->messages_with_same_id_and_different_name.empty())
			{
				const char* err_msg = "Error : The Message %s from the Network %s, have the same id 0x%X but a different name %s";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : Le message %s du reseau %s, possede le meme id 0x%X mais avec un nom different %s";

				for (auto msg_with_same_id_and_different_name : msgPtr->messages_with_same_id_and_different_name)
				{
					auto msg_with_same_id_and_different_name_Ptr = msg_with_same_id_and_different_name.getValidShared();
					if (msg_with_same_id_and_different_name_Ptr)
					{
						auto parentNetworkPtr = msg_with_same_id_and_different_name_Ptr->parent_network.getValidShared();
						const char* parent_network_name = "Unknown";
						if (parentNetworkPtr)
							parent_network_name = parentNetworkPtr->name.c_str();

						msgPtr->merge_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_BUT_NOT_SAME_NAME_AS_OTHER_MSG,
							ct::toStr(err_msg,
								msg_with_same_id_and_different_name_Ptr->name.c_str(),
								parent_network_name,
								(uint32_t)msg_with_same_id_and_different_name_Ptr->id,
								msgPtr->name.c_str()));
					}
				}

				m_Network_Errors_Count++;
				//m_Node_Errors_Count++;
				m_Message_Errors_Count++;
				//m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_MSG_HAVE_SAME_ID_BUT_NOT_SAME_NAME_AS_OTHER_MSG]++;
			}
		}

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_STANDARD))
		{
			if (msgPtr->id < 0x000 || msgPtr->id > 0x7FF)
			{
				const char* err_msg = "Error : The message ID (Can 2.0A - 11 bits) is out of range [0x000:0x7FF]";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : L'ID du message (Can 2.0A - 11 bits) est en dehors de la plage autorisee [0x000:0x7FF]";

				msgPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_STANDARD,
					err_msg);
				m_Network_Errors_Count++;
				//m_Node_Errors_Count++;
				m_Message_Errors_Count++;
				//m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_STANDARD]++;
			}
		}

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_EXTENDED))
		{
			if (msgPtr->id < 0x00000000 || msgPtr->id > 0x1FFFFFFF)
			{
				const char* err_msg = "Error : The message ID (Can 2.0B - 29 bits) is out of range [0x0000 0000:0x1FFF FFFF]";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : L'ID du message (Can 2.0B - 29 bits) est en dehors de la plage autorisee [0x0000 0000:0x1FFF FFFF]";

				msgPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_EXTENDED,
					err_msg);
				m_Network_Errors_Count++;
				//m_Node_Errors_Count++;
				m_Message_Errors_Count++;
				//m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_ID_OUT_OF_RANGE_CAN_EXTENDED]++;
			}
		}

		msgPtr->local_count_errors = msgPtr->local_errors.GetCountErrors();
		msgPtr->merge_count_errors = msgPtr->merge_errors.GetCountErrors();
	}

	for (auto& sigIter : m_DBCSignals_by_name)
	{
		auto sigPtr = sigIter.second.getValidShared();
		if (!sigPtr)
			continue;

		sigPtr->ClearErrors();

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_SIG_IS_NOT_MAPPED_TO_A_MSG))
		{
			bool _is_any_parents = false;
			for (auto pa : sigPtr->parent_messages)
			{
				if (!pa.expired())
				{
					_is_any_parents = true;
					break;
				}
			}
			if (!_is_any_parents && !sigPtr->parent_messages.empty())
				CTOOL_DEBUG_BREAK;

			if (sigPtr->parent_messages.empty())
			{
				const char* err_msg = "Error : The Signal is not mapped to a Message";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : Le signal n'est associe a aucun message";

				sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_IS_NOT_MAPPED_TO_A_MSG,
					err_msg);
				m_Network_Errors_Count++;
				//m_Node_Errors_Count++;
				//m_Message_Errors_Count++;
				m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_IS_NOT_MAPPED_TO_A_MSG]++;
			}
		}

		if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_MERGE_SIG_HAVE_SAME_NAME_AS_OTHER_SIG))
		{
			if (!sigPtr->signals_with_same_name.empty())
			{
				const char* err_msg = "Error : The Signal %s from the Network %s, have the same name %s";
				if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
					err_msg = "Erreur : Le signal %s du reseau %s, possede le meme nom %s";

				for (auto signal_same_name : sigPtr->signals_with_same_name)
				{
					auto signal_same_name_Ptr = signal_same_name.getValidShared();
					if (signal_same_name_Ptr)
					{
						auto parentNetworkPtr = signal_same_name_Ptr->parent_network.getValidShared();
						const char* parent_network_name = "Unknown";
						if (parentNetworkPtr)
							parent_network_name = parentNetworkPtr->name.c_str();

						sigPtr->merge_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_SIG_HAVE_SAME_NAME_AS_OTHER_SIG,
							ct::toStr(err_msg,
								signal_same_name_Ptr->name.c_str(), 
								parent_network_name,
								sigPtr->name.c_str()));
					}
				}

				m_Network_Errors_Count++;
				//m_Node_Errors_Count++;
				//m_Message_Errors_Count++;
				m_Signal_Errors_Count++;
				m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_MERGE_SIG_HAVE_SAME_NAME_AS_OTHER_SIG]++;
			}
		}

		const dbcppp::ISignal* sig = sigPtr->m_Signal;
		if (sig)
		{
			const auto& vt = sig->ValueType();
			const auto& evt = sig->ExtendedValueType();
			const auto& offset = sig->Offset();
			const auto& factor = sig->Factor();
			const auto& size = sig->BitSize();
			const auto& start = sig->StartBit();
			auto raw_min_value = (sig->Minimum() - offset) / factor;
			auto raw_max_value = (sig->Maximum() - offset) / factor;

			if ((uint32_t)(start + size) > 64U)
			{
				if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_MSG_SIZE_OUT_OF_RANGE))
				{
					for (auto msg : sigPtr->parent_messages)
					{
						auto parent_message_ptr = msg.getValidShared();
						if (parent_message_ptr)
						{
							const char* err_msg = "Error : The message is bigger than 64 bits / 8 bytes";
							if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
								err_msg = "Erreur : Le message est plus grand que 64 bits / 8 Octets";

							parent_message_ptr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_SIZE_OUT_OF_RANGE,
								err_msg);
							m_Network_Errors_Count++;
							//m_Node_Errors_Count++;
							m_Message_Errors_Count++;
							//m_Signal_Errors_Count++;
							m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_MSG_SIZE_OUT_OF_RANGE]++;
						}
					}
				}

				if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_SIG_END_OUT_OF_MSG_SIZE_RANGE))
				{
					const char* err_msg = "Error : the bit end (bit start + bit count) is out of 64 bits";
					if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
						err_msg = "Erreur : le bit du fin (bit de demarrage + nombre de bits) est au dela de 64 bits";

					sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_END_OUT_OF_MSG_SIZE_RANGE,
						err_msg);
					m_Network_Errors_Count++;
					//m_Node_Errors_Count++;
					//m_Message_Errors_Count++;
					m_Signal_Errors_Count++;
					m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_END_OUT_OF_MSG_SIZE_RANGE]++;
				}
			}

			if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX))
			{
				if (size)
				{
					if (raw_min_value > raw_max_value)
					{
						const char* err_msg = "Error : the min value must be inferior to the max value";
						if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
							err_msg = "Erreur : La valeur mini doit etre inferieur a la valeur maxi";

						sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX,
							err_msg);
						m_Network_Errors_Count++;
						//m_Node_Errors_Count++;
						//m_Message_Errors_Count++;
						m_Signal_Errors_Count++;
						m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX]++;
					}

					if (IS_DOUBLE_EQUAL(raw_min_value, raw_max_value))
					{
						const char* err_msg = "Error : the min value must be different than the max value";
						if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
							err_msg = "Erreur : La valeur minimale doit etre differente de la valeur maximale";

						sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX,
							err_msg);
						m_Network_Errors_Count++;
						//m_Node_Errors_Count++;
						//m_Message_Errors_Count++;
						m_Signal_Errors_Count++;
						m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX]++;
					}
				}
			}

			switch (evt)
			{
				case dbcppp::ISignal::EExtendedValueType::Integer:
				{
					raw_min_value = floor(raw_min_value);
					raw_max_value = floor(raw_max_value);

					switch (vt)
					{
						case dbcppp::ISignal::EValueType::Signed:
						{
							const double& range_inf = -pow(2, size - 1);
							const double& range_sup = pow(2, size - 1) - 1;
							if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT))
							{
								if (size == 1)
								{
									const char* err_msg = "Error : A bit size of 1 must be Unsigned";
									if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
										err_msg = "Erreur : Le nombre de bit ne peut pas etre de 1 pour un entier UnSigned";

									sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT,
										err_msg);
									m_Network_Errors_Count++;
									//m_Node_Errors_Count++;
									//m_Message_Errors_Count++;
									m_Signal_Errors_Count++;
									m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT]++;
								}

								if (raw_min_value < range_inf || raw_max_value > range_sup)
								{
									const auto& minimal_bit_size = GetBitSizeForMinMax(raw_min_value, raw_max_value, vt, evt);
									if (minimal_bit_size != 0U)
									{
										const char* err_msg = "Error : with this min/max and this type, the minimal bit size must be %u";
										if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
											err_msg = "Erreur : Avec ces valeurs minimales/maximale et ce type, le nombre de bit minimal doit etre de %u";

										sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT, ct::toStr(
											err_msg, minimal_bit_size));
										m_Network_Errors_Count++;
										//m_Node_Errors_Count++;
										//m_Message_Errors_Count++;
										m_Signal_Errors_Count++;
										m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT]++;
									}
								}
							}
							break;
						}
						case dbcppp::ISignal::EValueType::Unsigned:
						{
							const double& range_inf = 0;
							const double& range_sup = pow(2, size) - 1;

							if (raw_min_value < 0.0 || raw_max_value < 0.0)
							{
								if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX))
								{
									const char* err_msg = "Error : a negative value must be Signed";
									if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
										err_msg = "Erreur : Une valeur négative doit etre de type Signed";

									sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX, ct::toStr(
										err_msg));
									m_Network_Errors_Count++;
									//m_Node_Errors_Count++;
									//m_Message_Errors_Count++;
									m_Signal_Errors_Count++;
									m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_MIN_MAX]++;
								}
							}
							else if (raw_min_value < range_inf || raw_max_value > range_sup)
							{
								const auto& minimal_bit_size = GetBitSizeForMinMax(raw_min_value, raw_max_value, vt, evt);
								if (minimal_bit_size != 0U)
								{
									if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT))
									{
										const char* err_msg = "Error : with this min/max and this type, the minimal bit size must be %u";
										if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
											err_msg = "Erreur : Avec ces valeurs minimales/maximale et ce type, le nombre de bits doit etre de %u au minimum";

										sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT, ct::toStr(
											err_msg, minimal_bit_size));
										m_Network_Errors_Count++;
										//m_Node_Errors_Count++;
										//m_Message_Errors_Count++;
										m_Signal_Errors_Count++;
										m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT]++;
									}
								}
							}

							break;
						}
					}
					break;
				}
				case dbcppp::ISignal::EExtendedValueType::Double:
				{
					if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT))
					{
						if (size < 64)
						{
							const char* err_msg = "Error : A double cannot be less than 64 bits";
							if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
								err_msg = "Erreur : Un type double ne peut pas avoir un nombre de bits different de 64 bits";

							sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT,
								err_msg);
							m_Network_Errors_Count++;
							//m_Node_Errors_Count++;
							//m_Message_Errors_Count++;
							m_Signal_Errors_Count++;
							m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT]++;
						}
					}
					break;
				}
				case dbcppp::ISignal::EExtendedValueType::Float:
				{
					if (CHECK_DBC_FLAGS(DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT))
					{
						if (size < 32)
						{
							const char* err_msg = "Error : A float cannot be less than 32 bits";
							if (TranslationHelper::s_HelpLanguage == LanguageEnum::FR)
								err_msg = "Erreur : Un type float ne peut pas avoir un nombre de bits different de 32 bits";

							sigPtr->local_errors.AddError(DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT,
								err_msg);
							m_Network_Errors_Count++;
							//m_Node_Errors_Count++;
							//m_Message_Errors_Count++;
							m_Signal_Errors_Count++;
							m_ErrorsCountByTypes[DBCErrorTypeFlags::DBC_ERROR_TYPE_LOCAL_SIG_BAD_BIT_COUNT]++;
						}
					}
					break;
				}
			}
		}

		sigPtr->local_count_errors = sigPtr->local_errors.GetCountErrors();
		sigPtr->merge_count_errors = sigPtr->merge_errors.GetCountErrors();
	}
}

const uint32_t& DBCFile::GetNetworkErrorsCount() const
{
	return m_Network_Errors_Count;
}

const uint32_t& DBCFile::GetNodeErrorsCount() const
{
	return m_Node_Errors_Count;
}

const uint32_t& DBCFile::GetMessageErrorsCount() const
{
	return m_Message_Errors_Count;
}

const uint32_t& DBCFile::GetSignalErrorsCount() const
{
	return m_Signal_Errors_Count;
}

uint32_t DBCFile::GetErrorCountForType(const DBCErrorTypeFlags& vErrorType) const
{
	if (m_ErrorsCountByTypes.find(vErrorType) != m_ErrorsCountByTypes.end())
	{
		return m_ErrorsCountByTypes.at(vErrorType);
	}

	return 0U;
}