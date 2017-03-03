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

require_once(ToObjects("groups")."assign_group.inc");
require_once(ToIncludes("groups")."group.inc");

class assign_save extends assign_group
{
	function __construct()
	{
		parent::__construct();

		$this->MakeL("FormData");
		$this->Error = array();

		// input validation
		if (!$this->FormData["person_id"])
		{
			$this->Error[] = "A Person is Required";
		}

		if (!sizeof($this->Error))
		{
			$this->Assign = new Assign();
			$this->Assign->LoadFromArray($this->FormData);
			$this->Assign->Save();
		}
	}

	function PageContent()
	{
		if (sizeof($this->Error))
		{
			$this->ShowErrors($this->Error);
			parent::PageContent();
		}
		else
		{
			unset($this->FormData);
			$this->ShowErrors("Record Saved");
			$this->Redir("index.php?Action=people|search_person");
		}
	}
}

?>
