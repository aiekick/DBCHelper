// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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

#include <Panes/Abstract/AbstractPane.h>
#include <ctools/ConfigAbstract.h>
#include <ctools/cTools.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <map>
#include <vector>
#include <array>
#include <string>

class MatlabView;
class MatlabPane : public AbstractPane, public conf::ConfigAbstract
{
private:
	ct::cWeak<MatlabView> m_MatlabView;

public:
	bool Init() override;
	void Unit() override;
	int DrawPanes(int vWidgetId, std::string vUserDatas) override;
	void DrawDialogsAndPopups(std::string vUserDatas) override;
	int DrawWidgets(int vWidgetId, std::string vUserDatas) override;
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;

	void ConnectToMatlabView(ct::cWeak<MatlabView> vMatlabView);

private:
	void DrawView();

public: // singleton
	static MatlabPane *Instance()
	{
		static MatlabPane _instance;
		return &_instance;
	}

protected:
	MatlabPane(); // Prevent construction
	MatlabPane(const MatlabPane&) {}; // Prevent construction by copying
	MatlabPane& operator =(const MatlabPane&) { return *this; }; // Prevent assignment
	~MatlabPane(); // Prevent unwanted destruction};
};

