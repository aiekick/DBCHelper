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
#include <string>

enum class LanguageEnum
{
	FR = 0,
	EN
};

class TranslationHelper : public conf::ConfigAbstract
{
public:
	static LanguageEnum s_HelpLanguage;

public:
	static const char* layout_menu_name;
	static const char* layout_menu_help;
	static const char* layout_dbc_tree_pane_name;
	static const char* layout_dbc_view_pane_name;
	static const char* layout_matlab_tools_pane_name;
	static const char* layout_cstruct_tools_pane_name;
	static const char* layout_help_pane_name;
	static const char* layout_errors_control_pane_name;
	static const char* layout_errors_summary_pane_name;
	static const char* layout_tools_pane_name;

	static const char* error_pane_select_all;
	static const char* error_pane_select_none;

	static const char* error_pane_local_name;
	static const char* error_pane_local_all;
	static const char* error_pane_local_none;
	static const char* error_pane_local_msg_category_name;
	static const char* error_pane_local_msg_id_out_can_a;
	static const char* error_pane_local_msg_id_out_can_b;
	static const char* error_pane_local_msg_size_higher_64;
	static const char* error_pane_local_msg_not_mapped;
	static const char* error_pane_local_sig_category_name;
	static const char* error_pane_local_sig_bit_end_out_of_64;
	static const char* error_pane_local_sig_bad_bit_count;
	static const char* error_pane_local_sig_bad_min_max;
	static const char* error_pane_local_sig_not_mapped;

	static const char* error_pane_merge_name;
	static const char* error_pane_merge_all;
	static const char* error_pane_merge_none;

	static const char* error_pane_merge_nod_category_name;
	static const char* error_pane_merge_nod_same_name;
	static const char* error_pane_merge_msg_category_name;
	static const char* error_pane_merge_msg_same_name;
	static const char* error_pane_merge_msg_same_id;
	static const char* error_pane_merge_msg_same_id_not_same_name;
	static const char* error_pane_merge_msg_same_name_not_same_id;
	static const char* error_pane_merge_sig_category_name;
	static const char* error_pane_merge_sig_same_name;

	static const char* dbc_view_pane_all_networks_name;
	static const char* dbc_view_pane_all_nodes_name;
	static const char* dbc_view_pane_all_messages_name;
	static const char* dbc_view_pane_all_signals_name;

	static const char* dbc_view_pane_sort_messages;
	static const char* dbc_view_pane_sort_messages_by_name;
	static const char* dbc_view_pane_sort_messages_by_ids;

	static const char* dbc_view_pane_table_to_do;
	static const char* dbc_view_pane_table_not_mapped;

	static const char* dbc_view_pane_table_header_all_networks_name;
	static const char* dbc_view_pane_table_header_all_networks_protocol;
	static const char* dbc_view_pane_table_header_all_networks_comment;
	static const char* dbc_view_pane_table_header_all_networks_bus_type;

	static const char* dbc_view_pane_table_header_all_nodes_name;
	static const char* dbc_view_pane_table_header_all_nodes_messages;
	static const char* dbc_view_pane_table_header_all_nodes_network;

	static const char* dbc_view_pane_table_header_all_messages_name;
	static const char* dbc_view_pane_table_header_all_messages_id;
	static const char* dbc_view_pane_table_header_all_messages_format;
	static const char* dbc_view_pane_table_header_all_messages_dlc;
	static const char* dbc_view_pane_table_header_all_messages_tx_method;
	static const char* dbc_view_pane_table_header_all_messages_cycle_time;
	static const char* dbc_view_pane_table_header_all_messages_transmitter;
	static const char* dbc_view_pane_table_header_all_messages_comment;
	static const char* dbc_view_pane_table_header_all_messages_network;
	static const char* dbc_view_pane_table_header_all_messages_node;

	static const char* dbc_view_pane_table_header_all_signals_name;
	static const char* dbc_view_pane_table_header_all_signals_bit_start;
	static const char* dbc_view_pane_table_header_all_signals_bit_count;
	static const char* dbc_view_pane_table_header_all_signals_byte_order;
	static const char* dbc_view_pane_table_header_all_signals_type;
	static const char* dbc_view_pane_table_header_all_signals_factor;
	static const char* dbc_view_pane_table_header_all_signals_offset;
	static const char* dbc_view_pane_table_header_all_signals_min;
	static const char* dbc_view_pane_table_header_all_signals_max;
	static const char* dbc_view_pane_table_header_all_signals_unit;
	static const char* dbc_view_pane_table_header_all_signals_comment;
	static const char* dbc_view_pane_table_header_all_signals_network;
	static const char* dbc_view_pane_table_header_all_signals_node;
	static const char* dbc_view_pane_table_header_all_signals_message;

	static const char* dbc_view_pane_table_header_one_network_sub_content_name;
	static const char* dbc_view_pane_table_header_one_network_sub_content_count_messages;
	static const char* dbc_view_pane_table_header_one_network_sub_content_network;

	static const char* dbc_view_pane_tree_one_network_name;
	static const char* dbc_view_pane_tree_one_node_name;
	static const char* dbc_view_pane_tree_one_message_name;
	static const char* dbc_view_pane_tree_one_signal_name;

	static const char* mainframe_menubar_file;
	static const char* mainframe_menubar_open_dbc;
	static const char* mainframe_dialog_open_dbc;
	static const char* mainframe_menubar_merge_dbc;
	static const char* mainframe_dialog_merge_dbc;
	static const char* mainframe_menubar_reload;
	static const char* mainframe_menubar_export;
	static const char* mainframe_dialog_export;
	static const char* mainframe_menubar_close;
	static const char* mainframe_menubar_settings;

public:
	void DefineLanguage(LanguageEnum vLanguage, bool vForce = false);
	float DrawMenu();

private:
	void DefineLanguageEN();
	void DefineLanguageFR();

public: // configuration
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas) override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas) override;

public:
	static TranslationHelper* Instance()
	{
		static TranslationHelper _instance;
		return &_instance;
	}

protected:
	TranslationHelper(); // Prevent construction
	TranslationHelper(const TranslationHelper&) {}; // Prevent construction by copying
	TranslationHelper& operator =(const TranslationHelper&) { return *this; }; // Prevent assignment
	~TranslationHelper() = default; // Prevent unwanted destruction
};