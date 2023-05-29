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
#include <Headers/Globals.h>
#include <ctools/cTools.h>
#include <ctools/ConfigAbstract.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <vector>
#include <string>

class DBCFile;
class DBCModel;
class ErrorsPane : public AbstractPane, public conf::ConfigAbstract
{
private:
	ct::cWeak<DBCModel> m_DBCModel;

public:
	bool Init() override;
	void Unit() override;
	int DrawPanes(int vWidgetId, std::string vUserDatas) override;
	void DrawDialogsAndPopups(std::string vUserDatas) override;
	int DrawWidgets(int vWidgetId, std::string vUserDatas) override;
	std::string getXml(const std::string& vOffset, const std::string& vUserDatas = "") override;
	bool setFromXml(tinyxml2::XMLElement* vElem, tinyxml2::XMLElement* vParent, const std::string& vUserDatas = "") override;

	void ConnectToDBCModel(ct::cWeak<DBCModel> vDBCModel);

private:
	void DrawParams();
	bool DrawErrorCategory(std::shared_ptr<DBCFile> vDBCFilePtr, const char* vLabel, const DBCErrorTypeFlags& vFlags);

public: // singleton
	static ErrorsPane *Instance()
	{
		static ErrorsPane _instance;
		return &_instance;
	}

protected:
	ErrorsPane(); // Prevent construction
	ErrorsPane(const ErrorsPane&) {}; // Prevent construction by copying
	ErrorsPane& operator =(const ErrorsPane&) { return *this; }; // Prevent assignment
	~ErrorsPane(); // Prevent unwanted destruction};
};

