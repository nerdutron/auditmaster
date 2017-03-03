<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: services_save.inc 8 2006-12-10 05:06:30Z scott                   $ //
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

require_once(ToObjects("core")."services_list.inc");

class services_save extends services_list
{
	function __construct()
	{
		parent::__construct();
		$this->MakeL("FormData");

//	function PageContent()
		$this->Errors = array();
		if (trim($this->FormData["code"]) == "")
			$this->Errors[] = "You must provide a Service Code";
		else
		{
			$Query = "SELECT COUNT(*) FROM services WHERE service_code = '".addslashes($this->FormData["code"])."' AND service_id <> ".($this->EditID * 1);
			if (DBGet(ACC_DB, $Query))
				$this->Errors[] = "This Service Code already exists.";
		}
		if (trim($this->FormData["name"]) == "")
			$this->Errors[] = "You must provide a Service Name";
		else
		{
			$Query = "SELECT COUNT(*) FROM services WHERE service_name = '".addslashes($this->FormData["name"])."' AND service_id <> ".($this->EditID * 1);
			if (DBGet(ACC_DB, $Query))
				$this->Errors[] = "This Service Code already exists.";
		}

		if (!sizeof($this->Errors))
		{
			$INQ = new DBInsert;
			$INQ->Save("service_code", $this->FormData["code"]);
			$INQ->Save("service_name", $this->FormData["name"]);
			$INQ->Save("publish", $this->FormData["publish"]);
			if ($this->EditID)
			{
				$INQ->SaveWhere("service_id", $this->EditID);
				$INQ->Run(ACC_DB, "services", "UPDATE");
			}
			else
			{
				$INQ->Run(ACC_DB, "services", "INSERT");
			}
			$INQ->EndQuery();
			$this->Redir($this->URL("services_list"));
		}
	}
			
	function PageContent()
	{
		$this->ShowErrors($this->Errors);
		parent::PageContent();
	}
}

?>
