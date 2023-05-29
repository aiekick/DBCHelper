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

#include "DBCView.h"
#include <Gui/MainFrame.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <imgui/imgui_internal.h>
#include <ctools/FileHelper.h>

#include <Modules/Dbc/DBCModel.h>
#include <Modules/Dbc/DBCFile.h>
#include <Modules/Dbc/DBCObjects/DBCNode.h>
#include <Modules/Dbc/DBCObjects/DBCSignal.h>
#include <Modules/Dbc/DBCObjects/DBCMessage.h>
#include <Modules/Dbc/DBCObjects/DBCNetwork.h>
#include <Modules/Dbc/DBCObjects/DBCGranularErrors.h>

#include <Helper/TranslationHelper.h>

#define LABEL_ALL_NETWORKS TranslationHelper::dbc_view_pane_all_networks_name
#define LABEL_ALL_NODES TranslationHelper::dbc_view_pane_all_nodes_name
#define LABEL_ALL_MESSAGES TranslationHelper::dbc_view_pane_all_messages_name
#define LABEL_ALL_SIGNALS TranslationHelper::dbc_view_pane_all_signals_name

static ImVec4 s_Error_Color = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);

///////////////////////////////////////////////////////////////
// DBCFILE::DBCGranularErrors::DrawOneError ///////////////////
///////////////////////////////////////////////////////////////

void DBCGranularErrors::DrawOneError(const std::string& vError)
{
	if (!vError.empty())
	{
		//auto str = ct::wstring_to_string(vError);
		ImGui::TextColored(s_Error_Color, "%s", vError.c_str());
	}
}

////////////////////////////////////////////////////
//// STATIC ////////////////////////////////////////
////////////////////////////////////////////////////

static ImGuiTableFlags s_Table_Flags =
ImGuiTableFlags_BordersV
| ImGuiTableFlags_ScrollX
| ImGuiTableFlags_ScrollY
| ImGuiTableFlags_SizingFixedFit
| ImGuiTableFlags_BordersInnerH
| ImGuiTableFlags_BordersOuterH
//| ImGuiTableFlags_Resizable
| ImGuiTableFlags_RowBg
//| ImGuiTableFlags_NoBordersInBody
//| ImGuiTableFlags_Sortable
;

////////////////////////////////////////////////////
//// CTOR/DTOR /////////////////////////////////////
////////////////////////////////////////////////////

DBCView::DBCView()
{

}

DBCView::~DBCView()
{

}

////////////////////////////////////////////////////
//// INIT/UNIT /////////////////////////////////////
////////////////////////////////////////////////////

bool DBCView::Init()
{
	return true;
}

void DBCView::Unit()
{
	Clear();
}

void DBCView::Clear()
{

}

////////////////////////////////////////////////////
//// CONNECTION TO THE MODEL ///////////////////////
////////////////////////////////////////////////////

void DBCView::ConnectToDBCModel(ct::cWeak<DBCModel> vDBCModel)
{
	m_DBCModel = vDBCModel;
}

////////////////////////////////////////////////////
//// IMGUI DRAW ////////////////////////////////////
////////////////////////////////////////////////////

void DBCView::DrawDialogsAndPopups(const std::string& vUserDatas)
{

}

void DBCView::DrawTree()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(TranslationHelper::dbc_view_pane_sort_messages))
		{
			if (ImGui::MenuItem(TranslationHelper::dbc_view_pane_sort_messages_by_name, nullptr, m_Show_Tree_ByName_Or_ById == true))
			{
				m_Show_Tree_ByName_Or_ById = !m_Show_Tree_ByName_Or_ById;
			}

			if (ImGui::MenuItem(TranslationHelper::dbc_view_pane_sort_messages_by_ids, nullptr, m_Show_Tree_ByName_Or_ById == false))
			{
				m_Show_Tree_ByName_Or_ById = !m_Show_Tree_ByName_Or_ById;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	auto dbcModulePtr = m_DBCModel.getValidShared();
	if (!dbcModulePtr)
		return;

	if (dbcModulePtr->IsLoaded())
	{
		auto mergedDBCFilePtr = dbcModulePtr->GetMergedDBCFile();
		if (mergedDBCFilePtr)
		{
			DrawTreeDBCFile_AllNetworks(LABEL_ALL_NETWORKS, mergedDBCFilePtr);
			DrawTreeDBCFile_AllNodes(LABEL_ALL_NODES, mergedDBCFilePtr);
			DrawTreeDBCFile_AllMessages(LABEL_ALL_MESSAGES, mergedDBCFilePtr);
			DrawTreeDBCFile_AllSignals(LABEL_ALL_SIGNALS, mergedDBCFilePtr);
		}
	}
}

void DBCView::DrawView()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(TranslationHelper::dbc_view_pane_sort_messages))
		{
			if (ImGui::MenuItem(TranslationHelper::dbc_view_pane_sort_messages_by_name, nullptr, m_Show_Tree_ByName_Or_ById == true))
			{
				m_Show_Tree_ByName_Or_ById = !m_Show_Tree_ByName_Or_ById;
			}

			if (ImGui::MenuItem(TranslationHelper::dbc_view_pane_sort_messages_by_ids, nullptr, m_Show_Tree_ByName_Or_ById == false))
			{
				m_Show_Tree_ByName_Or_ById = !m_Show_Tree_ByName_Or_ById;
			}

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	auto dbcModulePtr = m_DBCModel.getValidShared();
	if (!dbcModulePtr)
		return;

	auto mergedDBCFilePtr = dbcModulePtr->GetMergedDBCFile();
	if (mergedDBCFilePtr)
	{
		// m_SelectedDBCObject have the priority on m_SelectedItemPtr

		auto selectedDBCObjectPtr = m_SelectedDBCObject.getValidShared();
		if (!selectedDBCObjectPtr)
		{
			// selection of main category
			if (m_SelectedItemPtr)
			{
				const char* label = (const char*)m_SelectedItemPtr;
				if (strcmp(label, LABEL_ALL_NETWORKS) == 0)
				{
					DrawViewDBCFile_AllNetworks(mergedDBCFilePtr); // All networks
				}
				else if (strcmp(label, LABEL_ALL_NODES) == 0)
				{
					DrawViewDBCFile_AllNodes(mergedDBCFilePtr); // All Nodes
				}
				else if (strcmp(label, LABEL_ALL_MESSAGES) == 0)
				{
					DrawViewDBCFile_AllMessages(mergedDBCFilePtr); // All Messages
				}
				else if (strcmp(label, LABEL_ALL_SIGNALS) == 0)
				{
					DrawViewDBCFile_AllSignals(mergedDBCFilePtr); // All Signals
				}
			}
		}
		else
		{
			// draw subcontents of each big category
			switch (selectedDBCObjectPtr->type)
			{
			case DBCObjectTypeEnum::NETWORK:
				DrawViewDBCFile_OneNetwork_SubContent(std::static_pointer_cast<DBCNetwork>(selectedDBCObjectPtr), true);
				break;
			case DBCObjectTypeEnum::NODE:
				DrawViewDBCFile_OneNode_SubContent(std::static_pointer_cast<DBCNode>(selectedDBCObjectPtr), true);
				break;
			case DBCObjectTypeEnum::MESSAGE:
				DrawViewDBCFile_OneMessage_SubContent(std::static_pointer_cast<DBCMessage>(selectedDBCObjectPtr), true);
				break;
			case DBCObjectTypeEnum::SIGNAL:
				DrawViewDBCFile_OneSignal(std::static_pointer_cast<DBCSignal>(selectedDBCObjectPtr), true);
				break;

			}
		}
	}
}

////////////////////////////////////////////////////
//// IMGUI DRAW : PRIVATE //////////////////////////
////////////////////////////////////////////////////

////////////////////////////////////////////////////
//// IMGUI DRAW : VIEW /////////////////////////////
////////////////////////////////////////////////////

void DBCView::DrawViewDBCFile_AllNetworks(DBCFile_Shared vDBCFilePtr)
{
	if (!vDBCFilePtr)
		return;

	ImGuiID id = ImGui::GetID(vDBCFilePtr.get());
	if (ImGui::BeginTableEx("##DrawViewDBCFile_AllNetworks", id, 4, s_Table_Flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_networks_name, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_networks_protocol, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_networks_comment, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_networks_bus_type, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();

		for (auto network : vDBCFilePtr->GetContainerNetworksByNames())
		{
			auto networkPtr = network.second.getValidShared();
			if (networkPtr)
			{
				if (ImGui::TableNextColumn())
				{
					ImGui::Text("%s", networkPtr->name.c_str());

					networkPtr->local_errors.DrawErrors();
					networkPtr->merge_errors.DrawErrors();
				}
				if (ImGui::TableNextColumn())
				{
					ImGui::Text(TranslationHelper::dbc_view_pane_table_to_do); // Protocol
				}
				if (ImGui::TableNextColumn())
				{
					ImGui::Text(TranslationHelper::dbc_view_pane_table_to_do); // Comment
				}
				if (ImGui::TableNextColumn())
				{
					ImGui::Text(TranslationHelper::dbc_view_pane_table_to_do); // Bus Type
				}
			}
		}

		ImGui::EndTable();
	}
}

void DBCView::DrawViewDBCFile_AllNodes(DBCFile_Shared vDBCFilePtr)
{
	if (!vDBCFilePtr)
		return;

	ImGuiID id = ImGui::GetID(vDBCFilePtr.get());
	if (ImGui::BeginTableEx("##DrawViewDBCFile_AllNodes", id, 3, s_Table_Flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_nodes_name, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_nodes_messages, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_nodes_network, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();

		for (auto node : vDBCFilePtr->GetContainerNodesByNames())
		{
			auto nodePtr = node.second.getValidShared();
			if (nodePtr)
			{
				if (ImGui::TableNextColumn())
				{
					ImGui::Text("%s", nodePtr->name.c_str());

					nodePtr->local_errors.DrawErrors();
					nodePtr->merge_errors.DrawErrors();
				}
				
				if (ImGui::TableNextColumn())
				{
					ImGui::Text("%u", (uint32_t)nodePtr->messages_by_name.size()); // Count Messages
				}

				if (ImGui::TableNextColumn())
				{
					auto netPtr = nodePtr->parent_network.getValidShared();
					if (netPtr)
					{
						ImGui::Text("%s", netPtr->name.c_str());
					}
					else
					{
						ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
					}
				}
			}
		}

		ImGui::EndTable();
	}
}

void DBCView::DrawViewDBCFile_AllMessages(DBCFile_Shared vDBCFilePtr)
{
	if (!vDBCFilePtr)
		return;

	ImGuiID id = ImGui::GetID(vDBCFilePtr.get());
	if (ImGui::BeginTableEx("##DrawViewDBCFile_AllMessages", id, 10, s_Table_Flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_name, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_id, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_format, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_dlc, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_tx_method, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_cycle_time, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_transmitter, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_comment, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_network, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_node, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();

		if (m_Show_View_ByName_Or_ById)
		{
			for (auto message : vDBCFilePtr->GetContainerMessagesByNames())
			{
				DrawViewDBCFile_OneMessage(message.second);
			}
		}
		else
		{
			for (auto message : vDBCFilePtr->GetContainerMessagesByIds())
			{
				DrawViewDBCFile_OneMessage(message.second);
			}
		}

		ImGui::EndTable();
	}
}

void DBCView::DrawViewDBCFile_AllSignals(DBCFile_Shared vDBCFilePtr)
{
	if (!vDBCFilePtr)
		return;

	ImGuiID id = ImGui::GetID(vDBCFilePtr.get());
	if (ImGui::BeginTableEx("##DrawViewDBCFile_AllSignals", 
		id, 14, s_Table_Flags, ImGui::GetContentRegionAvail()))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_name, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_bit_start, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_bit_count, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_byte_order, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_type, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_factor, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_offset, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_min, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_max, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_unit, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_comment, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_network, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_node, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_message, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();

		for (auto signal : vDBCFilePtr->GetContainerSignalsByNames())
		{
			auto signalPtr = signal.second.getValidShared();
			if (signalPtr)
			{
				if (ImGui::TableNextColumn())
				{
					ImGui::Text("%s", signalPtr->name.c_str());

					signalPtr->local_errors.DrawErrors();
					signalPtr->merge_errors.DrawErrors();
				}

				DrawViewDBCFile_OneSignal(signalPtr, false);
			}
		}

		ImGui::EndTable();
	}
}

/// <summary>
/// will draw the content of  network
/// </summary>
/// <param name="vDBCNetworkPtr"></param>
/// <param name="vDrawTableHeader"></param>
void DBCView::DrawViewDBCFile_OneNetwork_SubContent(DBCNetwork_Shared vDBCNetworkPtr, bool vDrawTableHeader)
{
	if (!vDBCNetworkPtr)
		return;

	ImGuiID id = ImGui::GetID(vDBCNetworkPtr.get());
	if (ImGui::BeginTableEx("##DrawViewDBCFile_OneNetwork_SubContent", id, 3, s_Table_Flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_name, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_count_messages, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_network, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();

		for (auto node : vDBCNetworkPtr->nodes)
		{
			auto nodePtr = node.second.getValidShared();
			if (nodePtr)
			{
				if (ImGui::TableNextColumn())
				{
					ImGui::Text("%s", nodePtr->name.c_str());

					nodePtr->local_errors.DrawErrors();
					nodePtr->merge_errors.DrawErrors();
				}
				if (ImGui::TableNextColumn())
				{
					ImGui::Text("%u", (uint32_t)nodePtr->messages_by_name.size()); // Count Messages
				}
				if (ImGui::TableNextColumn())
				{
					ImGui::Text("%s", vDBCNetworkPtr->name.c_str());
				}
			}
		}

		ImGui::EndTable();
	}
}

/// <summary>
/// will draw the content of a node
/// </summary>
/// <param name="vDBCNodePtr"></param>
/// <param name="vDrawTableHeader"></param>
void DBCView::DrawViewDBCFile_OneNode_SubContent(DBCNode_Shared vDBCNodePtr, bool vDrawTableHeader)
{
	if (!vDBCNodePtr)
		return;

	ImGuiID id = ImGui::GetID(vDBCNodePtr.get());
	if (ImGui::BeginTableEx("##DrawViewDBCFile_OneNode_SubContent", id, 10, s_Table_Flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_name, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_id, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_format, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_dlc, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_tx_method, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_cycle_time, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_transmitter, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_comment, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_network, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_node, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();

		for (auto message : vDBCNodePtr->messages_by_name)
		{
			auto messagePtr = message.second.getValidShared();
			if (messagePtr)
			{
				const dbcppp::IMessage* msg = messagePtr->m_Message;
				if (msg)
				{
					if (ImGui::TableNextColumn())
					{
						ImGui::Text("%s", messagePtr->name.c_str());
					
						messagePtr->local_errors.DrawErrors();
						messagePtr->merge_errors.DrawErrors();
					}
					if (ImGui::TableNextColumn()) ImGui::Text("0x%03X", (uint32_t)msg->Id()); // id
					if (ImGui::TableNextColumn()) ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_to_do); // format (can standard, etc)
					if (ImGui::TableNextColumn()) ImGui::Text("%u", (uint32_t)msg->MessageSize()); // dlc
					if (ImGui::TableNextColumn()) ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_to_do); // tx method
					if (ImGui::TableNextColumn()) ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_to_do); // cycle time
					if (ImGui::TableNextColumn()) ImGui::Text("%s", msg->Transmitter().c_str()); // transmitter
					if (ImGui::TableNextColumn()) ImGui::Text("%s", msg->Comment().c_str()); // Comment

					if (ImGui::TableNextColumn())
					{
						auto netPtr = messagePtr->parent_network.getValidShared();
						if (netPtr)
						{
							ImGui::Text("%s", netPtr->name.c_str());
						}
						else
						{
							ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
						}
					}

					if (ImGui::TableNextColumn())
					{
						if (!messagePtr->parent_nodes.empty())
						{
							for (auto parent_node : messagePtr->parent_nodes)
							{
								auto parent_node_Ptr = parent_node.getValidShared();
								if (parent_node_Ptr)
								{
									ImGui::Text("%s", parent_node_Ptr->name.c_str());
								}
							}
						}
						else
						{
							ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
						}
					}
				}
			}
		}

		ImGui::EndTable();
	}
}

void DBCView::DrawViewDBCFile_OneMessage(DBCMessage_Weak vMessage)
{
	auto messagePtr = vMessage.getValidShared();
	if (messagePtr)
	{
		if (ImGui::TableNextColumn())
		{
			ImGui::Text("%s", messagePtr->name.c_str());

			messagePtr->local_errors.DrawErrors();
			messagePtr->merge_errors.DrawErrors();
		}

		DrawViewDBCFile_OneMessage(messagePtr, false);
	}
}

void DBCView::DrawViewDBCFile_OneMessage(DBCMessage_Shared vDBCMessagePtr, bool vDrawTableHeader)
{
	if (!vDBCMessagePtr)
		return;

	const dbcppp::IMessage* msg = vDBCMessagePtr->m_Message;
	if (msg)
	{
		bool _is_table_opened = false;
		if (vDrawTableHeader)
		{
			ImGuiID id = ImGui::GetID(vDBCMessagePtr.get());
			_is_table_opened = ImGui::BeginTableEx("##DrawViewDBCFile_OneMessage",
				id, 9, s_Table_Flags, ImGui::GetContentRegionAvail());
			if (_is_table_opened)
			{
				ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_name, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_id, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_format, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_dlc, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_tx_method, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_cycle_time, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_transmitter, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_comment, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_network, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_messages_node, ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableHeadersRow();

				if (ImGui::TableNextColumn())
				{
					ImGui::Text("%s", vDBCMessagePtr->name.c_str());

					vDBCMessagePtr->local_errors.DrawErrors();
					vDBCMessagePtr->merge_errors.DrawErrors();
				}
			}
		}

		if (ImGui::TableNextColumn()) ImGui::Text("0x%03X", (uint32_t)msg->Id()); // id
		if (ImGui::TableNextColumn()) ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_to_do); // format (can standard, etc)
		if (ImGui::TableNextColumn()) ImGui::Text("%u", (uint32_t)msg->MessageSize()); // dlc
		if (ImGui::TableNextColumn()) ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_to_do); // tx method
		if (ImGui::TableNextColumn()) ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_to_do); // cycle time
		if (ImGui::TableNextColumn()) ImGui::Text("%s", msg->Transmitter().c_str()); // transmitter
		if (ImGui::TableNextColumn()) ImGui::Text("%s", msg->Comment().c_str()); // Comment

		if (ImGui::TableNextColumn())
		{
			auto netPtr = vDBCMessagePtr->parent_network.getValidShared();
			if (netPtr)
			{
				ImGui::Text("%s", netPtr->name.c_str());
			}
			else
			{
				ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
			}
		}

		if (ImGui::TableNextColumn())
		{
			if (!vDBCMessagePtr->parent_nodes.empty())
			{
				for (auto parent_node : vDBCMessagePtr->parent_nodes)
				{
					auto parent_node_Ptr = parent_node.getValidShared();
					if (parent_node_Ptr)
					{
						ImGui::Text("%s", parent_node_Ptr->name.c_str());
					}
				}
			}
			else
			{
				ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
			}
		}

		if (vDrawTableHeader &&
			_is_table_opened)
		{
			ImGui::EndTable();
		}
	}
}

/// <summary>
/// will draw the content of a message, so the list of signals
/// </summary>
/// <param name="vDBCMessagePtr"></param>
/// <param name="vDrawTableHeader"></param>
void DBCView::DrawViewDBCFile_OneMessage_SubContent(DBCMessage_Shared vDBCMessagePtr, bool vDrawTableHeader)
{
	if (!vDBCMessagePtr)
		return;

	ImGuiID id = ImGui::GetID(vDBCMessagePtr.get());
	if (ImGui::BeginTableEx("##DrawViewDBCFile_OneMessage_SubContent", id, 14, s_Table_Flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_name, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_bit_start, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_bit_count, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_byte_order, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_type, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_factor, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_offset, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_min, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_max, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_unit, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_comment, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_network, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_node, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_message, ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();

		for (auto signal : vDBCMessagePtr->signals)
		{
			auto signalPtr = signal.second.getValidShared();
			if (signalPtr)
			{
				const dbcppp::ISignal* sig = signalPtr->m_Signal;
				if (sig)
				{
					if (ImGui::TableNextColumn())
					{
						ImGui::Text("%s", signalPtr->name.c_str());

						signalPtr->local_errors.DrawErrors();
						signalPtr->merge_errors.DrawErrors();
					}
					if (ImGui::TableNextColumn()) ImGui::Text("%u", (uint32_t)sig->StartBit());
					if (ImGui::TableNextColumn()) ImGui::Text("%u", (uint32_t)sig->BitSize());
					if (ImGui::TableNextColumn()) ImGui::Text("%s", (sig->ByteOrder() == dbcppp::ISignal::EByteOrder::BigEndian) ? "Motorola (BE)" : "Intel (LE)");
					if (ImGui::TableNextColumn())
					{
						if (sig->ExtendedValueType() == dbcppp::ISignal::EExtendedValueType::Double)
							ImGui::Text("%s", "Double");
						else if (sig->ExtendedValueType() == dbcppp::ISignal::EExtendedValueType::Float)
							ImGui::Text("%s", "Float");
						else if (sig->ExtendedValueType() == dbcppp::ISignal::EExtendedValueType::Integer)
							ImGui::Text("%s", (sig->ValueType() == dbcppp::ISignal::EValueType::Signed) ? "Signed" : "Unsigned");
					}
					if (ImGui::TableNextColumn()) ImGui::Text("%.2f", sig->Factor());
					if (ImGui::TableNextColumn()) ImGui::Text("%.2f", sig->Offset());
					if (ImGui::TableNextColumn()) ImGui::Text("%.2f", sig->Minimum());
					if (ImGui::TableNextColumn()) ImGui::Text("%.2f", sig->Maximum());
					if (ImGui::TableNextColumn()) ImGui::Text("%s", sig->Unit().c_str());
					if (ImGui::TableNextColumn())
					{
						ImGui::Text("%s", sig->Comment().c_str());
					}

					if (ImGui::TableNextColumn())
					{
						auto netPtr = signalPtr->parent_network.getValidShared();
						if (netPtr)
						{
							ImGui::Text("%s", netPtr->name.c_str());
						}
						else
						{
							ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
						}
					}

					if (ImGui::TableNextColumn())
					{
						if (!signalPtr->parent_nodes.empty())
						{
							for (auto parent_node : signalPtr->parent_nodes)
							{
								auto parent_node_Ptr = parent_node.getValidShared();
								if (parent_node_Ptr)
								{
									ImGui::Text("%s", parent_node_Ptr->name.c_str());
								}
							}
						}
						else
						{
							ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
						}
					}

					if (ImGui::TableNextColumn())
					{
						if (!signalPtr->parent_messages.empty())
						{
							for (auto parent_message : signalPtr->parent_messages)
							{
								auto parent_message_Ptr = parent_message.getValidShared();
								if (parent_message_Ptr)
								{
									ImGui::Text("%s", parent_message_Ptr->name.c_str());
								}
							}
						}
						else
						{
							ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
						}
					}

				}
			}
		}

		ImGui::EndTable();
	}
}

void DBCView::DrawViewDBCFile_OneSignal(DBCSignal_Shared vDBCSignalPtr, bool vDrawTableHeader)
{
	if (!vDBCSignalPtr)
		return;

	const dbcppp::ISignal* sig = vDBCSignalPtr->m_Signal;

	bool _is_table_opened = false;
	if (vDrawTableHeader)
	{
		ImGuiID id = ImGui::GetID(vDBCSignalPtr.get());
		_is_table_opened = ImGui::BeginTableEx("##DrawViewDBCFile_OneSignal", 
			id, 14, s_Table_Flags, ImGui::GetContentRegionAvail());
		if (_is_table_opened)
		{
			ImGui::TableSetupScrollFreeze(0, 1); // Make header always visible
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_name, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_bit_start, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_bit_count, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_byte_order, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_type, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_factor, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_offset, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_min, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_max, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_unit, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_comment, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_network, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_node, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn(TranslationHelper::dbc_view_pane_table_header_all_signals_message, ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			if (ImGui::TableNextColumn())
			{
				ImGui::Text("%s", vDBCSignalPtr->name.c_str());

				vDBCSignalPtr->local_errors.DrawErrors();
				vDBCSignalPtr->merge_errors.DrawErrors();
			}
		}
	}

	if (ImGui::TableNextColumn()) ImGui::Text("%u", (uint32_t)sig->StartBit());
	if (ImGui::TableNextColumn()) ImGui::Text("%u", (uint32_t)sig->BitSize());

	if (ImGui::TableNextColumn()) ImGui::Text("%s", (sig->ByteOrder() == dbcppp::ISignal::EByteOrder::BigEndian) ? "Motorola (BE)" : "Intel (LE)");
	if (ImGui::TableNextColumn())
	{
		if (sig->ExtendedValueType() == dbcppp::ISignal::EExtendedValueType::Double)
			ImGui::Text("%s", "double");
		else if (sig->ExtendedValueType() == dbcppp::ISignal::EExtendedValueType::Float)
			ImGui::Text("%s", "float");
		else if (sig->ExtendedValueType() == dbcppp::ISignal::EExtendedValueType::Integer)
			ImGui::Text("%s", (sig->ValueType() == dbcppp::ISignal::EValueType::Signed) ? "Signed" : "Unsigned");
	}
	if (ImGui::TableNextColumn()) ImGui::Text("%.5f", sig->Factor());

	if (ImGui::TableNextColumn()) ImGui::Text("%.5f", sig->Offset());
	if (ImGui::TableNextColumn()) ImGui::Text("%.5f", sig->Minimum());
	if (ImGui::TableNextColumn()) ImGui::Text("%.5f", sig->Maximum());

	if (ImGui::TableNextColumn()) ImGui::Text("%s", sig->Unit().c_str());
	if (ImGui::TableNextColumn())
	{
		ImGui::Text("%s", sig->Comment().c_str());
	}

	if (ImGui::TableNextColumn())
	{
		auto netPtr = vDBCSignalPtr->parent_network.getValidShared();
		if (netPtr)
		{
			ImGui::Text("%s", netPtr->name.c_str());
		}
		else
		{
			ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
		}
	}

	if (ImGui::TableNextColumn())
	{
		if (!vDBCSignalPtr->parent_nodes.empty())
		{
			for (auto parent_node : vDBCSignalPtr->parent_nodes)
			{
				auto parent_node_Ptr = parent_node.getValidShared();
				if (parent_node_Ptr)
				{
					ImGui::Text("%s", parent_node_Ptr->name.c_str());
				}
			}
		}
		else
		{
			ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
		}
	}

	if (ImGui::TableNextColumn())
	{
		if (!vDBCSignalPtr->parent_messages.empty())
		{
			for (auto parent_message : vDBCSignalPtr->parent_messages)
			{
				auto parent_message_Ptr = parent_message.getValidShared();
				if (parent_message_Ptr)
				{
					ImGui::Text("%s", parent_message_Ptr->name.c_str());
				}
			}
		}
		else
		{
			ImGui::Text("%s", TranslationHelper::dbc_view_pane_table_not_mapped);
		}
	}

	if (vDrawTableHeader && _is_table_opened)
	{
		ImGui::EndTable();
	}
}

////////////////////////////////////////////////////
//// IMGUI DRAW : TREE /////////////////////////////
////////////////////////////////////////////////////

bool DBCView::SelectableTreeNode(void* vVoidPtr, bool vIsLeaf, const char* fmt, ...)
{
	if (!vVoidPtr)
		return false;

	static ImGuiTreeNodeFlags base_flags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth;

	va_list args;
	va_start(args, fmt);

	ImGuiTreeNodeFlags node_flags = base_flags;
	if (vIsLeaf)
		node_flags |= ImGuiTreeNodeFlags_Leaf;
	if (m_SelectedItemPtr == vVoidPtr)
		node_flags |= ImGuiTreeNodeFlags_Selected;
	bool is_open = ImGui::TreeNodeExV(vVoidPtr, node_flags, fmt, args);
	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	{
		m_SelectedItemPtr = vVoidPtr;
		m_SelectedDBCObject.reset();
	}

	va_end(args);
	return is_open;
}

bool DBCView::SelectableTreeNode(DBCObject_Shared vDBCObjectPtr, bool vLocalErrorsOnly, bool vIsLeaf, const char* fmt, ...)
{
	if (!vDBCObjectPtr)
		return false;

	static ImGuiTreeNodeFlags base_flags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_SpanAvailWidth;

	va_list args;
	va_start(args, fmt);

	ImGuiTreeNodeFlags node_flags = base_flags;
	if (vIsLeaf)
		node_flags |= ImGuiTreeNodeFlags_Leaf;
	if (m_SelectedItemPtr == (void*)vDBCObjectPtr.get())
		node_flags |= ImGuiTreeNodeFlags_Selected;
	
	uint32_t _count_errors = vDBCObjectPtr->local_count_errors;
	if (!vLocalErrorsOnly)
		_count_errors = 
			vDBCObjectPtr->local_count_errors + 
			vDBCObjectPtr->merge_count_errors;

	if (_count_errors)
		ImGui::PushStyleColor(ImGuiCol_Text, s_Error_Color);
	bool is_open = ImGui::TreeNodeExV((void*)vDBCObjectPtr.get(), node_flags, fmt, args);
	if (_count_errors)
		ImGui::PopStyleColor();

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
	{
		m_SelectedItemPtr = (void*)vDBCObjectPtr.get();
		m_SelectedDBCObject.reset();
		if (vDBCObjectPtr)
		{
			if (vDBCObjectPtr->type == DBCObjectTypeEnum::NETWORK ||
				vDBCObjectPtr->type == DBCObjectTypeEnum::NODE ||
				vDBCObjectPtr->type == DBCObjectTypeEnum::MESSAGE ||
				vDBCObjectPtr->type == DBCObjectTypeEnum::SIGNAL)
			{
				m_SelectedDBCObject = vDBCObjectPtr;
			}
		}
	}

	va_end(args);
	return is_open;
}

void DBCView::DrawTreeDBCFile_AllNetworks(const char* vLabel, DBCFile_Shared vDBCFilePtr)
{
	if (!vLabel || !vDBCFilePtr)
		return;

	if (SelectableTreeNode((void*)vLabel, (vDBCFilePtr->GetContainerNetworksByNames().empty()), 
		"%s (%u) (%u Errors)", vLabel, (uint32_t)vDBCFilePtr->GetContainerNetworksByNames().size(), 
		vDBCFilePtr->GetNetworkErrorsCount()))
	{
		DrawTreeDBCFile_OneNetwork(vDBCFilePtr->GetMainNetwork(), true);

		for (auto networks : vDBCFilePtr->GetContainerNetworksByNames())
		{
			auto networkPtr = networks.second.getValidShared();
			if (networkPtr && networkPtr != vDBCFilePtr->GetMainNetwork())
			{
				DrawTreeDBCFile_OneNetwork(networkPtr, true);
			}
		}

		ImGui::TreePop();
	}
}

void DBCView::DrawTreeDBCFile_OneNetwork(DBCNetwork_Shared vNetworkPtr, bool vLocalErrorsOnly)
{
	if (!vNetworkPtr)
		return;

	bool _is_opened = false;

	if (!vNetworkPtr->isMain)
	{
		if (ImGui::ContrastedSmallButton(ICON_NDP_CANCEL "##close"))
		{
			auto dbcModelPtr = m_DBCModel.getValidShared();
			if (dbcModelPtr)
			{
				dbcModelPtr->RemoveDBC(vNetworkPtr->name);
			}
		}

		ImGui::SameLine();
	}

	auto dbcParentPtr = vNetworkPtr->parent_dbc.getValidShared();
	if (dbcParentPtr)
	{
		_is_opened = SelectableTreeNode(vNetworkPtr, vLocalErrorsOnly, false,
			vNetworkPtr->isMain ? "Main : %s (%u Errors)" : "Net : %s (%u Errors)", vNetworkPtr->name.c_str(),
			dbcParentPtr->GetNetworkErrorsCount());
	}
	else
	{
		_is_opened = SelectableTreeNode(vNetworkPtr, vLocalErrorsOnly, false,
			vNetworkPtr->isMain ? "Main : %s" : "Net : %s", vNetworkPtr->name.c_str());
	}	

	if (_is_opened)
	{
		for (auto node : vNetworkPtr->nodes)
		{
			DrawTreeDBCFile_OneNode(node.second.getValidShared(), vLocalErrorsOnly);
		}

		ImGui::TreePop();
	}
}

void DBCView::DrawTreeDBCFile_AllNodes(const char* vLabel, DBCFile_Shared vDBCFilePtr)
{
	if (!vLabel || !vDBCFilePtr)
		return;

	if (SelectableTreeNode((void*)vLabel, (vDBCFilePtr->GetContainerNodesByNames().empty()), 
		"%s (%u) (%u Errors)", vLabel, (uint32_t)vDBCFilePtr->GetContainerNodesByNames().size(), 
		vDBCFilePtr->GetNodeErrorsCount()))
	{
		for (auto nodes : vDBCFilePtr->GetContainerNodesByNames())
		{
			auto nodePtr = nodes.second.getValidShared();
			if (nodePtr)
			{
				auto _total_errors = nodePtr->local_count_errors + nodePtr->merge_count_errors;
				if (SelectableTreeNode(nodePtr, false, (_total_errors == 0) && nodePtr->messages_by_name.empty(), "%s (%u)",
					nodePtr->name.c_str(), (uint32_t)nodePtr->messages_by_name.size()))
				{
					nodePtr->local_errors.DrawErrors();
					nodePtr->merge_errors.DrawErrors();

					if (m_Show_Tree_ByName_Or_ById)
					{
						for (auto message : nodePtr->messages_by_name)
						{
							DrawTreeDBCFile_OneMessage(message.second.getValidShared(), false);
						}
					}
					else
					{
						for (auto message : nodePtr->messages_by_id)
						{
							DrawTreeDBCFile_OneMessage(message.second.getValidShared(), false);
						}
					}

					ImGui::TreePop();
				}
			}
		}

		ImGui::TreePop();
	}
}

void DBCView::DrawTreeDBCFile_OneNode(DBCNode_Shared vDBCNodePtr, bool vLocalErrorsOnly)
{
	if (!vDBCNodePtr)
		return;

	if (SelectableTreeNode(vDBCNodePtr, vLocalErrorsOnly, false, TranslationHelper::dbc_view_pane_tree_one_node_name, vDBCNodePtr->name.c_str()))
	{
		if (m_Show_Tree_ByName_Or_ById)
		{
			for (auto message : vDBCNodePtr->messages_by_name)
			{
				DrawTreeDBCFile_OneMessage(message.second.getValidShared(), vLocalErrorsOnly);
			}
		}
		else
		{
			for (auto message : vDBCNodePtr->messages_by_id)
			{
				DrawTreeDBCFile_OneMessage(message.second.getValidShared(), vLocalErrorsOnly);
			}
		}

		ImGui::TreePop();
	}
}

void DBCView::DrawTreeDBCFile_AllMessages(const char* vLabel, DBCFile_Shared vDBCFilePtr)
{
	if (!vLabel || !vDBCFilePtr)
		return;

	if (SelectableTreeNode((void*)vLabel, (vDBCFilePtr->GetContainerMessagesByNames().empty()), 
		"%s (%u) (%u Errors)", vLabel, (uint32_t)vDBCFilePtr->GetContainerMessagesByNames().size(), 
		vDBCFilePtr->GetMessageErrorsCount()))
	{
		if (m_Show_Tree_ByName_Or_ById)
		{
			for (auto messages : vDBCFilePtr->GetContainerMessagesByNames())
			{
				DrawTreeDBCFile_AllMessages_OneMessage(messages.second);
			}
		}
		else
		{
			for (auto messages : vDBCFilePtr->GetContainerMessagesByIds())
			{
				DrawTreeDBCFile_AllMessages_OneMessage(messages.second);
			}
		}

		ImGui::TreePop();
	}
}

void DBCView::DrawTreeDBCFile_AllMessages_OneMessage(DBCMessage_Weak vMessage)
{
	auto messagePtr = vMessage.getValidShared();
	if (messagePtr)
	{
		bool _opened = false;
		auto _total_errors = messagePtr->local_count_errors + messagePtr->merge_count_errors;

		if (m_Show_Tree_ByName_Or_ById)
		{
			_opened = SelectableTreeNode(messagePtr, false, (_total_errors == 0) && messagePtr->signals.empty(), "Msg %s : 0x%03X (%u)",
				messagePtr->name.c_str(), (uint32_t)messagePtr->id, (uint32_t)messagePtr->signals.size());
		}
		else
		{
			_opened = SelectableTreeNode(messagePtr, false, (_total_errors == 0) && messagePtr->signals.empty(), "Msg 0x%03X : %s (%u)",
				(uint32_t)messagePtr->id, messagePtr->name.c_str(), (uint32_t)messagePtr->signals.size());
		}

		if (_opened)
		{
			if (_total_errors)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, s_Error_Color);

				messagePtr->local_errors.DrawErrors();
				messagePtr->merge_errors.DrawErrors();

				ImGui::PopStyleColor();
			}

			for (auto signal : messagePtr->signals)
			{
				DrawTreeDBCFile_OneSignal(signal.second.getValidShared(), false);
			}

			ImGui::TreePop();
		}
	}
}

void DBCView::DrawTreeDBCFile_OneMessage(DBCMessage_Shared vDBCMessagePtr, bool vLocalErrorsOnly)
{
	if (!vDBCMessagePtr)
		return;

	bool _opened = false;

	if (m_Show_Tree_ByName_Or_ById)
	{
		_opened = SelectableTreeNode(vDBCMessagePtr, vLocalErrorsOnly, vDBCMessagePtr->signals.empty(), "Msg %s : 0x%03X (%u)",
			vDBCMessagePtr->name.c_str(), (uint32_t)vDBCMessagePtr->id, (uint32_t)vDBCMessagePtr->signals.size());
	}
	else
	{
		_opened = SelectableTreeNode(vDBCMessagePtr, vLocalErrorsOnly, vDBCMessagePtr->signals.empty(), "Msg 0x%03X : %s (%u)",
			(uint32_t)vDBCMessagePtr->id, vDBCMessagePtr->name.c_str(), (uint32_t)vDBCMessagePtr->signals.size());
	}

	if (_opened)
	{
		for (auto signal : vDBCMessagePtr->signals)
		{
			DrawTreeDBCFile_OneSignal(signal.second.getValidShared(), vLocalErrorsOnly);
		}

		ImGui::TreePop();
	}
}

void DBCView::DrawTreeDBCFile_AllSignals(const char* vLabel, DBCFile_Shared vDBCFilePtr)
{
	if (!vLabel || !vDBCFilePtr)
		return;

	if (SelectableTreeNode((void*)vLabel, (vDBCFilePtr->GetContainerSignalsByNames().empty()),
		"%s (%u) (%u Errors)", vLabel, (uint32_t)vDBCFilePtr->GetContainerSignalsByNames().size(), 
		vDBCFilePtr->GetSignalErrorsCount()))
	{
		for (auto signals : vDBCFilePtr->GetContainerSignalsByNames())
		{
			auto signalPtr = signals.second.getValidShared();
			if (signalPtr)
			{
				DrawTreeDBCFile_OneSignal(signalPtr, false);
			}
		}

		ImGui::TreePop();
	}
}

/// <summary>
/// Draw the signal starting at column start bit
/// </summary>
/// <param name="vDBCSignalPtr"></param>
void DBCView::DrawTreeDBCFile_OneSignal(DBCSignal_Shared vDBCSignalPtr, bool vLocalErrorsOnly)
{
	if (!vDBCSignalPtr)
		return;

	uint32_t _count_errors = vDBCSignalPtr->local_count_errors;
	if (!vLocalErrorsOnly)
		_count_errors = 
			vDBCSignalPtr->local_count_errors + 
			vDBCSignalPtr->merge_count_errors;

	if (SelectableTreeNode(vDBCSignalPtr, vLocalErrorsOnly, (_count_errors == 0U), "Sig : %s", vDBCSignalPtr->name.c_str()))
	{
		// local errors
		vDBCSignalPtr->local_errors.DrawErrors();

		// merge errors
		if (!vLocalErrorsOnly && _count_errors)
		{
			vDBCSignalPtr->merge_errors.DrawErrors();
		}

		ImGui::TreePop();
	}
}