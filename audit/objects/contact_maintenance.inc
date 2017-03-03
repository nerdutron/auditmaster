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
require_once(ToIncludes("audit")."operator.inc");
require_once(ToIncludes("audit")."tools.inc");

class contact_maintenance extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "Contact Maintenance";
		$this->MakeL("OperatorID", "ContactID", "Return", "FormData");

		$this->Operator = new operator($this->OperatorID);

		if ($this->ContactID)
		{
			if (!is_array($this->FormData))
			{
				$this->FormData = $this->Operator->GetContact($this->ContactID);
			}
		}
	}

	function PageContent($Message = "")
	{
		if ($Message)
		{
			$this->ShowMessage($Message);
		}

		$T = new Table();

		$T->AddHead("Surname:");
		$T->AddData(field::GetText("FormData[surname]", $this->FormData["surname"], 64, 128));
		$T->EndRow();

		$T->AddHead("First Name:");
		$T->AddData(field::GetText("FormData[firstname]", $this->FormData["firstname"], 64, 128));
		$T->EndRow();

		$T->AddHead("Phone:");
		$T->AddData(field::GetText("FormData[phone]", $this->FormData["phone"], 32, 64));
		$T->EndRow();

		$T->AddHead("Fax:");
		$T->AddData(field::GetText("FormData[fax]", $this->FormData["fax"], 32, 64));
		$T->EndRow();

		$T->AddHead("Mobile:");
		$T->AddData(field::GetText("FormData[mobile]", $this->FormData["mobile"], 32, 64));
		$T->EndRow();

		$T->AddHead("Email:");
		$T->AddData(field::GetText("FormData[email]", $this->FormData["email"], 64, 128));
		$T->EndRow();

		$T->AddHead("Position:");
		$T->AddData(field::GetText("FormData[position]", $this->FormData["position"], 32, 64));
		$T->EndRow();

		DivBox::PutBoxStart("form_block");
		field::PutFormStart($this->URL("audit|contact_maintenance_save"));
		$T->Put();

		// Force to text option
		field::PutHidden("OperatorID", $this->OperatorID);
		field::PutHidden("ContactID", $this->ContactID);
		field::PutHidden("Return", $this->Return);
		field::PutFormSubmit("Save");
		field::PutFormEnd();
		DivBox::PutBoxEnd();
	}
}

?>
