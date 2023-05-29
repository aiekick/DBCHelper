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

#include <Headers/Globals.h>

class DBCObject;
class DBCFile;
class DBCModel;
class DBCNetwork;
class DBCNode;
class DBCMessage;
class DBCSignal;
class DBCView
{
private:
	ct::cWeak<DBCModel> m_DBCModel;
	bool m_Show_Tree_ByName_Or_ById = true;
	bool m_Show_View_ByName_Or_ById = true;

	void* m_SelectedItemPtr = nullptr; // selected when 
	ct::cWeak<DBCObject> m_SelectedDBCObject;

public:
	DBCView();
	~DBCView();

	bool Init();
	void Unit();
	void Clear();

	void ConnectToDBCModel(ct::cWeak<DBCModel> vDBCModel);

	void DrawDialogsAndPopups(const std::string& vUserDatas);

	void DrawTree();
	void DrawView();

private:
	// draw View
	void DrawViewDBCFile_AllNetworks(DBCFile_Shared vDBCFilePtr);
	void DrawViewDBCFile_OneNetwork(DBCNetwork_Shared vDBCNetworkPtr, bool vDrawTableHeader);
	void DrawViewDBCFile_OneNetwork_SubContent(DBCNetwork_Shared vDBCNetworkPtr, bool vDrawTableHeader);
	
	void DrawViewDBCFile_AllNodes(DBCFile_Shared vDBCFilePtr);
	void DrawViewDBCFile_OneNode(DBCNode_Shared vDBCNodePtr, bool vDrawTableHeader);
	void DrawViewDBCFile_OneNode_SubContent(DBCNode_Shared vDBCNodePtr, bool vDrawTableHeader);
	
	void DrawViewDBCFile_AllMessages(DBCFile_Shared vDBCFilePtr);
	void DrawViewDBCFile_OneMessage(DBCMessage_Weak vMessage);
	void DrawViewDBCFile_OneMessage(DBCMessage_Shared vDBCMessagePtr, bool vDrawTableHeader);
	void DrawViewDBCFile_OneMessage_SubContent(DBCMessage_Shared vDBCMessagePtr, bool vDrawTableHeader);
	
	void DrawViewDBCFile_AllSignals(DBCFile_Shared vDBCFilePtr);
	void DrawViewDBCFile_OneSignal(DBCSignal_Shared vDBCSignalPtr, bool vDrawTableHeader);

	// draw Tree
	bool SelectableTreeNode(void* vVoidPtr, bool vIsLeaf, const char* fmt, ...);
	bool SelectableTreeNode(DBCObject_Shared vDBCObjectPtr, bool vLocalErrorsOnly, bool vIsLeaf, const char* fmt, ...);

	void DrawTreeDBCFile_AllNetworks(const char* vLabel, DBCFile_Shared vDBCFilePtr);
	void DrawTreeDBCFile_OneNetwork(DBCNetwork_Shared vNetworkPtr, bool vLocalErrorsOnly);

	void DrawTreeDBCFile_AllNodes(const char* vLabel, DBCFile_Shared vDBCFilePtr);
	void DrawTreeDBCFile_OneNode(DBCNode_Shared vDBCNodePtr, bool vLocalErrorsOnly);

	void DrawTreeDBCFile_AllMessages(const char* vLabel, DBCFile_Shared vDBCFilePtr);
	void DrawTreeDBCFile_AllMessages_OneMessage(DBCMessage_Weak vMessage);
	void DrawTreeDBCFile_OneMessage(DBCMessage_Shared vDBCMessagePtr, bool vLocalErrorsOnly);

	void DrawTreeDBCFile_AllSignals(const char* vLabel, DBCFile_Shared vDBCFilePtr);
	void DrawTreeDBCFile_OneSignal(DBCSignal_Shared vDBCSignalPtr, bool vLocalErrorsOnly);
};
