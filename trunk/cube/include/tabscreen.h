#ifndef _TABSCREEN_H_
#define _TABSCREEN_H_

#include <nds.h>
#include "gadget.h"
#include "gadgetstyle.h"
#include "gadgeteventhandler.h"

namespace WoopsiUI {

	class TabTop;
	class ScrollingPanel;
	class WoopsiString;

	/**
	 * Class providing a calendar/date picker.  Raises an EVENT_ACTION event
	 * when a new date is clicked.
	 */
	class TabScreen : public Gadget, public GadgetEventHandler
	{
	public:
	
	u8 numPages, currentPage;

		/**
		 * Constructor.
		 * @param x The x co-ordinate of the calendar.
		 * @param y The y co-ordinate of the calendar.
		 * @param width The width of the calendar.
		 * @param height The height of the calendar.
		 * @param day The default day to display.
		 * @param month The default month to display.
		 * @param year The default year to display.
		 * @param flags Standard flags.
		 * @param style The style that the gadget should use.  If this is not
		 * specified, the gadget will use the values stored in the global
		 * defaultGadgetStyle object.  The gadget will copy the properties of
		 * the style into its own internal style object.
		 */
		TabScreen(s16 x, s16 y, u16 width, u16 height, int tabs, u32 flags, GadgetStyle* style = NULL);

		/**
		 * Insert the dimensions that this gadget wants to have into the rect
		 * passed in as a parameter.  All co-ordinates are relative to the gadget's
		 * parent.
		 * @param rect Reference to a rect to populate with data.
		 */
		virtual void getPreferredDimensions(Rect& rect) const;

		/**
		 * 
		 * @param rect Reference to a rect to populate with data.
		 */
		virtual void nameTab(u8 pageNum, WoopsiString title);

		/**
		 * 
		 * @param rect Reference to a rect to populate with data.
		 */
		virtual void addGadgetToPage(u8 pageNum, Gadget* gadget);

		/**
		 * Creates Game Page gadgets.
		 */
		virtual void changePage(u8 newPageIndex);
		
		inline int getPageWidth() {return _pageRect.width;};
		inline int getPageHeight() {return _pageRect.height;};

	protected:
	
		TabTop** _tabTop;
		ScrollingPanel** _tabPage;
		TabTop* _selectedTabTop;
		ScrollingPanel* _selectedTabPage;
		Rect _pageRect;
		
		/**
		 * Draw the area of this gadget that falls within the clipping region.
		 * Called by the redraw() function to draw all visible regions.
		 * @param port The GraphicsPort to draw to.
		 * @see redraw()
		 */
		virtual void drawContents(GraphicsPort* port);

		/**
		 * Resize the calendar to the new dimensions.
		 * @param width The new width.
		 * @param height The new height.
		 */
		void onResize(u16 width, u16 height);

		/**
		 * Destructor.
		 */
		virtual ~TabScreen();

		/**
		 * Creates all gadgets but leaves them textless.
		 */
		virtual void buildGUI();
		
		// Gadget handler for Woopsi button presses
		void handleActionEvent(const WoopsiUI::GadgetEventArgs&);

		/**
		 * Copy constructor is protected to prevent usage.
		 */
		inline TabScreen(const TabScreen& calendar) : Gadget(calendar) { };
	};
}

#endif
