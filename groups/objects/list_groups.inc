<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: header.txt 13 2006-12-10 13:10:12Z scott                         $ //
//                                                                          //
//  Sanctuary Faith Community Management Software                           //
//  Copyright (C) Authors listed in AUTHORS.txt                             //
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

//require_once(ToObjects("core").CONTAINER_PROJECT.".inc");
require_once(ToObjects("groups")."base.inc");
require_once(ToIncludes("groups")."group.inc");

class list_groups extends base
{

	function __construct()
	{
		parent::__construct();
		$this->Title = "[Sanctuary] Search Groups";
	}

	function PageContent()
	{
		$T = new Table;

		$T->AddHead("Name:");
		$T->AddHead("Location:");
		$T->AddHead("Status:");
		$T->AddHead("Group Type:");
		$T->AddHead("");
		$T->AddHead("");
		$T->EndRow();

		$Groups = GetGroupDetailList();

		foreach ($Groups as $Group)
		{
			$T->AddData($Group["name"]);
			$T->AddData($Group["location_desc"]);
			$T->AddData($Group["status_desc"]);
			$T->AddData($Group["type_desc"]);
			$T->AddData("<a href='index.php?Action=groups|add_group&group_id=".$Group["group_id"]."'>Edit Group</a>");
			$T->AddData("<a href='index.php?Action=groups|show_group&group_id=".$Group["group_id"]."'>List Members</a>");
			$T->EndRow();
		}

		$T->Put();

	}
}

?>
