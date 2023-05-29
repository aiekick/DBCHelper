#pragma once

#include <Gui/ImWidgets.h>
#include <Res/CustomFont.h>
#include <Res/CustomFont2.h>

// uncomment and modify defines under for customize ImGuiFileDialog

//#define MAX_FILE_DIALOG_NAME_BUFFER 1024
//#define MAX_PATH_BUFFER_SIZE 1024

// options of use std::filesystem isntead of dirent.h but need c++17
//#define USE_STD_FILESYSTEM

//#define USE_THUMBNAILS
//the thumbnail generation use the stb_image and stb_resize lib who need to define the implementation
//btw if you already use them in your app, you can have compiler error due to "implemntation found in double"
//so uncomment these line for prevent the creation of implementation of these libs again
//#define DONT_DEFINE_AGAIN__STB_IMAGE_IMPLEMENTATION
//#define DONT_DEFINE_AGAIN__STB_IMAGE_RESIZE_IMPLEMENTATION
//#define IMGUI_RADIO_BUTTON RadioButton
//#define DisplayMode_ThumbailsList_ImageHeight 32.0f
#define DisplayMode_FilesList_ButtonString ICON_NDP_FILE_LIST
//#define DisplayMode_FilesList_ButtonHelp "File List"
#define DisplayMode_ThumbailsList_ButtonString ICON_NDP_FILE_LIST_THUMBNAILS
//#define DisplayMode_ThumbailsList_ButtonHelp "Thumbnails List"
#define DisplayMode_ThumbailsGrid_ButtonString ICON_NDP_FILE_GRID_THUMBNAILS
//#define DisplayMode_ThumbailsGrid_ButtonHelp "Thumbnails Grid"

#define USE_EXPLORATION_BY_KEYS
// this mapping by default is for GLFW but you can use another
#ifdef USE_SDL2
	//#include <SDL.h>
	// Up key for explore to the top
	#define IGFD_KEY_UP 265//GLFW_KEY_UP
	// Down key for explore to the bottom
	#define IGFD_KEY_DOWN 264//GLFW_KEY_DOWN
	// Enter key for open directory
	#define IGFD_KEY_ENTER 257//GLFW_KEY_ENTER
	// BackSpace for comming back to the last directory
	#define IGFD_KEY_BACKSPACE 259//GLFW_KEY_BACKSPACE
#else
	//#include <GLFW/glfw3.h> 
	// Up key for explore to the top
	#define IGFD_KEY_UP 265//GLFW_KEY_UP
	// Down key for explore to the bottom
	#define IGFD_KEY_DOWN 264//GLFW_KEY_DOWN
	// Enter key for open directory
	#define IGFD_KEY_ENTER 257//GLFW_KEY_ENTER
	// BackSpace for comming back to the last directory
	#define IGFD_KEY_BACKSPACE 259//GLFW_KEY_BACKSPACE
#endif

// by ex you can quit the dialog by pressing the key excape
#define USE_DIALOG_EXIT_WITH_KEY
#define IGFD_EXIT_KEY 256

// widget
// filter combobox width
//#define FILTER_COMBO_WIDTH 120.0f
// button widget use for compose path
#define IMGUI_PATH_BUTTON ImGui::ContrastedButton
// standar button
#define IMGUI_BUTTON ImGui::ContrastedButton

// locales string
#define createDirButtonString ICON_NDP_ADD
#define okButtonString ICON_NDP_OK " OK"
#define cancelButtonString ICON_NDP_CANCEL " Cancel"
#define resetButtonString ICON_NDP_RESET
#define drivesButtonString ICON_NDP_DRIVES
#define editPathButtonString ICON_NDP2_PENCIL
#define searchString ICON_NDP_SEARCH
#define dirEntryString ICON_NDP2_FOLDER
#define linkEntryString ICON_NDP_LINK
#define fileEntryString ICON_NDP2_FILE
//#define fileNameString "File Name : "
//#define buttonResetSearchString "Reset search"
//#define buttonDriveString "Drives"
//#define buttonEditPathString "Edit path\nYou can also right click on path buttons"
//#define buttonResetPathString "Reset to current directory"
//#define buttonCreateDirString "Create Directory"
//#define OverWriteDialogTitleString "The file Already Exist !"
//#define OverWriteDialogMessageString "Would you like to OverWrite it ?"
//#define OverWriteDialogConfirmButtonString "Confirm"
//#define OverWriteDialogCancelButtonString "Cancel"

// DateTimeFormat
// see strftime functionin <ctime> for customize
// "%Y/%m/%d %H:%M" give 2021:01:22 11:47
// "%Y/%m/%d %i:%M%p" give 2021:01:22 11:45PM
//#define DateTimeFormat "%Y/%m/%d %i:%M%p"

// theses icons will appear in table headers
#define USE_CUSTOM_SORTING_ICON
#define tableHeaderAscendingIcon ICON_NDP_CHEVRON_UP
#define tableHeaderDescendingIcon ICON_NDP_CHEVRON_DOWN
#define tableHeaderFileNameString " File name"
#define tableHeaderFileTypeString " Type"
#define tableHeaderFileSizeString " Size"
#define tableHeaderFileDateTimeString " Date"

#define USE_BOOKMARK
//#define bookmarkPaneWith 150.0f
//#define IMGUI_TOGGLE_BUTTON ToggleButton
#define bookmarksButtonString ICON_NDP_BOOKMARK
//#define bookmarksButtonHelpString "bookmark"
#define addBookmarkButtonString ICON_NDP_ADD
#define removeBookmarkButtonString ICON_NDP_REMOVE