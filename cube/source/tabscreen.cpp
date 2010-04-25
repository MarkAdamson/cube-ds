/*
 * Copyright Mark Adamson 2010
 *
 * This file is part of cube-ds.
 *
 * cube-ds is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cube-ds is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cube-ds.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "tabscreen.h"
#include "graphicsport.h"
#include "tabtop.h"
#include "scrollingpanel.h"
#include "woopsistring.h"

using namespace WoopsiUI;

TabScreen::TabScreen(s16 x, s16 y, u16 width, u16 height,int tabs, u32 flags, GadgetStyle* style) : Gadget(x, y, width, height, flags, style)
{	
	numPages=tabs;
	setBorderless(true);
	
	_tabTop = new TabTop*[tabs];
	_tabPage = new ScrollingPanel*[tabs];
	
	buildGUI();
}

TabScreen::~TabScreen()
{
}

void TabScreen::drawContents(GraphicsPort* port)
{
	port->drawFilledRect(0, 0, _width, _height, getBackColour());
	
	port->drawBevelledRect(_pageRect.x-1, _pageRect.y-1, _pageRect.width+2, _pageRect.height+2, getShineColour(), getShadowColour());
	port->drawLine(_pageRect.x, _pageRect.y-1, _pageRect.width, _pageRect.y-1, getBackColour());
	port->drawPixel(_pageRect.width+1, _pageRect.y-1, getShadowColour());
}

void TabScreen::nameTab(u8 pageNum, WoopsiString title)
{
	_tabTop[pageNum]->setText(title);
	_tabTop[pageNum]->redraw();
}

void TabScreen::addGadgetToPage(u8 pageNum, Gadget* gadget)
{
	ScrollingPanel* tabpage;
	if(!(pageNum>=numPages))
	{
		// Locate the old button in the gadget array
		for (int i = _decorationCount; i < _gadgets.size(); i++)
		{
			if (_gadgets[i]->getRefcon()-10 == pageNum)
			{
				tabpage = (ScrollingPanel*)_gadgets[i];
				tabpage->addGadget(gadget);
				return;
			}
		}
	}

}

void TabScreen::buildGUI()
{
	Rect rect, rect1;
	
	getClientRect(rect);
	rect1=rect;
	int tabsWide = rect1.width/60;
	
	rect1.width=rect1.width/tabsWide;
	rect1.height=20;

	int remainder = rect.width-(rect1.width*tabsWide);
	
	int tabsHigh=numPages/tabsWide;
	if(!(numPages%tabsWide)) tabsHigh--;

	getClientRect(_pageRect);
	
	_pageRect.x+=1;
	_pageRect.y+=(tabsHigh+1)*rect1.height+1;
	_pageRect.width-=2;
	_pageRect.height-=(tabsHigh+1)*rect1.height+2;
	//tabsHigh++;
	
	//printf("tabsWide = %i\n", tabsWide);
	//printf("tabsHigh = %i\n", tabsHigh);
	
	rect1.y+=(tabsHigh*rect1.height);
	
	for(int i=0; i<numPages; i++)
	{
		if(((i+1)%tabsWide==0)||((i+1)==numPages)) rect1.width+=remainder;
		
		_tabTop[i] = new TabTop(rect1.x, rect1.y, rect1.width, rect1.height, "");
		if(i==currentPage) _selectedTabTop=_tabTop[i];
		_tabTop[i]->setRefcon(i);
		_tabTop[i]->addGadgetEventHandler(this);
		addGadget(_tabTop[i]);
		
		_tabPage[i] = new ScrollingPanel(_pageRect.x, _pageRect.y, _pageRect.width, _pageRect.height, GADGET_BORDERLESS);
		if(i==currentPage) _selectedTabPage=_tabPage[i];
		_tabPage[i]->hide();
		_tabPage[i]->setRefcon(10+i);
		addGadget(_tabPage[i]);
		
		if((i+1)%tabsWide==0) rect1.width-=remainder;
		
		rect1.x+=rect1.width;
		if((rect1.x+rect1.width)>(rect.x+rect.width))
		{
			rect1.x=rect.x;
			rect1.y-=rect1.height;
			
			if(rect1.y==rect.y)
			{
				if(!(numPages%tabsWide)) rect1.width=rect.width/tabsWide;
				else
				{
					rect1.width=(rect.width/(numPages%tabsWide));
					remainder=(rect.width%(numPages%tabsWide));
				}
			}
		}
	}
	
	_selectedTabTop->setStuckDown(true);
	_selectedTabPage->show();
}

void TabScreen::onResize(u16 width, u16 height)
{

}

// Get the preferred dimensions of the gadget
void TabScreen::getPreferredDimensions(Rect& rect) const {
	rect.x = _x;
	rect.y = _y;
	rect.width = 0;
	rect.height = 0;

	if (!_flags.borderless) {
		//rect.width = _borderSize.left + _borderSize.right;
		//rect.height = _borderSize.top + _borderSize.bottom;
	}

	rect.width += 192;
	rect.height += 256;
}

void TabScreen::changePage(u8 newPageIndex)
{
	_tabTop[currentPage]->setStuckDown(false);
	_tabTop[currentPage]->redraw();
	_tabTop[newPageIndex]->setStuckDown(true);
	_selectedTabTop = _tabTop[newPageIndex];
	
	_tabPage[currentPage]->hide();
	_tabPage[newPageIndex]->show();
	_selectedTabPage = _tabPage[newPageIndex];
	
	
	if(_tabTop[newPageIndex]->getY()<_tabTop[currentPage]->getY())
	{
		bool moved[numPages];
		for(int i=0; i<numPages; i++)
			moved[i]=false;

		int higherY=_tabTop[newPageIndex]->getY();
		int lowerY=_tabTop[currentPage]->getY();

		for(int i=0; i<numPages; i++)
		{
			if(_tabTop[i]->getY()==higherY)
			{
				_tabTop[i]->moveTo(_tabTop[i]->getX()-1, lowerY-1);
				moved[i]=true;
			}
		}

		for(int i=0; i<numPages; i++)
		{
			if((_tabTop[i]->getY()==lowerY)&&moved[i]==false)
				_tabTop[i]->moveTo(_tabTop[i]->getX()-1, higherY-1);
		}
	}
	
	currentPage=newPageIndex;
}


void TabScreen::handleActionEvent(const GadgetEventArgs& e)
{
	int refcon=e.getSource()->getRefcon();
	
	if(refcon<numPages)
	{
		if(refcon!=currentPage)
		{
			changePage(refcon);
		}
	}
}
