<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: services_objects_save.inc 8 2006-12-10 05:06:30Z scott           $ //
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

require_once(ToObjects("core")."services_objects.inc");

class services_objects_save extends services_objects
{
	function __construct()
	{
		parent::__construct();
		$this->MakeL("Edit");

		$this->Errors = array();
		if ($this->Object != $this->Edit["secure_object"])
		{
			$Query = "SELECT COUNT(*) FROM service_objects WHERE secure_object = '".addslashes($this->Edit["secure_object"])."'";
			if (DBGet(ACC_DB, $Query))
				$this->Errors[] = "This object is already in use - please choose another.";
		}

		if (!trim($this->Edit["secure_object"]))
			$this->Errors[] = "An Object to secure must be selected from the list.";

		$INQ = new DBInsert;
		$INQ->Save("secure_object", $this->Edit["secure_object"]);
		$INQ->Save("service_id", $this->ViewID);
		$INQ->Save("admin_required", $this->Edit["admin_required"]);

		if ($this->Edit["is_base"] == "Y")
		{
			$INQ->Save("is_base", "Y");
		}
		else
		{
			if (trim($this->Edit["sub_section"]))
				$INQ->Save("sub_section", $this->Edit["sub_section"]);
			else
				$this->Errors[] = "A description of the Service Sub Section is required.";
		}

		if (!sizeof($this->Errors))
		{
			if ($this->Object)
			{
				$Query = "DELETE FROM service_objects WHERE secure_object = '".addslashes($this->Object)."'";
				DB_DoQuery(ACC_DB, $Query);
			}
			$INQ->Run(ACC_DB, "service_objects", "INSERT");
			$INQ->EndQuery();
			$this->Redir($this->URL("services_objects", array("ViewID"=>$this->ViewID)));
		}
	}

	function PageContent()
	{
		$this->ShowErrors($this->Errors);
		parent::PageContent();
	}
}

?>
