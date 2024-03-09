#include "pch.h"
#include "menu.h"

/**
 * \brief sets the visibility flag for all the children of the caller!
 * Does NOT render the children, only sets the flag!
 * \param caller normally this should be "this", for example the button, that's been clicked
 * \return -1 on Error, the number of submenus opened on success
 */
// ReSharper disable once CppParameterMayBeConstPtrOrRef
int AL_GUI::openAllSubMenus(MenuElement* caller, const std::vector<MenuElement*>*)
{
	if(!caller) return -1;
	for(auto& child : caller->children())
	{
		child->setVisibility(true);
	}
	return 0;
}

int AL_GUI::defaultOnClick(MenuElement* caller, const std::vector<MenuElement*>*)
{
	if(!caller) return -1;
	caller->_dev_setDevMessage("CLICKED BUTTON");
	return 0;
}

ALLEGRO_COLOR AL_GUI::rgbaToAllegroColor(RGBA rgba)
{
	return al_map_rgba(rgba.r, rgba.g, rgba.b, rgba.a);
}


AL_GUI::MenuElement::MenuElement(MenuElement&& other) noexcept
{
	*this = std::move(other);
}

AL_GUI::MenuElement& AL_GUI::MenuElement::operator=(MenuElement&& other) noexcept
{
	if (this != &other)
	{
		std::memcpy(this, &other, sizeof(MenuElement));  // NOLINT(bugprone-undefined-memory-manipulation)
		std::memset(&other, 0, sizeof(MenuElement));  // NOLINT(bugprone-undefined-memory-manipulation, cert-oop57-cpp)
	}
	return *this;
}

/**
 * \brief tries to render all the menu elements in the tree. It will find the root and then goes down the tree, rendering all the children of the root.
 * this function doesn't bring any buffer to the screen, it only draws the menu on the buffer or corresponding bitmap
 * \param target the bitmap to draw the menu on. If it is NULL, we try to render on the main display. When this fails, there should be an error in the ALLEGRO Error Handling System, or whatever you implemented there.
 * 
*/
void AL_GUI::MenuElement::renderAll(ALLEGRO_BITMAP* target) const
{
	if(!target)
	{
		target = this->getDisplayBuffer();
		if(!target) return; // could not find the buffer
	}
	if(!this->m_parent)
	{
		this->drawAllChildren(target); // WE ARE THE ROOT
	}
	else
	{
		this->m_parent->renderAll(target); // GO UP THE TREE
	}
}

/**
 * \brief this function will draw THIS menu element on the target bitmap ON TOP. It will not draw the children or parents of this menu element.
 * \param target target to draw this menu on.
 * \return true on success, false on failure
 */
bool AL_GUI::MenuElement::draw(ALLEGRO_BITMAP* target)
{
	if (!target)
	{
		target = this->getDisplayBuffer();
		if(!target) return false; // nothing to draw on
	}
	ALLEGRO_BITMAP *targetBefore = al_get_target_bitmap();
	al_set_target_bitmap(target);

	if(this->m_isVisible)
	{
		const float targetWidth = static_cast<float>(al_get_bitmap_width(target));
		const float targetHeight = static_cast<float>(al_get_bitmap_height(target));
		// DRAW FILLINGS
		// TODO
		// DRAW TEXT
		// TODO
		// DRAW BORDER
		if(this->m_primitiveInformation.hasBorder)
		{

			if(this->m_primitiveInformation.borderIsBitmap )
			{
				
				if(this->m_primitiveInformation.borderBitmaps)
				{
					
					auto * LEFT = m_primitiveInformation.borderBitmaps.left;
					auto * RIGHT = m_primitiveInformation.borderBitmaps.right;
					auto * TOP = m_primitiveInformation.borderBitmaps.top;
					auto * BOTTOM = m_primitiveInformation.borderBitmaps.bottom;

					const auto LEFT_width = static_cast<float>(al_get_bitmap_width(LEFT));
					const auto RIGHT_width = static_cast<float>(al_get_bitmap_width(LEFT));
					const auto TOP_width = static_cast<float>(al_get_bitmap_width(LEFT));
					const auto BOTTOM_width = static_cast<float>(al_get_bitmap_width(LEFT));
					const auto LEFT_height = static_cast<float>(al_get_bitmap_height(LEFT));
					const auto RIGHT_height = static_cast<float>(al_get_bitmap_height(LEFT));
					const auto TOP_height = static_cast<float>(al_get_bitmap_height(LEFT));
					const auto BOTTOM_height = static_cast<float>(al_get_bitmap_height(LEFT));

					const auto drawLengthTop = targetWidth - RIGHT_width;
					const auto drawHeightRight = targetHeight - BOTTOM_height;
					const auto drawLengthBottom = targetWidth - LEFT_width;
					const auto drawHeightLeft = targetHeight - TOP_height;

					// TODO TEST THIS!!
					// BORDER TOP
					al_draw_scaled_bitmap(TOP, 0,0, TOP_width, TOP_height, 0, 0, drawLengthTop, TOP_height, 0);
					// BORDER RIGHT
					al_draw_scaled_bitmap(RIGHT, 0, 0, RIGHT_width, RIGHT_height, drawLengthTop, 0, RIGHT_width, drawHeightRight, 0);
					//BORDER BOTTOM
					al_draw_scaled_bitmap(BOTTOM, 0, 0, BOTTOM_width, BOTTOM_height, RIGHT_width, drawHeightRight, drawLengthBottom, BOTTOM_height, 0);
					// BORDER LEFT
					al_draw_scaled_bitmap(LEFT, 0, 0, LEFT_width, LEFT_height, 0, TOP_height, BOTTOM_width, drawHeightLeft, 0);
				}
			}
			else // PRIMITIVE BORDERS
			{
				const auto leftBorderSize = static_cast<float>(m_primitiveInformation.borderSize.left);
				const auto rightBorderSize = static_cast<float>(m_primitiveInformation.borderSize.right);
				const auto topBorderSize = static_cast<float>(m_primitiveInformation.borderSize.top);
				const auto bottomBorderSize = static_cast<float>(m_primitiveInformation.borderSize.bottom);

				const ALLEGRO_COLOR colorLeft = rgbaToAllegroColor(m_primitiveInformation.borderColors.left);
				const ALLEGRO_COLOR colorRight = rgbaToAllegroColor(m_primitiveInformation.borderColors.right);
				const ALLEGRO_COLOR colorTop = rgbaToAllegroColor(m_primitiveInformation.borderColors.top);
				const ALLEGRO_COLOR colorBottom = rgbaToAllegroColor(m_primitiveInformation.borderColors.bottom);

				// TODO TEST THIS
				al_draw_filled_rectangle(0, 0, targetWidth - rightBorderSize, topBorderSize, colorTop);
				al_draw_filled_rectangle(targetWidth - rightBorderSize, 0, targetWidth, targetHeight - bottomBorderSize, colorRight);
				al_draw_filled_rectangle(targetWidth, targetHeight, leftBorderSize, targetHeight - bottomBorderSize, colorBottom);
				al_draw_filled_rectangle(0, topBorderSize, leftBorderSize, targetHeight, colorLeft);

			}

		}
	}
	al_set_target_bitmap(targetBefore);
}

void AL_GUI::MenuElement::toggleVisibility()
{
	m_isVisible = !m_isVisible;
}

void AL_GUI::MenuElement::addCallBack(MenuCallbackFn_Name(callback))
{
	m_callBacks.push_back(callback);
}

/**
 * \brief call all callbacks with the given arguments in a container
 * \param args a vector that should be as big as the callback function container size.
 * \return a vector with all the return values of the callbacks in the same order as the calls.
 */
std::vector<int> AL_GUI::MenuElement::callAllCallBacks(std::vector<const std::vector<MenuElement*>*>& args)
{

	std::vector<int> ret;
	ret.reserve(m_callBacks.size());
	if(args.size() < m_callBacks.size())
	{
		size_t diff = m_callBacks.size() - args.size();
		args.reserve(m_callBacks.size());
		while(diff > 0) // fill the vector with null pointers, if the size is smaller than the callback container
		{
			args.emplace_back(nullptr);
			diff--;
		}
	}
	size_t i = 0;
	for(const auto& function: m_callBacks)
	{
		ret.emplace_back(function(this, args[i]));
		i++;
	}
	return ret;
}


/**
 * \brief 
 * \param args the argument to give to ALL the callbacks
 * \return a vector with all the return values of the callbacks in the same order as the calls
 */
std::vector<int> AL_GUI::MenuElement::callAllCallBacks(const std::vector<MenuElement*>* args)
{
	std::vector<int> ret;
	ret.reserve(m_callBacks.size());
	for(const auto& function : m_callBacks)
	{
		ret.emplace_back(function(this, args));
	}
	return ret;
}

/**
 * \brief setter for the menu. Give it the name of the variable and the value to set it to. --> this function tries to be very smart, so maybe don't use it too often!!
 * \param varSet variable to set
 * \param value value to set the variable with
 * \return true, if variable was found and could be set!
 */
bool AL_GUI::MenuElement::set(const std::string& varSet, const std::string& value)
{
	// TODO: implement this function after the class is "finished"
	return false;
}

/**
 * \brief setter for the menu. Give it the name of the variable and the value to set it to (whenever it is possible to set a number). --> this function tries to be very smart, so maybe don't use it too often!!
 * \param varSet variable to set
 * \param value value to set the variable with
 * \return true, if variable was found and could be set!
 */
bool AL_GUI::MenuElement::set(const std::string& varSet, long long value)
{
	// TODO: implement this function after the class is "finished"
	return false;
}

void AL_GUI::MenuElement::setVisibility(const bool visible)
{
	m_isVisible = visible;
}


/**
 * \brief getter for the menu. Give it the name of the variable and it will return the value of the variable as number WHEN IT IS POSSIBLE. --> this function tries to be very smart, so maybe don't use it too often!!
 * \param varGet variable to get
 * \param safeValueHere the value of the variable
 * \return true on success, false on failure
 */
bool AL_GUI::MenuElement::get(const std::string& varGet, long long* safeValueHere)
{
	// TODO : implement this function after the class is "finished"
	return false;
}

const std::vector<AL_GUI::MenuElement*>& AL_GUI::MenuElement::children() const
{
	return m_children;
}

void AL_GUI::MenuElement::_dev_setDevMessage(const std::string_view& message)
{
	_dev_message = message;
}

void AL_GUI::MenuElement::drawAllChildren(ALLEGRO_BITMAP* target) const
{
	if(!target)
	{
		target = this->getDisplayBuffer();
		if(!target) return; // could not find the buffer
	}
	for(const auto& child : m_children)
	{
		child->draw(target);
		child->drawAllChildren(target);
	}
}

ALLEGRO_BITMAP* AL_GUI::MenuElement::getDisplayBuffer() const
{
	return m_display ? al_get_backbuffer(m_display) : nullptr;
}
