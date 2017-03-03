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

class show_group extends base
{

	function __construct()
	{
		parent::__construct();
		$this->Title = "[Sanctuary] Show Group";

		$this->MakeV("group_id");	

		$Groups = GetGroupDetailList(0, $this->group_id);

		$this->Group = $Groups[$this->group_id];


	} 


	function PageContent()
	{
		$T = new Table;

		$T->AddHead("Name:");
		$T->AddData($this->Group["name"]);
		$T->EndRow();
		$T->AddHead("Location:");
		$T->AddData($this->Group["location_desc"]);
		$T->EndRow();
		$T->AddHead("Status:");
		$T->AddData($this->Group["status_desc"]);
		$T->EndRow();
		$T->AddHead("Group Type:");
		$T->AddData($this->Group["type_desc"]);
		$T->EndRow();

		$Members = GetGroupMembersFull($this->group_id);

		$T->AddHead("<center>Members</center>", 6);
		$T->EndRow();
		$T->AddHead("Name");
		$T->AddHead("Role");
		$T->AddHead("Note");
		$T->AddHead("Assigned");
		$T->AddHead("");
		$T->AddHead("");
		$T->EndRow();

		$Count = 0;

		foreach ($Members as $Member)
		{
			$T->AddData($Member["person_name"]);
			$T->AddData($Member["person_role"]);
			$T->AddData($Member["note"]);
			$T->AddData($Member["added"]);
			$T->AddData("<a href='index.php?Action=groups|assign_group&group_id=".$this->group_id."&person_id=".$Member["person_id"]."'>Edit</a>");
			$T->AddData("<a href='index.php?Action=groups|delete_member&group_id=".$this->group_id."&person_id=".$Member["person_id"]."'>Delete</a>");
			$T->EndRow();
			$Count++;
		}

		$T->AddHead("<center>$Count Members</center>", 6);
		$T->EndRow();

		$T->Put();

	}
}

?>
