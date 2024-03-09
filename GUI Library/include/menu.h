#pragma once
#include "pch.h"

/**
 *	@file menu.h
 *	@brief Contains all the structs, classes and functions to create, manage and modify Menus, Buttons and all other interactive elements.
 *	@author Nico Burkholder
 */


typedef unsigned int px;
#define  MenuCallbackFn int(*)(MenuElement*, const std::vector<MenuElement*>*)
#define MenuCallbackFn_Name(X) int(*(X))(AL_GUI::MenuElement*, const std::vector<AL_GUI::MenuElement*>*)

namespace AL_GUI
{
	class MenuElement;
	// COLLECTION OF DEFAULT CALLBACKS
	int openAllSubMenus(MenuElement* caller, const std::vector<MenuElement*>* = nullptr);
	int defaultOnClick(MenuElement* caller, const std::vector<MenuElement*>* = nullptr);


	// COLLECTION OF REFERENCE RELATED FUNCTIONS IN THE MENUS
	void copyCallbacks(const MenuElement& source, MenuElement& target);

	struct RGBA
	{
		unsigned char r = 0;
		unsigned char g = 0;
		unsigned char b = 0;
		unsigned char a = 0;

		RGBA(const unsigned char value) {r = value; g = value; b= value; a = value;}
		RGBA& operator=(const unsigned char value) { r = value; g = value; b = value; a = value; return *this; }
	};

	ALLEGRO_COLOR rgbaToAllegroColor(RGBA rgba);


	template<typename  T>
	struct CoordinateInformation
	{
		T left = 0;
		T right = 0;
		T top = 0;
		T bottom = 0;

		operator bool() const
		{
			return left && right && top && bottom;
		}
	};

	typedef CoordinateInformation<px> Border;
	typedef CoordinateInformation<ALLEGRO_BITMAP*> BorderBitmaps;
	typedef CoordinateInformation<RGBA> BorderColors;

	struct MenuPrimitiveInformation
	{
		bool hasBorder = false;
		Border borderSize = { 0,0,0,0 };
		bool borderIsBitmap = false;
		BorderBitmaps borderBitmaps = { nullptr,nullptr,nullptr,nullptr };
		BorderColors borderColors = { 0,0,0,0 };

		px height = 0;
		px width = 0;
		bool scaleToSize = false;
		float scaleX = .0;
		float scaleY = .0;
		RGBA fill_color =  0;

		bool printText = false; // this will be the value the object, that contains this struct
		bool hasIcon = false;
		ALLEGRO_BITMAP * icon = nullptr;

		bool m_isBitmap = false;
		ALLEGRO_BITMAP *m_bitmap = nullptr; // ONLY USED, If the whole element is a bitmap
	};

	/**
	 * \brief this class does NOT take any ownership for any elements given to it, for example the display, the bitmap or the parent.
	 */
	class MenuElement
	{
	public:
		MenuElement() = delete;
		/**
		 * \brief // we don't want to copy the children and other references --> don't copy, move or create new menus
		 * There are functions, to (not deep) copy the references, if you really need to.
		 */
		MenuElement(const MenuElement& other) = delete; 
		MenuElement(MenuElement&& other) noexcept;
		MenuElement& operator=(const MenuElement& other) = delete;
		MenuElement& operator=(MenuElement&& other) noexcept;
		~MenuElement() = default;

		// MEMBER FUNCTIONALITY
		void renderAll(ALLEGRO_BITMAP* target = nullptr) const;
		bool draw(ALLEGRO_BITMAP* target = nullptr);
		void toggleVisibility();
		void addCallBack(int (*callback)(MenuElement*, const std::vector<MenuElement*>*));
		std::vector<int> callAllCallBacks(std::vector<const std::vector<MenuElement*>*>& args);
		std::vector<int> callAllCallBacks(const std::vector<MenuElement*>* args);

		bool set(const std::string& varSet, const std::string& value);
		bool set(const std::string& varSet, long long value);
		void setVisibility(bool visible);
		[[nodiscard]] std::string get(const std::string& varGet);
		[[nodiscard]] bool get(const std::string& varGet, long long* safeValueHere);

		[[nodiscard]] const std::vector<MenuElement*>& children() const;

		// DEVELOPMENT
		void _dev_setDevMessage(const std::string_view& message);
	protected:


		
		// MEMBER VARIABLES GENERAL
		std::string m_id;
		std::string m_value;
		std::string m_type;
		std::string m_tooltipText;
		MenuPrimitiveInformation m_primitiveInformation; // all the information regarding size, color, border, etc.

		MenuElement *m_parent = nullptr;
		std::vector<MenuElement*> m_children;
		std::vector<int(*)(MenuElement*, const std::vector<MenuElement*>*)> m_callBacks;
		std::vector<MenuElement*> m_callBackParameters;
		// MEMBER VARIABLES RENDERING
		bool m_isVisible = true;
		bool m_isClickable = true;
		bool m_isActive = true;
		bool m_hasTooltip = false;
		ALLEGRO_DISPLAY *m_display = nullptr;

		// EVENT HANDLING VARIABLES
		int(*m_onClick)(MenuElement*, const std::vector<MenuElement*>*) =  &defaultOnClick;

	private:
		std::string_view _dev_message; // for debugging and development

		// HELPER FUNCTIONS
		void drawAllChildren(ALLEGRO_BITMAP* target = nullptr) const;
		ALLEGRO_BITMAP * getDisplayBuffer() const;
	};
};



