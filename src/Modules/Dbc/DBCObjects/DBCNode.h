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
#include <map>
#include <vector>
#include <string>
#include <cstdint> // uint32_t, etc..

#include "DBCObject.h"
#include <dbcppp/include/dbcppp/CApi.h>
#include <dbcppp/include/dbcppp/Network.h>

class DBCNode : public DBCObject
{
public:
	const dbcppp::INode* m_Node = nullptr;
	std::map<std::string, DBCMessage_Weak> messages_by_name;
	std::map<uint64_t, DBCMessage_Weak> messages_by_id;
	DBCNetwork_Weak parent_network;

	// errors
	DBCNodeSet nodes_with_same_name;

public:
	DBCNode(const dbcppp::INode* vNode);
};