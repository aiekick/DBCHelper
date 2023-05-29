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

#include "TranslationHelper.h"

#include <ctools/cTools.h>
#include <Res/CustomFont.h>
#include <Res/CustomFont2.h>

///////////////////////////////////////////////////////
//// STATIC ///////////////////////////////////////////
///////////////////////////////////////////////////////

LanguageEnum TranslationHelper::s_HelpLanguage = LanguageEnum::FR;

const char* TranslationHelper::layout_menu_name = nullptr;
const char* TranslationHelper::layout_menu_help = nullptr;
const char* TranslationHelper::layout_dbc_tree_pane_name = nullptr;
const char* TranslationHelper::layout_dbc_view_pane_name = nullptr;
const char* TranslationHelper::layout_matlab_tools_pane_name = nullptr;
const char* TranslationHelper::layout_cstruct_tools_pane_name = nullptr;
const char* TranslationHelper::layout_help_pane_name = nullptr;
const char* TranslationHelper::layout_errors_control_pane_name = nullptr;
const char* TranslationHelper::layout_errors_summary_pane_name = nullptr;
const char* TranslationHelper::layout_tools_pane_name = nullptr;

const char* TranslationHelper::error_pane_select_all = nullptr;
const char* TranslationHelper::error_pane_select_none = nullptr;

const char* TranslationHelper::error_pane_local_name = nullptr;
const char* TranslationHelper::error_pane_local_all = nullptr;
const char* TranslationHelper::error_pane_local_none = nullptr;
const char* TranslationHelper::error_pane_local_msg_category_name = nullptr;
const char* TranslationHelper::error_pane_local_msg_id_out_can_a = nullptr;
const char* TranslationHelper::error_pane_local_msg_id_out_can_b = nullptr;
const char* TranslationHelper::error_pane_local_msg_size_higher_64 = nullptr;
const char* TranslationHelper::error_pane_local_msg_not_mapped = nullptr;
const char* TranslationHelper::error_pane_local_sig_category_name = nullptr;
const char* TranslationHelper::error_pane_local_sig_bit_end_out_of_64 = nullptr;
const char* TranslationHelper::error_pane_local_sig_bad_bit_count = nullptr;
const char* TranslationHelper::error_pane_local_sig_bad_min_max = nullptr;
const char* TranslationHelper::error_pane_local_sig_not_mapped = nullptr;

const char* TranslationHelper::error_pane_merge_name = nullptr;
const char* TranslationHelper::error_pane_merge_all = nullptr;
const char* TranslationHelper::error_pane_merge_none = nullptr;

const char* TranslationHelper::error_pane_merge_nod_category_name = nullptr;
const char* TranslationHelper::error_pane_merge_nod_same_name = nullptr;
const char* TranslationHelper::error_pane_merge_msg_category_name = nullptr;
const char* TranslationHelper::error_pane_merge_msg_same_name = nullptr;
const char* TranslationHelper::error_pane_merge_msg_same_id = nullptr;
const char* TranslationHelper::error_pane_merge_msg_same_id_not_same_name = nullptr;
const char* TranslationHelper::error_pane_merge_msg_same_name_not_same_id = nullptr;
const char* TranslationHelper::error_pane_merge_sig_category_name = nullptr;
const char* TranslationHelper::error_pane_merge_sig_same_name = nullptr;

const char* TranslationHelper::dbc_view_pane_all_networks_name = nullptr;
const char* TranslationHelper::dbc_view_pane_all_nodes_name = nullptr;
const char* TranslationHelper::dbc_view_pane_all_messages_name = nullptr;
const char* TranslationHelper::dbc_view_pane_all_signals_name = nullptr;

const char* TranslationHelper::dbc_view_pane_sort_messages = nullptr;
const char* TranslationHelper::dbc_view_pane_sort_messages_by_name = nullptr;
const char* TranslationHelper::dbc_view_pane_sort_messages_by_ids = nullptr;

const char* TranslationHelper::dbc_view_pane_table_to_do = nullptr;
const char* TranslationHelper::dbc_view_pane_table_not_mapped = nullptr;

const char* TranslationHelper::dbc_view_pane_table_header_all_networks_name = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_networks_protocol = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_networks_comment = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_networks_bus_type = nullptr;

const char* TranslationHelper::dbc_view_pane_table_header_all_nodes_name = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_nodes_messages = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_nodes_network = nullptr;

const char* TranslationHelper::dbc_view_pane_table_header_all_messages_name = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_messages_id = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_messages_format = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_messages_dlc = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_messages_tx_method = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_messages_cycle_time = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_messages_transmitter = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_messages_comment = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_messages_network = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_messages_node = nullptr;

const char* TranslationHelper::dbc_view_pane_table_header_all_signals_name = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_bit_start = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_bit_count = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_byte_order = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_type = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_factor = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_offset = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_min = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_max = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_unit = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_comment = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_network = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_node = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_all_signals_message = nullptr;

const char* TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_name = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_count_messages = nullptr;
const char* TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_network = nullptr;

const char* TranslationHelper::dbc_view_pane_tree_one_network_name = nullptr;
const char* TranslationHelper::dbc_view_pane_tree_one_node_name = nullptr;
const char* TranslationHelper::dbc_view_pane_tree_one_message_name = nullptr;
const char* TranslationHelper::dbc_view_pane_tree_one_signal_name = nullptr;

const char* TranslationHelper::mainframe_menubar_file = nullptr;
const char* TranslationHelper::mainframe_menubar_open_dbc = nullptr;
const char* TranslationHelper::mainframe_dialog_open_dbc = nullptr;
const char* TranslationHelper::mainframe_menubar_merge_dbc = nullptr;
const char* TranslationHelper::mainframe_dialog_merge_dbc = nullptr;
const char* TranslationHelper::mainframe_menubar_reload = nullptr;
const char* TranslationHelper::mainframe_menubar_export = nullptr;
const char* TranslationHelper::mainframe_dialog_export = nullptr;
const char* TranslationHelper::mainframe_menubar_close = nullptr;
const char* TranslationHelper::mainframe_menubar_settings = nullptr;

///////////////////////////////////////////////////////
//// CTOR /////////////////////////////////////////////
///////////////////////////////////////////////////////

TranslationHelper::TranslationHelper()
{
	DefineLanguage(LanguageEnum::FR, true);
}

///////////////////////////////////////////////////////
//// CHANGE LANGUAGE //////////////////////////////////
///////////////////////////////////////////////////////

void TranslationHelper::DefineLanguage(LanguageEnum vLanguage, bool vForce)
{
	if (vLanguage != TranslationHelper::s_HelpLanguage || vForce)
	{
		TranslationHelper::s_HelpLanguage = vLanguage;

		switch (vLanguage)
		{
		case LanguageEnum::EN:
			DefineLanguageEN();
			break;
		case LanguageEnum::FR:
			DefineLanguageFR();
			break;
		default:
			break;
		}
	}
}

///////////////////////////////////////////////////////
//// CHANGE IMGUI MENU ////////////////////////////////
///////////////////////////////////////////////////////

float TranslationHelper::DrawMenu()
{
	float last_cur_pos = ImGui::GetCursorPosX();

	if (ImGui::MenuItem("EN", nullptr, TranslationHelper::s_HelpLanguage == LanguageEnum::EN))
	{
		DefineLanguage(LanguageEnum::EN);
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Change the translation to the English\nBut you need to restart the app\nAnd dont forgot reset the layout to the default after the restart\nIf you have a shity layout");
	}

	if (ImGui::MenuItem("FR", nullptr, TranslationHelper::s_HelpLanguage == LanguageEnum::FR))
	{
		DefineLanguage(LanguageEnum::FR);
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Change la traduction pour le Francais\nMais vous devez redemarrer l'application\n et ne pas oublier de reinitialier la disposition par defaut apres le redemarrage\nSi vous avez une disposition a la con");
	}

	return ImGui::GetCursorPosX() - last_cur_pos + ImGui::GetStyle().FramePadding.x;
}

///////////////////////////////////////////////////////
//// CHANGE LANGUAGE : PRIVATE ////////////////////////
///////////////////////////////////////////////////////

void TranslationHelper::DefineLanguageEN()
{
	TranslationHelper::layout_menu_name = ICON_NDP2_DESKTOP_MAC " Layouts";
	TranslationHelper::layout_menu_help = "Default Layout";
	TranslationHelper::layout_dbc_tree_pane_name = "DBC Tree";
	TranslationHelper::layout_dbc_view_pane_name = "DBC View";
	TranslationHelper::layout_matlab_tools_pane_name = "Matlab Tools";
	TranslationHelper::layout_cstruct_tools_pane_name = "C Structures Tools";
	TranslationHelper::layout_help_pane_name = "Help";
	TranslationHelper::layout_errors_control_pane_name = "Errors Settings";
	TranslationHelper::layout_errors_summary_pane_name = "Errors Summary";
	TranslationHelper::layout_tools_pane_name = "Other Tools";

	TranslationHelper::error_pane_select_all = "Select All";
	TranslationHelper::error_pane_select_none = "Select None";

	TranslationHelper::error_pane_local_name = "Internal Error Type";
	TranslationHelper::error_pane_local_all = "All Internal errors";
	TranslationHelper::error_pane_local_none = "No Internal errors";
	TranslationHelper::error_pane_local_msg_category_name = "Messages";
	TranslationHelper::error_pane_local_msg_id_out_can_a = "Id out of CAN 2.0A Range";
	TranslationHelper::error_pane_local_msg_id_out_can_b = "Id out of CAN 2.0B Range";
	TranslationHelper::error_pane_local_msg_size_higher_64 = "Size higher than 64bits";
	TranslationHelper::error_pane_local_msg_not_mapped = "Not mapped to a node";
	TranslationHelper::error_pane_local_sig_category_name = "Signals";
	TranslationHelper::error_pane_local_sig_bit_end_out_of_64 = "(Start + Size) are out of 64 bits";
	TranslationHelper::error_pane_local_sig_bad_bit_count = "Bad bit count ";
	TranslationHelper::error_pane_local_sig_bad_min_max = "Bad min / max ";
	TranslationHelper::error_pane_local_sig_not_mapped = "Not mapped to a message";

	TranslationHelper::error_pane_merge_name = "Merge Error Types";
	TranslationHelper::error_pane_merge_all = "All merge errors";
	TranslationHelper::error_pane_merge_none = "No merge errors";

	TranslationHelper::error_pane_merge_nod_category_name = "Nodes";
	TranslationHelper::error_pane_merge_nod_same_name = "Same Name##nod";
	TranslationHelper::error_pane_merge_msg_category_name = "Messages";
	TranslationHelper::error_pane_merge_msg_same_name = "Same Name##msg";
	TranslationHelper::error_pane_merge_msg_same_id = "Same ID";
	TranslationHelper::error_pane_merge_msg_same_id_not_same_name = "Same ID but not same Name";
	TranslationHelper::error_pane_merge_msg_same_name_not_same_id = "Same Name but not same ID";
	TranslationHelper::error_pane_merge_sig_category_name = "Signals";
	TranslationHelper::error_pane_merge_sig_same_name = "Same Name##sig";

	TranslationHelper::dbc_view_pane_all_networks_name = "Networks :";
	TranslationHelper::dbc_view_pane_all_nodes_name = "Nodes :";
	TranslationHelper::dbc_view_pane_all_messages_name = "Messages :";
	TranslationHelper::dbc_view_pane_all_signals_name = "Signals :";

	TranslationHelper::dbc_view_pane_sort_messages = "Sort Messages";
	TranslationHelper::dbc_view_pane_sort_messages_by_name = "By Names";
	TranslationHelper::dbc_view_pane_sort_messages_by_ids = "By Ids";

	TranslationHelper::dbc_view_pane_table_to_do = "to do";
	TranslationHelper::dbc_view_pane_table_not_mapped = "Not mapped";

	TranslationHelper::dbc_view_pane_table_header_all_networks_name = "Name";
	TranslationHelper::dbc_view_pane_table_header_all_networks_protocol = "Protocol";
	TranslationHelper::dbc_view_pane_table_header_all_networks_comment = "Comment";
	TranslationHelper::dbc_view_pane_table_header_all_networks_bus_type = "Bus Type";

	TranslationHelper::dbc_view_pane_table_header_all_nodes_name = "Name";
	TranslationHelper::dbc_view_pane_table_header_all_nodes_messages = "Messages";
	TranslationHelper::dbc_view_pane_table_header_all_nodes_network = "Network";

	TranslationHelper::dbc_view_pane_table_header_all_messages_name = "Name";
	TranslationHelper::dbc_view_pane_table_header_all_messages_id = "ID";
	TranslationHelper::dbc_view_pane_table_header_all_messages_format = "Format";
	TranslationHelper::dbc_view_pane_table_header_all_messages_dlc = "DLC (Bytes)";
	TranslationHelper::dbc_view_pane_table_header_all_messages_tx_method = "Tx Method";
	TranslationHelper::dbc_view_pane_table_header_all_messages_cycle_time = "Cycle Time";
	TranslationHelper::dbc_view_pane_table_header_all_messages_transmitter = "Transmitter";
	TranslationHelper::dbc_view_pane_table_header_all_messages_comment = "Comment";
	TranslationHelper::dbc_view_pane_table_header_all_messages_network = "Network";
	TranslationHelper::dbc_view_pane_table_header_all_messages_node = "Node";

	TranslationHelper::dbc_view_pane_table_header_all_signals_name = "Name";
	TranslationHelper::dbc_view_pane_table_header_all_signals_bit_start = "Bit Start";
	TranslationHelper::dbc_view_pane_table_header_all_signals_bit_count = "Bit Count";
	TranslationHelper::dbc_view_pane_table_header_all_signals_byte_order = "Byte Order";
	TranslationHelper::dbc_view_pane_table_header_all_signals_type = "Type";
	TranslationHelper::dbc_view_pane_table_header_all_signals_factor = "Factor";
	TranslationHelper::dbc_view_pane_table_header_all_signals_offset = "Offset";
	TranslationHelper::dbc_view_pane_table_header_all_signals_min = "Min";
	TranslationHelper::dbc_view_pane_table_header_all_signals_max = "Max";
	TranslationHelper::dbc_view_pane_table_header_all_signals_unit = "Unit";
	TranslationHelper::dbc_view_pane_table_header_all_signals_comment = "Commment";
	TranslationHelper::dbc_view_pane_table_header_all_signals_network = "Network";
	TranslationHelper::dbc_view_pane_table_header_all_signals_node = "Node";
	TranslationHelper::dbc_view_pane_table_header_all_signals_message = "Message";

	TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_name = "Name";
	TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_count_messages = "Count Messages";
	TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_network = "Network";

	TranslationHelper::dbc_view_pane_tree_one_network_name = "";
	TranslationHelper::dbc_view_pane_tree_one_node_name = "Node : %s";
	TranslationHelper::dbc_view_pane_tree_one_message_name = "";
	TranslationHelper::dbc_view_pane_tree_one_signal_name = "";

	TranslationHelper::mainframe_menubar_file = ICON_NDP2_FILE " File";
	TranslationHelper::mainframe_menubar_open_dbc = ICON_NDP2_FOLDER " Open a DBC";
	TranslationHelper::mainframe_dialog_open_dbc = "Open a DBC File";
	TranslationHelper::mainframe_menubar_merge_dbc = ICON_NDP2_FORMAT_VERTICAL_ALIGN_BOTTOM " Merge some DBC's";
	TranslationHelper::mainframe_dialog_merge_dbc = "Merge some DBC File(s)";
	TranslationHelper::mainframe_menubar_reload = ICON_NDP_REFRESH " Reload";
	TranslationHelper::mainframe_menubar_export = ICON_NDP_FLOPPY_O " Export";
	TranslationHelper::mainframe_dialog_export = "Export DBC File";
	TranslationHelper::mainframe_menubar_close = ICON_NDP_BAN " Close DBC";
	TranslationHelper::mainframe_menubar_settings = ICON_NDP_COG " Settings";
}

void TranslationHelper::DefineLanguageFR()
{
	TranslationHelper::layout_menu_name = ICON_NDP2_DESKTOP_MAC " Dispositions";
	TranslationHelper::layout_menu_help = "Disposition par defaut";
	TranslationHelper::layout_dbc_tree_pane_name = "Arbre DBC";
	TranslationHelper::layout_dbc_view_pane_name = "Vue DBC";
	TranslationHelper::layout_matlab_tools_pane_name = "Outils Matlab";
	TranslationHelper::layout_cstruct_tools_pane_name = "Outils C Structures";
	TranslationHelper::layout_help_pane_name = "Aide";
	TranslationHelper::layout_errors_control_pane_name = "Reglage des Erreurs";
	TranslationHelper::layout_errors_summary_pane_name = "Sommaire des Erreurs";
	TranslationHelper::layout_tools_pane_name = "Outils Autres";

	TranslationHelper::error_pane_select_all = "Tout Selectionner";
	TranslationHelper::error_pane_select_none = "Ne rien slectionner";

	TranslationHelper::error_pane_local_name = "Erreurs de type Interne";
	TranslationHelper::error_pane_local_all = "Tout les erreurs internes";
	TranslationHelper::error_pane_local_none = "Pas d'erreurs Internes";
	TranslationHelper::error_pane_local_msg_category_name = "Messages";
	TranslationHelper::error_pane_local_msg_id_out_can_a = "Id en dehors de la plage CAN 2.0A";
	TranslationHelper::error_pane_local_msg_id_out_can_b = "Id en dehors de la plage CAN 2.0B";
	TranslationHelper::error_pane_local_msg_size_higher_64 = "Taille plus grande que 64bits";
	TranslationHelper::error_pane_local_msg_not_mapped = "Pas associe a un noeud";
	TranslationHelper::error_pane_local_sig_category_name = "Signaux";
	TranslationHelper::error_pane_local_sig_bit_end_out_of_64 = "bit de fin en dehors des 64 bits";
	TranslationHelper::error_pane_local_sig_bad_bit_count = "Mauvais nombre de bits";
	TranslationHelper::error_pane_local_sig_bad_min_max = "Mauvaises valeurs Min/Max ";
	TranslationHelper::error_pane_local_sig_not_mapped = "Pas associe a un message";

	TranslationHelper::error_pane_merge_name = "Erreurs de type Fusion";
	TranslationHelper::error_pane_merge_all = "Tout les erreurs de fusion";
	TranslationHelper::error_pane_merge_none = "Pas d'erreurs de fusion";

	TranslationHelper::error_pane_merge_nod_category_name = "Noeuds";
	TranslationHelper::error_pane_merge_nod_same_name = "Meme Nom##nod";
	TranslationHelper::error_pane_merge_msg_category_name = "Messages";
	TranslationHelper::error_pane_merge_msg_same_name = "Meme Nom##msg";
	TranslationHelper::error_pane_merge_msg_same_id = "Meme ID";
	TranslationHelper::error_pane_merge_msg_same_id_not_same_name = "Meme ID mais different Nom";
	TranslationHelper::error_pane_merge_msg_same_name_not_same_id = "Meme Nom mais different ID";
	TranslationHelper::error_pane_merge_sig_category_name = "Signaux";
	TranslationHelper::error_pane_merge_sig_same_name = "Meme Nom##sig";

	TranslationHelper::dbc_view_pane_all_networks_name = "Reseaux :";
	TranslationHelper::dbc_view_pane_all_nodes_name = "Noeuds :";
	TranslationHelper::dbc_view_pane_all_messages_name = "Messages :";
	TranslationHelper::dbc_view_pane_all_signals_name = "Signaux :";

	TranslationHelper::dbc_view_pane_sort_messages = "Trier les Messages";
	TranslationHelper::dbc_view_pane_sort_messages_by_name = "Par noms";
	TranslationHelper::dbc_view_pane_sort_messages_by_ids = "Par Ids";

	TranslationHelper::dbc_view_pane_table_to_do = "a faire";
	TranslationHelper::dbc_view_pane_table_not_mapped = "pas associe";

	TranslationHelper::dbc_view_pane_table_header_all_networks_name = "Nom";
	TranslationHelper::dbc_view_pane_table_header_all_networks_protocol = "Protocole";
	TranslationHelper::dbc_view_pane_table_header_all_networks_comment = "Commentaire";
	TranslationHelper::dbc_view_pane_table_header_all_networks_bus_type = "Type de bus";

	TranslationHelper::dbc_view_pane_table_header_all_nodes_name = "Nom";
	TranslationHelper::dbc_view_pane_table_header_all_nodes_messages = "Messages";
	TranslationHelper::dbc_view_pane_table_header_all_nodes_network = "Reseaux";

	TranslationHelper::dbc_view_pane_table_header_all_messages_name = "Nom";
	TranslationHelper::dbc_view_pane_table_header_all_messages_id = "ID";
	TranslationHelper::dbc_view_pane_table_header_all_messages_format = "Format";
	TranslationHelper::dbc_view_pane_table_header_all_messages_dlc = "DLC (Octets)";
	TranslationHelper::dbc_view_pane_table_header_all_messages_tx_method = "Tx Methode";
	TranslationHelper::dbc_view_pane_table_header_all_messages_cycle_time = "Cycle Time";
	TranslationHelper::dbc_view_pane_table_header_all_messages_transmitter = "Transmetteurr";
	TranslationHelper::dbc_view_pane_table_header_all_messages_comment = "Commentaire";
	TranslationHelper::dbc_view_pane_table_header_all_messages_network = "Reseaux";
	TranslationHelper::dbc_view_pane_table_header_all_messages_node = "Noeud";

	TranslationHelper::dbc_view_pane_table_header_all_signals_name = "Nom";
	TranslationHelper::dbc_view_pane_table_header_all_signals_bit_start = "Bit Start";
	TranslationHelper::dbc_view_pane_table_header_all_signals_bit_count = "Bit Count";
	TranslationHelper::dbc_view_pane_table_header_all_signals_byte_order = "Byte Order";
	TranslationHelper::dbc_view_pane_table_header_all_signals_type = "Type";
	TranslationHelper::dbc_view_pane_table_header_all_signals_factor = "Facteur";
	TranslationHelper::dbc_view_pane_table_header_all_signals_offset = "Offset";
	TranslationHelper::dbc_view_pane_table_header_all_signals_min = "Min";
	TranslationHelper::dbc_view_pane_table_header_all_signals_max = "Max";
	TranslationHelper::dbc_view_pane_table_header_all_signals_unit = "Unit";
	TranslationHelper::dbc_view_pane_table_header_all_signals_comment = "Commentaire";
	TranslationHelper::dbc_view_pane_table_header_all_signals_network = "Reseau";
	TranslationHelper::dbc_view_pane_table_header_all_signals_node = "Noeud";
	TranslationHelper::dbc_view_pane_table_header_all_signals_message = "Message";

	TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_name = "Nom";
	TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_count_messages = "Nombre de Messages";
	TranslationHelper::dbc_view_pane_table_header_one_network_sub_content_network = "Reseau";

	TranslationHelper::dbc_view_pane_tree_one_network_name = "";
	TranslationHelper::dbc_view_pane_tree_one_node_name = "Noeud : %s";
	TranslationHelper::dbc_view_pane_tree_one_message_name = "";
	TranslationHelper::dbc_view_pane_tree_one_signal_name = "";

	TranslationHelper::mainframe_menubar_file = ICON_NDP2_FILE " Fichiers";
	TranslationHelper::mainframe_menubar_open_dbc = ICON_NDP2_FOLDER " Ouvrir une DBC";
	TranslationHelper::mainframe_dialog_open_dbc = "Ouvrir des fichier(s) DBC";
	TranslationHelper::mainframe_menubar_merge_dbc = ICON_NDP2_FORMAT_VERTICAL_ALIGN_BOTTOM " Fusionner des DBC's";
	TranslationHelper::mainframe_dialog_merge_dbc = "Fusionner des fichier(s) DBC";
	TranslationHelper::mainframe_menubar_reload = ICON_NDP_REFRESH " Recharger";
	TranslationHelper::mainframe_menubar_export = ICON_NDP_FLOPPY_O " Exporter";
	TranslationHelper::mainframe_dialog_export = "Exporter un fichier DBC";
	TranslationHelper::mainframe_menubar_close = ICON_NDP_BAN " Fermer la DBC";
	TranslationHelper::mainframe_menubar_settings = ICON_NDP_COG " Reglages";
}

///////////////////////////////////////////////////////
//// CONFIGURATION ////////////////////////////////////
///////////////////////////////////////////////////////

std::string TranslationHelper::getXml(const std::string& vOffset, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	std::string str;

	// the rest
	str += vOffset + "<help_lang>" + ct::toStr((int)TranslationHelper::s_HelpLanguage) + "</help_lang>\n";

	return str;
}

bool TranslationHelper::setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas)
{
	UNUSED(vUserDatas);

	// The value of this child identifies the name of this element
	std::string strName;
	std::string strValue;
	std::string strParentName;

	strName = vElem->Value();
	if (vElem->GetText())
		strValue = vElem->GetText();
	if (vParent != nullptr)
		strParentName = vParent->Value();

	if (strName == "help_lang")
		DefineLanguage((LanguageEnum)ct::ivariant(strValue).GetI());

	return true; // continue for explore childs. need to return false if we want explore child ourselves
}