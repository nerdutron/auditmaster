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
require_once(ToIncludes("groups")."assign.inc");
require_once(ToIncludes("people")."person.inc");

class assign_group extends base
{

	function __construct()
	{
		parent::__construct();
		$this->Title = "[Sanctuary] Assign Group";
		$this->MakeV("person_id");
		if ($this->person_id)
		{
			$this->FormData["name"] = GetPersonName($this->person_id);
		}

		$this->MakeV("group_id");
		if ($this->group_id && $this->person_id)
		{
			$this->Assign = new Assign($this->group_id, $this->person_id);

			$this->FormData = $this->Assign->GetArray();
			$this->FormData["name"] = GetPersonName($this->FormData["person_id"]);
		}
	}

	function PageContent()
	{
		$T = new Table;

		$T->AddHead("Name:");
		$T->AddData($this->FormData["name"]);
		$T->EndRow();
		$T->AddHead("Group:");
		$T->AddData(field::GetDrop("FormData[group_id]", $this->FormData["group_id"], GetGroupList()));
		$T->EndRow();
		$T->AddHead("Role:");
		$T->AddData(field::GetDrop("FormData[role_id]", $this->FormData["role_id"], GetRoleList()));
		$T->EndRow();
		$T->AddHead("Note:");
		$T->AddData(field::GetText("FormData[note]", $this->FormData["note"], 100, 100));
		$T->EndRow();

		field::PutFormStart($this->URL("groups|assign_save"));
		$T->Put();
		field::PutHidden("FormData[person_id]", $this->person_id);
		field::PutFormSubmit("Save");
		field::PutFormEnd();
	}
}

?>
