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

class main_operator_search extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "Operator Search";

//		$this->Operator = new operator($this->OperatorID);

//		if ($this->OperatorID)
//		{
//			if (!is_array($this->FormData))
//			{
//				$this->FormData = $this->Operator->GetData();
//			}
//		}

		$this->States = tools::GetStateDrop(true);
	}

	function Head()
	{
		parent::Head();

		echo tools::MoveCursorJS();
		echo '<script type="text/javascript" src="js/jquery.js"></script>'."\n";
		echo tools::GetAutocompleteSuburbSearchJS("suburb", "state", "postcode");
		echo tools::GetAutoCompleteOperatorSearchJS($this->URL("audit|audit_main"));
		echo tools::GetAutoCompleteContactSearchJS($this->URL("audit|audit_main"));
		tools::GetDatePopupJS("setup_after");
		tools::GetDatePopupJS("setup_before");
	}

	function PageContent($Message = "")
	{
		if ($Message)
		{
			$this->ShowMessage($Message);
		}

		$T = new Table();

		$T->AddHead("Name:");
		$T->AddData(field::GetText("FormData[name]", $this->FormData["name"], 64, 128), 5);

//		$T->AddHead("Trading Name:");
//		$T->AddData(field::GetText("FormData[trading_name]", $this->FormData["trading_name"], 64, 128), 3);
		$T->EndRow();

		$T->AddHead("ABN:");
		$T->AddData(tools::ABNField());

		$T->AddHead("Owners Name:");
		$T->AddData(field::GetText("FormData[owners_name]", $this->FormData["owners_name"], 64, 128), 3);
		$T->EndRow();

		$T->AddHead("Phone:");
		$T->AddData(tools::PhoneNumberField("phone"));

		$T->AddHead("Fax:");
		$T->AddData(tools::PhoneNumberField("fax"), 3);
		$T->EndRow();

		$T->AddHead("Customer Number:");
		$T->AddData(field::GetText("FormData[customer_number]", $this->FormData["customer_number"], 16, 16));

		$T->AddHead("Email:");
		$T->AddData(field::GetText("FormData[email]", $this->FormData["email"], 64, 128), 3);
		$T->EndRow();

		$T->AddHead("Status:");
		$T->AddData(field::GetDrop("FormData[active]", $this->FormData["active"], array("" => "", 1 => "Active", 0 => "Inactive")));

		$T->AddHead("Setup By:");
		$T->AddData(field::GetDrop("FormData[setup_by]", $this->FormData["setup_by"], tools::GetUsersDrop(true)), 3);
		$T->EndRow();

		if ($this->User->NeedsAdmin(2))
		{
			$T->AddHead("Setup After:");
			$T->AddData(field::GetText("FormData[setup_after]", $this->FormData["setup_after"], 12, 12, "id='setup_after'"));
			$T->AddHead("Setup Before:");
			$T->AddData(field::GetText("FormData[setup_before]", $this->FormData["setup_before"], 12, 12, "id='setup_before'"), 3);
			$T->EndRow();
		}
	
		$T->AddHead("Address", 6);
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

		$T->AddHead("<center>".field::GetFormSubmit("Search")."</center>", 6);
//		$T->AddHead("<center><button onClick='parent.location=\"".$this->URL("audit|operator_maintenance")."\"'>Add New</button></center>", 2);
		$T->EndRow();

		DivBox::PutBoxStart("form_block");
		field::PutFormStart($this->URL("audit|main_operator_search_display"));
		$T->Put();
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
