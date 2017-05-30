<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: box.inc 894 2007-03-18 13:54:52Z scott                           $ //
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
// These functions in this class manage <div> tags in web pages.
// They provide a way of standardising what happens.
//
// Usage:
//
//	DivBox::GetBoxStart($Class, $ShadowClass)
//
//		Returns a string to create the Start of box <div>(s)
//		The optional $Class and $ShadowClass parameters
//		are applied to the <div>(s) as class="xxxx" strings.
//
//	DivBox::PutBoxStart($Class, $ShadowClass)
//
//		As above except that the string is output
//
//	DivBox::GetBoxEnd($Shadow = TRUE|FALSE)
//
//		Returns a string to create the End of box </div>(s)
//		The optional $Shadow parameter indicates whether or
//		not to apply a box shadow.
//
//	DivBox::PutBoxEnd($Shadow = TRUE|FALSE)	 
//
//		As above except that the string is output
//
//	DivBox::GetCreateBox($Content, $Class, $ShadowClass)
//
//		Returns a string containing Start and End box <div>(s)
//		as well as the box content. As explained above the 
//		$Class & $ShadowClass parameters are optional.
//
//	DivBox::PutCreateBox($Content, "Box CSS Class Name", $Shadow = TRUE|FALSE) << bad doco line
//	DivBox::PutCreateBox($Content, $Class, $ShadowClass)
//
//		As above except that the string is output
//

class DivBox
{
	function GetBoxStart($Class = "", $ShadowClass = "")
	{
		$S = "";

		if($ShadowClass) $S .= "<div class=\"".$Class."\"><div class=\"".$ShadowClass."\">";

		$S .= "<div";

		if($Class)
		{
			$S .= " class=\"".$Class;
			if($ShadowClass) $S .= ".inside";
			$S .= "\"";
		}
		$S .= ">";

		return($S."\n");
	}

	function PutBoxStart($Class = "", $ShadowClass = "")
	{
		echo DivBox::GetBoxStart($Class, $ShadowClass);
	}

	function GetBoxEnd($Shadow = FALSE)
	{
		$S = "</div>";

		if($Shadow) $S .= "</div></div>";

		return($S."\n");
	}

	function PutBoxEnd($Shadow = FALSE)
	{
		echo DivBox::GetBoxEnd($Shadow);
	}

	function GetCreateBox($Content, $Class = "", $ShadowClass = "")
	{
		$S = DivBox::GetBoxStart($Class, $ShadowClass);

		$S .= $Content;

		if($ShadowClass) $Shadow = TRUE;
		else $Shadow = FALSE;

		$S .= DivBox::GetBoxEnd($Shadow);

		return($S);
	}

	function PutCreateBox($Content, $Class = "", $ShadowClass = "")
	{
		echo DivBox::GetCreateBox($Content, $Class, $ShadowClass);
	}
}
?>
