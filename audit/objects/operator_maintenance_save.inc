<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: about.inc 8 2006-12-10 05:06:30Z scott                           $ //
//                                                                          //
// 	Object Model															// 
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

require_once(ToObjects("audit")."operator_maintenance.inc");

class operator_maintenance_save extends operator_maintenance
{
	function __construct()
	{
		parent::__construct();

		$this->Operator = new operator($this->OperatorID);

		
		$this->FormData["abn"] = trim(implode(" ", $this->FormData["abn"]));
		$this->FormData["phone"] = trim(implode(" ", $this->FormData["phone"]));
		$this->FormData["fax"] = trim(implode(" ", $this->FormData["fax"]));

		if ($this->Validate())
		{
			$this->Operator->Save($this->FormData, $this->User->UserID());

			if ($this->Return)
				$this->Redir($this->URL($this->Return, array("OperatorID" => $this->OperatorID)));
			$this->Message = "Operator Saved";
			$this->OperatorID = $this->Operator->OperatorID;
		}
	}

	function PageContent()
	{
		parent::PageContent($this->Message);
	}

	function Validate()
	{
		$RequiredFields = array(
			"legal_name" => "Company Name",
			"abn" => "ABN"
		);

		$Errors = array();
	
		$Valid = true;	
		foreach ($RequiredFields as $Field => $Name)
		{
			if (trim($this->FormData[$Field]) == "")
			{
				$Errors[] = $Name." is a required field";
			}
		}

		if (sizeof($Errors) > 0)
		{
			$this->Message = "<ul>";
			foreach ($Errors as $Error)
				$this->Message .= "<li>$Error</li>";
			$this->Message .= "</ul>";
			return false;
		}

		return true;
	}
}

?>
