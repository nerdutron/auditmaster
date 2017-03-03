<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id::                                                                  $ //
//                                                                          //
//  Copyright (C)2007 MU Systems Pty Ltd                                    //
//                                                                          //
//  Author: Scott McClintock                                                //
//                                                                          //
//  This program is free software; you can redistribute it and/or modify    //
//  it under the terms of the GNU General Public License as published by    //
//  the Free Software Foundation; either version 2 of the License, or       //
//  (at your option) any later version.                                     //
//                                                                          //
//  This program is distributed in the hope that it will be useful,         //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of          //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           //
//  GNU General Public License for more details.                            //
//                                                                          //
//  You should have received a copy of the GNU General Public License       //
//  along with this program; if not, write to the Free Software             //
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

//
// These functions in this class manage as simple Tab based output
// interface in web pages. It assumes that6 the whole page is redrawn.
// They provide a way of standardising what happens.
//
// Usage:
//
//	SimpleTab::GetTabStart($TabSet, $Class)
//
//		Returns a string to create the Start of Tab Set.
//		$TabSet is an array of Tabs.  Each Tab array is as follows:
//
//			$Tab["Label"] = LabelString
//			$Tab["Select"] = Y|N
//
//		The Label string may contain anything though mnore often than
//		not it will contain a URL string for switching to that tab
//		(Though typically the currently selected tab might be just a label)
//
//	SimpleTab::PutTabStart($TabSet, $Class)
//
//		As above except that the string is output
//
//	SimpleTab::GetTabEnd()
//
//		Returns a string to create the End of Tab.
//
//	SimpleTab::PutTabEnd()	 
//
//		As above except that the string is output
//
//	SimpleTab::GetCreateTab($Content, $TabSet, $Class)
//
//		Returns a string containing Start and End Tab elements
//		as well as the content passed.
//
//	SimpleTab::PutCreatetab($Content, $TabSet, $Class)
//
//		As above except that the string is output
//
//	Note: Assumes the presence of some CSS items if you want anything useful
//	to be displayed.  The following are examples only but are known to produce
//	useful output in most major browsers (yes even horrible IE6!!)
//
//		
//		.tab_content_container
//		{
//		        background-color: #CCCCCC;
//		        border: solid #999999 1px;
//		        padding: 10px;
//		        margin-top: 5px;
//		}
//		
//		.tab_content
//		{
//		        background-color: #CCCCCC;
//		}
//		
//		.visible_tab
//		{
//		        border-left: solid #999999 1px;
//		        border-right: solid #999999 1px;
//		        border-top: solid #999999 1px;
//		        -moz-border-radius-topright: 6px;
//		        background-color: #CCCCCC;
//		        padding: 5px 5px 6px 5px;
//		}
//		
//		.invisible_tab
//		{
//		        background-color: #CCCCCC;
//		        border: solid #999999 1px;
//		        -moz-border-radius-topright: 6px;
//		        padding: 3px 5px 5px 5px;
//		}


class SimpleTab
{
	function GetTabStart($TabSet, $Class = "")
	{
		$S = "";

		$S .= "<div";
		if($Class)
			$S .= " class=\"".$Class."\"";
		$S .= ">\n";
		$S .= "<div class=\"tab_header\">\n";

		foreach($TabSet as $Tab)
		{
			if($Tab[Selected] == "Y")
				$TabClass = "visible_tab";
			else
				$TabClass = "invisible_tab";

			$S .= "<span class=\"".$TabClass."\">".$Tab["Label"]."</span>";
		}

		$S .= "</div>\n";
		$S .= "<div class=\"tab_content_container\">\n";
		$S .= "<div class=\"tab_content\">\n";

		return($S);
	}

	function PutTabStart($TabSet, $Class = "")
	{
		echo SimpleTab::GetTabStart($TabSet, $Class);
	}
	
	function GetTabEnd()
	{
		$S .= "</div>\n";
		$S .= "</div>\n";
		$S .= "</div>\n";

		return($S);
	}

	function PutTabEnd($TabSet)
	{
		echo SimpleTab::GetTabEnd($TabSet);
	}
	
	function GetCreateTab($Content, $TabSet, $Class = "")
	{
		$S = SimpleTab::GetTabStart($TabSet, $Class);

		$S .= $Content;

		$S .= SimpleTab::GetTabEnd();

		return($S);
	}

	function PutCreateTab($Content, $TabSet, $Class = "")
	{
		echo SimpleTab::GetCreateTab($Content, $TabSet, $Class);
	}
}
?>
