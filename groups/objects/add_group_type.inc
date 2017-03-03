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

class add_group_type extends base
{

	function __construct()
	{
		parent::__construct();
		$this->Title = "[Sanctuary] Add Group Type";
	}

	function PageContent()
	{
		$T = new Table;

		$T->AddHead("Group Type:");
		$T->AddData(field::GetText("FormData[name]", $this->FormData["name"], 50, 50));
		$T->EndRow();

		field::PutFormStart($this->URL("groups|group_type_save"));
		$T->Put();
		if ($this->group_type_id)
			field::PutHidden("FormData[group_type_id]", $this->group_type_id);
		field::PutFormSubmit("Save");
		field::PutFormEnd();
	}
}

?>
