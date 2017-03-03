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

class operator_maintenance extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "Operator Maintenance";
		$this->MakeL("OperatorID", "Return", "FormData");

		$this->Operator = new operator($this->OperatorID);

		if ($this->OperatorID)
		{
			if (!is_array($this->FormData))
			{
				$this->FormData = $this->Operator->GetData();
			}
		}

		$this->States = tools::GetStateDrop();
	}

	function Head()
	{
		parent::Head();
		echo '<script type="text/javascript" src="js/jquery.js"></script>'."\n";
		echo tools::GetAutocompleteSuburbSearchJS("suburb", "state", "postcode");
		echo tools::GetAutocompleteSuburbSearchJS("postal_suburb", "postal_state", "postal_postcode");
        echo tools::GetAutocompleteOperatorSearchJS($this->URL("audit|operator_maintenance"));
        echo tools::GetAutocompleteContactSearchJS($this->URL("audit|operator_maintenance"));
		echo "<script type='text/javascript'>\n";
		echo "function CopyAddress()\n";
		echo "{\n";
		echo "	document.getElementById('postal_address_1').value = document.getElementById('address_1').value;\n";
		echo "	document.getElementById('postal_address_2').value = document.getElementById('address_2').value;\n";
		echo "	document.getElementById('postal_suburb').value = document.getElementById('suburb').value;\n";
		echo "	document.getElementById('postal_state').value = document.getElementById('state').value;\n";
		echo "	document.getElementById('postal_postcode').value = document.getElementById('postcode').value;\n";
		echo "}\n";
		echo "</script>\n";
	}

	function PageContent($Message = "")
	{
		if ($Message)
		{
			$this->ShowMessage($Message);
		}

		$T = new Table();

		$T->AddHead("Name:");
		$T->AddData(field::GetText("FormData[legal_name]", $this->FormData["legal_name"], 64, 128));

		$T->AddHead("Trading Name:");
		$T->AddData(field::GetText("FormData[trading_name]", $this->FormData["trading_name"], 64, 128), 3);
		$T->EndRow();

		$T->AddHead("ABN:");
		$T->AddData(field::GetText("FormData[abn]", $this->FormData["abn"], 16, 16));

		$T->AddHead("Owners Name:");
		$T->AddData(field::GetText("FormData[owners_name]", $this->FormData["owners_name"], 64, 128), 3);
		$T->EndRow();

		$T->AddHead("Phone:");
		$T->AddData(field::GetText("FormData[phone]", $this->FormData["phone"], 32, 64));

		$T->AddHead("Fax:");
		$T->AddData(field::GetText("FormData[fax]", $this->FormData["fax"], 32, 64), 3);
		$T->EndRow();

		$T->AddHead("Customer Number:");
		$T->AddData(field::GetText("FormData[customer_number]", $this->FormData["customer_number"], 16, 16), 5);
		$T->EndRow();
//		$T->AddHead("Website:");
//		$T->AddData(field::GetText("FormData[website]", $this->FormData["website"], 64, 128), 3);
//		$T->EndRow();

		$T->AddHead("Physical Address", 6);
		$T->EndRow();

		$T->AddHead("Address:");
		$T->AddData(field::GetText("FormData[address_1]", $this->FormData["address_1"], 128, 128, "id='address_1'"), 5);
		$T->EndRow();

		$T->AddHead("");
		$T->AddData(field::GetText("FormData[address_2]", $this->FormData["address_2"], 128, 128, "id='address_2'"), 5);
		$T->EndRow();

		$T->AddHead("Suburb:");
		$T->AddData("<div class='ui-widget'>".field::GetText("FormData[suburb]", $this->FormData["suburb"], 64, 128, "id='suburb'")."</div>");

		$T->AddHead("State:");
		$T->AddData(field::GetDrop("FormData[state]", $this->FormData["state"], $this->States, "id='state'"));

		$T->AddHead("Postcode:");
		$T->AddData(field::GetText("FormData[postcode]", $this->FormData["postcode"], 10, 4, "id='postcode'"));
		$T->EndRow();

		$T->AddHead("Postal Address", 2);
		$T->AddHead("<button id='copy_address' type='button' onClick='CopyAddress()'>Copy Address</button>", 4);
		$T->EndRow();
		$T->AddHead("Address:");
		$T->AddData(field::GetText("FormData[postal_address_1]", $this->FormData["postal_address_1"], 128, 128, "id='postal_address_1'"), 5);
		$T->EndRow();

		$T->AddHead("");
		$T->AddData(field::GetText("FormData[postal_address_2]", $this->FormData["postal_address_2"], 128, 128, "id='postal_address_2'"), 5);
		$T->EndRow();

		$T->AddHead("Suburb:");
		$T->AddData("<div class='ui-widget'>".field::GetText("FormData[postal_suburb]", $this->FormData["postal_suburb"], 64, 128, "id='postal_suburb'")."</div>");

		$T->AddHead("State:");
		$T->AddData(field::GetDrop("FormData[postal_state]", $this->FormData["postal_state"], $this->States, "id='postal_state'"));

		$T->AddHead("Postcode:");
		$T->AddData(field::GetText("FormData[postal_postcode]", $this->FormData["postal_postcode"], 10, 4, "id='postal_postcode'"));

		$T->EndRow();

		$T->AddHead("<center>".field::GetFormSubmit("Save")."</center>", 2);
		if ($this->OperatorID)
		{
			$T->AddHead("<center><button id='audit_maint' type='button' onClick='parent.location=\"".$this->URL("audit|audit_main", array("OperatorID" => $this->OperatorID))."\"'>Go to Audits</button></center>", 4);
		}
		else
			$T->AddHead("", 4);
		$T->EndRow();

		DivBox::PutBoxStart("form_block");
		field::PutFormStart($this->URL("audit|operator_maintenance_save"));
		$T->Put();

		// Force to text option
		field::PutHidden("OperatorID", $this->OperatorID);
		field::PutHidden("Return", $this->Return);
		field::PutFormEnd();
		DivBox::PutBoxEnd();
	}

	function MenuExtra()
	{
		echo "<div class='search_block'>\n";
		echo tools::GetAutocompleteContactSearch();
		echo "</div>\n";
		echo "<div class='search_block'>\n";
		echo tools::GetAutocompleteOperatorSearch();
		echo "</div>\n";
	}

}

?>
