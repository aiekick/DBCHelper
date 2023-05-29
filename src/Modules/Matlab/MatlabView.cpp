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

#include "MatlabView.h"

#include <Headers/Globals.h>
#include <ctools/FileHelper.h>
#include <Gui/MainFrame.h>
#include <Gui/ImWidgets.h>

#include <Modules/Matlab/MatlabModel.h>

////////////////////////////////////////////////////
//// CTOR/DTOR /////////////////////////////////////
////////////////////////////////////////////////////

MatlabView::MatlabView()
{

}

MatlabView::~MatlabView()
{

}

////////////////////////////////////////////////////
//// INIT/UNIT /////////////////////////////////////
////////////////////////////////////////////////////

bool MatlabView::Init()
{
	return true;
}

void MatlabView::Unit()
{

}

void MatlabView::Clear()
{

}

////////////////////////////////////////////////////
//// CONNECTION TO THE MODEL ///////////////////////
////////////////////////////////////////////////////

void MatlabView::ConnectToMatlabModel(ct::cWeak<MatlabModel> vMatlabModel)
{
	m_MatlabModel = vMatlabModel;
}

////////////////////////////////////////////////////
//// IMGUI DRAW ////////////////////////////////////
////////////////////////////////////////////////////

void MatlabView::DrawView()
{
	Draw_Simulink_BitUnpack();
	Draw_Simulink_BitPack();
}

////////////////////////////////////////////////////
//// IMGUI DRAW : PRIVATE //////////////////////////
////////////////////////////////////////////////////

void MatlabView::Draw_Simulink_BitUnpack()
{
#ifdef USE_MATLAB_MODULES
	auto matlabModelPtr = m_MatlabModel.getValidShared();
	if (!matlabModelPtr)
		return;

	if (ImGui::CollapsingHeader("Matlab : Bit UnPacking"))
	{
		if (ImGui::ContrastedButton("Generate Datas"))
		{
			matlabModelPtr->ComputeButUnpacking();
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(20);
		ImGui::InputText("Delimiter", matlabModelPtr->GetDelimiterBuffer(), 2);
		matlabModelPtr->GetDelimiterBuffer()[1] = '\0';
		ImGui::PopItemWidth();

		if (!matlabModelPtr->GetBitUnpackingBitPatterns().empty())
		{
			ImGui::Text("Bit-Unpacking");

			static float first_text_offset = 150.0f;
			static float first_button_offset = 400.0f;

			if (!matlabModelPtr->GetBitUnpackingErrors().empty())
			{
				ImGui::Separator();

				ImGui::Text("There is some errors :");
				ImGui::Text("%s", matlabModelPtr->GetBitUnpackingErrors().c_str());
			}

			ImGui::Separator();

			{
				ImGui::Text("Bit patterns :");
				ImGui::SameLine(first_button_offset);
				if (ImGui::ContrastedButton("Compact Copy to Clipboard"))
				{
					FileHelper::Instance()->SaveInClipBoard(MainFrame::sMainWindow,
						matlabModelPtr->Get_Compact_String(matlabModelPtr->GetBitUnpackingBitPatterns()));
				}
				for (const auto it : matlabModelPtr->GetBitUnpackingBitPatterns())
				{
					ImGui::Text("%s", it.first.c_str()); ImGui::SameLine(first_text_offset);
					ImGui::PushTextWrapPos(ImGui::GetContentRegionAvailWidth() - 50.0f);
					ImGui::TextUnformatted(it.second.c_str());
					ImGui::PopTextWrapPos();
				}
			}

			ImGui::Separator();

			{
				ImGui::Text("Input port (packed) data types : uint8");
				ImGui::SameLine(first_button_offset);
				if (ImGui::ContrastedButton("Compact Copy to Clipboard"))
				{
					FileHelper::Instance()->SaveInClipBoard(MainFrame::sMainWindow, "uint8");
				}
			}

			ImGui::Separator();

			{
				ImGui::Text("Input port (packed) dimensions :");
				ImGui::SameLine(first_button_offset);
				if (ImGui::ContrastedButton("Compact Copy to Clipboard"))
				{
					FileHelper::Instance()->SaveInClipBoard(MainFrame::sMainWindow, matlabModelPtr->GetBitUnpackingInputPort());
				}
				ImGui::PushTextWrapPos(ImGui::GetContentRegionAvailWidth() - 50.0f);
				ImGui::TextUnformatted(matlabModelPtr->GetBitUnpackingInputPort().c_str());
				ImGui::PopTextWrapPos();
			}

			ImGui::Separator();

			{
				ImGui::Text("Output port (unpacked) data types (cell array) :");
				ImGui::SameLine(first_button_offset);
				if (ImGui::ContrastedButton("Compact Copy to Clipboard"))
				{
					FileHelper::Instance()->SaveInClipBoard(MainFrame::sMainWindow,
						matlabModelPtr->Get_Compact_String(matlabModelPtr->GetBitUnpackingOutputPortDataType()));
				}
				for (const auto it : matlabModelPtr->GetBitUnpackingOutputPortDataType())
				{
					ImGui::Text("%s", it.first.c_str()); ImGui::SameLine(first_text_offset);
					ImGui::PushTextWrapPos(ImGui::GetContentRegionAvailWidth() - 50.0f);
					ImGui::TextUnformatted(it.second.c_str());
					ImGui::PopTextWrapPos();
				}
			}

			ImGui::Separator();

			{
				ImGui::Text("Output port (unpacked) dimensions (cell array) :");
				ImGui::SameLine(first_button_offset);
				if (ImGui::ContrastedButton("Compact Copy to Clipboard"))
				{
					FileHelper::Instance()->SaveInClipBoard(MainFrame::sMainWindow,
						matlabModelPtr->Get_Compact_String(matlabModelPtr->GetBitUnpackingOutputPortCellArray()));
				}
				for (const auto it : matlabModelPtr->GetBitUnpackingOutputPortCellArray())
				{
					ImGui::Text("%s", it.first.c_str()); ImGui::SameLine(first_text_offset);
					ImGui::PushTextWrapPos(ImGui::GetContentRegionAvailWidth() - 50.0f);
					ImGui::TextUnformatted(it.second.c_str());
					ImGui::PopTextWrapPos();
				}
			}
		}
	}
#endif
}

void MatlabView::Draw_Simulink_BitPack()
{
	
}