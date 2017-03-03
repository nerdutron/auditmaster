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

require_once(ToObjects("core")."secure.inc");
require_once(ToIncludes("core")."user_maintenance_data.inc");

class user_maintenance extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "User Maintenance";
		$this->MakeL("EUserID", "FormData");

		if ($this->EUserID)
		{
			$this->UserMaint = new user_maintenance_data($this->EUserID);

			if (!is_array($this->FormData))
				$this->FormData = $this->UserMaint->GetData();
		}
		else
			$this->UserMaint = new user_maintenance_data();
	}

	function PageContent($Message = "")
	{
		if ($Message)
		{
			$this->ShowMessage($Message);
		}

		$T = new Table();

		$T->AddHead("Name:");
		$T->AddData(field::GetText("FormData[name]", $this->FormData["name"], 64, 128));
		$T->EndRow();
		$T->AddHead("Login:");
		$T->AddData(field::GetText("FormData[login]", $this->FormData["login"], 16, 16));
		$T->EndRow();
		$T->AddHead("Email Address:");
		$T->AddData(field::GetText("FormData[email]", $this->FormData["email"], 64, 128));
		$T->EndRow();
		$T->AddHead("Phone:");
		$T->AddData(field::GetText("FormData[user_phone]", $this->FormData["user_phone"], 32, 64));
		$T->EndRow();
		$T->AddHead("Fax:");
		$T->AddData(field::GetText("FormData[user_fax]", $this->FormData["user_fax"], 32, 64));
		$T->EndRow();
		$T->AddHead("Mobile:");
		$T->AddData(field::GetText("FormData[user_mobile]", $this->FormData["user_mobile"], 32, 64));
		$T->EndRow();
		$T->AddHead("User Type:");
		$T->AddData(field::GetDrop("FormData[user_type]", $this->FormData["user_type"], GetUserTypeDrop()));
		$T->EndRow();
		$T->AddHead("Auditor:");
		$T->AddData(field::GetCheck("FormData[auditor]", $this->FormData["auditor"], "Y"));
		$T->EndRow();
		$T->AddHead("Active:");
		$T->AddData(field::GetDrop("FormData[active]", $this->FormData["active"], array("1" => "Yes", "0" => "No")));
		$T->EndRow();

	
		DivBox::PutBoxStart("form_block");
		field::PutFormStart($this->URL("user_maintenance_save"));
		$T->Put();

		// Force to text option
		field::PutHidden("EUserID", $this->EUserID);
		field::PutFormSubmit("Save");
		field::PutFormEnd();
		DivBox::PutBoxEnd();
	}
}

?>
