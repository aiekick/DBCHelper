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

#include "DBCNetwork.h"

DBCNetwork::DBCNetwork(const dbcppp::INetwork* vNetwork) : m_Network(vNetwork)
{
	type = DBCObjectTypeEnum::NETWORK;


}

ct::cWeak<DBCNode> DBCNetwork::GetNode(const std::string& vName)
{
	if (vName != "Vector__XXX")
	{
		if (nodes.find(vName) != nodes.end())
		{
			return nodes[vName];
		}
	}

	return ct::cWeak<DBCNode>();
}
