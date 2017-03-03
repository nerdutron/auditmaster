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

require_once(ToObjects("audit")."main_operator_search.inc");
require_once(ToIncludes("audit")."tools.inc");

class main_operator_search_display extends main_operator_search
{
	function __construct()
	{
		parent::__construct();

		$this->MakeL("FormData");
//		$this->Operator = new operator($this->OperatorID);

//		if ($this->OperatorID)
//		{
//			if (!is_array($this->FormData))
//			{
//				$this->FormData = $this->Operator->GetData();
//			}
//		}

		$this->FormData["name"] = str_replace(" ", "%", $this->FormData["name"]);
		$this->FormData["name"] = str_replace(".", "%", $this->FormData["name"]);

		if (is_array($this->FormData["abn"]))
			$this->FormData["abn"] = trim(implode(" ", $this->FormData["abn"]));
		if (is_array($this->FormData["phone"]))
			$this->FormData["phone"] = trim(implode(" ", $this->FormData["phone"]));
		if (is_array($this->FormData["fax"]))
			$this->FormData["fax"] = trim(implode(" ", $this->FormData["fax"]));
		$this->Result = operator::OperatorFullSearch($this->FormData);
	}


	function PageContent($Message = "")
	{
		if ($Message)
		{
			$this->ShowMessage($Message);
		}

		$T = new Table();

		$Columns = 10;

		$T->AddHead("Name");
//		$T->AddHead("Trading Name");
//		$T->AddHead("ABN");
		$T->AddHead("Owners Name");
		$T->AddHead("Phone");
		$T->AddHead("Suburb");
		$T->AddHead("State");
		$T->AddHead("Postcode");
		$T->AddHead("Contact");
		$T->AddHead("Contact Phone");
		$T->AddHead("");
		$T->AddHead("");
		$T->EndRow();

		$T->AlternateRows("row0", "row1");

		foreach ($this->Result as $Operator)
		{
	//		$T->AddData("<p title='".$Operator["legal_name"]."'>".$Operator["trading_name"]."</p>");
			$T->AddData($Operator["trading_name"], 1, 1, "title='".$Operator["legal_name"]."'");
//			$T->AddData($Operator["legal_name"]);
//			$T->AddData($Operator["trading_name"]);
//			$T->AddData($Operator["abn"]);
			$T->AddData($Operator["owners_name"]);
			$T->AddData($Operator["phone"], 1, 1, "nowrap='nowrap'");
			$T->AddData($Operator["suburb"]);
			$T->AddData($Operator["state"]);
			$T->AddData($Operator["postcode"]);

			// Display Contacts
			if (sizeof($Operator["contacts"]) > 0)
			{
				$First = true;
				foreach ($Operator["contacts"] as $Contact)
				{
					if ($First)
						$First = false;
					else
					{
						$T->AddData("", 2);
						$T->EndRow();
						$T->AddData("", 6);
					}
					$T->AddData($Contact["firstname"]." ".$Contact["surname"], 1, 1, "title='".$Contact["mobile"]."'");
//					$T->AddData($Contact["phone"]);
//					$T->AddData($Contact["mobile"]);
					$T->AddData($Contact["phone"], 1, 1, "nowrap='nowrap'");
				}
			}
			else
				$T->AddData("", 2);

			$T->AddData("<a href='".$this->URL("audit|audit_main", array("OperatorID" => $Operator["operator_id"]))."'>Audits</a>");
			$T->AddData("<a href='".$this->URL("audit|operator_maintenance", array("OperatorID" => $Operator["operator_id"]))."'>Edit</a>");

			$T->EndRow();

		}

		$T->AddHead("<center>".sizeof($this->Result)." operators</center>", $Columns - 1);
		$T->AddHead("<center><button onClick='parent.location=\"".$this->URL("audit|operator_maintenance")."\"'>Add New</button></center>");
		$T->EndRow();

		DivBox::PutBoxStart("form_block");
		$T->Put();
		DivBox::PutBoxEnd();
	}

}

?>
