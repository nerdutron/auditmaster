<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: secure.inc 80 2007-08-03 06:47:18Z rod                           $ //
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

require_once(ToObjects("core")."site.inc");

class secure extends site
{
	function __construct()
	{
		parent::__construct();
		$this->CustomMenus = array();
		$this->Title = "Main Menu";
		$this->InitialiseSecurity();

		if ($this->ValidUser())
		{
			$this->MenuLocation("Main Menu", "secure");
			$this->MainMenu = array();

			// Special option for power user
			if ($this->NeedsAdmin(2))
			{
					$this->MainMenu["Users"] = "user_list";
			}
//			if ($this->AllowAccess("system_administration"))
//				$this->MainMenu["SysAdmin"] = "system_administration";
				$this->MainMenu["Operators"] = "audit|main_operator_search";
//			if ($this->AllowAccess("audit"))
//				$this->MainMenu["Audit"] = "audit|audit_main";
				$this->MainMenu["Report"] = "audit|report";
		}
	}

	function AddItemURL($Title, $URL, $Menu = "")
	{
		if (!is_array($Menu))
			$Menu = array();
		if (sizeof($Menu))
		{
			$this->AddMenuURL($Title, $URL, $Menu);
		}
		else
		{
			$this->MENU .= "<li><a href=\"".$URL."\">".$Title."</a></li>\n";
		//	$this->MENU .= "<li><a href=\"".$URL."\" title=\"".$Title."\">".$Title."</a></li>\n";
		}
	}

	function AddItemObject($Title, $Object, $Params, $Menu = "")
	{
		if (!is_array($Menu))
			$Menu = array();
		if (sizeof($Menu))
		{
			$this->AddMenuObject($Title, $Object, $Params, $Menu);
		}
		else
		{
			if (!is_array($Params))
				$Params = array();
			$this->AddItemURL($Title, $this->URL($Object, $Params));
		}
	}

	function AddMenuURL($Title, $URL, $Menu)
	{
		$this->MENU .= "<li><a class=\"hide\" href=\"".$URL."\">".$Title."</a>\n";
		$this->MENU .= "<!--[if lte IE 6]>\n";
		$this->MENU .= "<a ";
		$this->MENU .= "class=\"sub\" ";
		$this->MENU .= "href=\"".$URL."\">".$Title."\n";
		$this->MENU .= "<table><tr><td>\n";
		$this->MENU .= "<![endif]-->\n";

		if (is_array($Menu))
		{
			if (sizeof($Menu))
			{
				$this->MENU .= "<ul>\n";
				foreach ($Menu as $Item)
				{
					if ($Item["object"])
						$this->AddItemObject($Item["title"], $Item["object"], $Item["params"], $Item["menu"]);
					else
						$this->AddItemURL($Item["title"], $Item["address"], $Item["menu"]);
				}
				$this->MENU .= "</ul>\n";
			}
		}

		$this->MENU .= "<!--[if lte IE 6]>\n";
		$this->MENU .= "</td></tr></table>\n";
		$this->MENU .= "</a>\n";
		$this->MENU .= "<![endif]-->\n";
		$this->MENU .= "</li>\n";
	}

	function AddMenuObject($Title, $Object, $Params, $Menu)
	{
		if (!is_array($Params))
			$Params = array();
		$this->AddMenuURL($Title, $this->URL($Object, $Params), $Menu);
	}

	function MenuBuild($Struct)
	{
		$this->MENU  = "";
		$this->MENU .= "<div class=\"menu\">\n";
		$this->MENU .= "<ul>\n";

		foreach ($Struct as $Item)
		{
			if ($Item["object"])
				$this->AddMenuObject($Item["title"], $Item["object"], $Item["params"], $Item["menu"]);
			else
				$this->AddMenuURL($Item["title"], $Item["address"], $Item["menu"]);
		}

		$this->MENU .= "</ul>\n";
		$this->MENU .= "</div>\n";
		return($this->MENU);
	}
	

	function PageMenu()
	{
		if (defined("EXPERIMENTAL") && $this->ValidUser())
		{
			require_once(ToIncludes("cw")."menu.inc");
			$MB = new menu_builder;

			foreach ($this->CustomMenus as $Location => $Data)
				$MB->set_custom($Location, $Data);

			$MB->create();
			$MAIN_MENU = $MB->get_menu("MAIN");
			echo $this->MenuBuild($MAIN_MENU);
		}
		else
			parent::PageMenu();
	}

	function PageContent()
	{

		if ($this->GetV("IP"))
		{
			echo "<div class=\"alert\">";
			echo "<h2>Site Access Denied</h2>";
			echo "<p>The IP address you are attempting to login from (";
			echo $_SERVER["REMOTE_ADDR"];
			echo ") is not authorised for access to this account. ";
			echo "Please ensure you login from an authosied location. ";
			echo "</p>";
			echo "</div>";
		}

		if ($this->ValidUser())
		{
			field::PutFieldsetStart("Main Menu");
			echo "<ul>\n";
			foreach ($this->MainMenu as $Label => $Object)
				echo "<li><a href=\"".$this->URL($Object)."\">".$Label."</a></li>\n";
			echo "<li><a href=\"".$this->URL("password")."\">Change my Password</a></li>\n";
			$this->ExtraHeader();

			echo "<li>".$this->ActionLink($this->URL("secure_logout"), "Are you sure you wish to logout?", "Logout")."</li>\n";
			echo "</ul>\n";
			field::PutFieldsetEnd();

			if ($this->User->Get("account_id") == 1)
			{
				field::PutFieldsetStart("Files");
				$Query  = "";
				$Query .= "SELECT ";
				$Query .= "file_id, title, data, ext ";
				$Query .= "FROM file_items ";
				$Query .= "ORDER BY title ";
				$Q = new DBQuery;
				$Q->Run(ACC_DB, $Query);

				$T = new CWTable;
				$T->AlternateMouseOver();
				$T->AddHead("Title");
				$T->AddHead("Type");
				$T->AddHead("Download");
				$T->EndRow();
				while ($Q->Next())
				{
					$ID = $Q->GetField("file_id");
					$T->AddData($Q->GetField("title"));
					$Query = "SELECT mime_desc FROM mime_types WHERE extension = '".$Q->GetField("ext")."'";
					$T->AddData(number_format(strlen(gzuncompress(base64_decode($Q->GetField("data")))) / 1000, 1)."Kb ".DBGet(WEB_DB, $Query));
					$T->AddData("<a href=\"file_get.php?ID=".$ID."\">Download</a>");
					$T->EndRow();
				}
				$T->Put();
				$Q->EndQuery();

				field::PutFieldsetEnd();
			}
		}
		else
		{
			if (trim($this->GetV("OS_LOGIN")))
			{
				echo "<div class=\"error_box\">";
				echo "Login and/or Password is incorrect. Please try again.";
				echo "</div>";
			}

			$T = new CWTable;
			$T->AddHead("Login");
			$T->EndRow();
			$T->AddData(field::GetText("OS_LOGIN", "", 16), 1, 1, "", "layout");
			$T->EndRow();
			$T->AddHead("Password");
			$T->EndRow();
			$T->AddData(field::GetPassword("OS_PASSWORD", "", 16), 1, 1, "", "layout");
			$T->EndRow();
			$T->AddData(field::GetFormSubmit("Login", "OS_FORM"), 1, 1, "", "layout");
			$T->EndRow();
			if ($this->GetV("OS_DEST"))
				$Params = array("OS_DEST"=>$this->GetV("OS_DEST"));
			else
				$Params = array("OS_DEST"=>base64_encode(SecurePublicObject::GetHere()));
			field::PutFormStart($this->URL("secure", $Params));
			$T->Put();
			field::PutFormEnd();
		}
	}

	function ShowErrors($Errors)
	{
		if (is_array($Errors))
		{
			if (sizeof($Errors))
			{
				echo "<div class=\"alert\">";
				echo "<ul><li>";
				echo implode("</li><li>", $Errors);
				echo "</li></ul>";
				echo "</div>";
			}
		}
	}


	function ShowAddressForm($Table, $Var, $Type, $Data = "", $Hide = 0)
	{
		if (!is_array($Data))
			$Data = array();
		
		$Table->AddHead("Address", 1, 3);
		$Table->AddData(field::GetSmartText($Var."[".$Type."_address_l1]", $Data[$Type."_address_l1"], 48, 128, "", 0, $Hide), 5);
		$Table->EndRow();
		$Table->AddData(field::GetSmartText($Var."[".$Type."_address_l2]", $Data[$Type."_address_l2"], 48, 128, "", 0, $Hide), 5);
		$Table->EndRow();
		$Table->AddData(field::GetSmartText($Var."[".$Type."_address_l3]", $Data[$Type."_address_l3"], 48, 128, "", 0, $Hide), 5);
		$Table->EndRow();

		$ID = $Data[$Type."_country_id"];
		$Table->AddHead($this->Localisation->GetSuburb($ID));
		$Table->AddData(field::GetSmartText($Var."[".$Type."_locality]", $Data[$Type."_locality"], 24, 64, "", 0, $Hide));
		$Table->AddHead($this->Localisation->GetState($ID));
		$Table->AddData(field::GetSmartText($Var."[".$Type."_state]", $Data[$Type."_state"], 4, 16, "", 0, $Hide));
		$Table->AddHead($this->Localisation->GetPostcode($ID));
		$Table->AddData(field::GetSmartText($Var."[".$Type."_postal_code]", $Data[$Type."_postal_code"], 5, 12, "", 0, $Hide));
		$Table->EndRow();

		// TODO : change to the account country if not australia
		$Table->AddHead("Country");
		$Show = $Data[$Type."_country_id"] ? $Data[$Type."_country_id"] : 1;
		$Table->AddData(field::GetSmartDrop($Var."[".$Type."_country_id]", $Show, $this->Localisation->GetList(), "", 0, $Hide), 5);
		$Table->EndRow();

		return ($Table);
	}

	function ShowAddressData($Table, $Var, $Type, $Data = "")
	{
		return ($this->ShowAddressForm($Table, $Var, $Type, $Data, 2));
	}

	function ConfirmTransaction()
	{
		//return $this->Quote->IsConfirmed();
	}

	function CommitTransaction()
	{
		// Write Transaction
		//return $this->Quote->CommitTransaction();

	}

	function ShowConfirm()
	{
		// Get Action
		if (defined("MODULES_NAME"))
			$Action = MODULES_NAME.CURRENT_ACTION;
		else
			$Action = CURRENT_ACTION;
		//$Action = $this->URL($this->class_path."|".$Action);
		$Action = $this->URL($Action);

		// Display Confirm Screen
//		$this->Quote->RequestConfirmation($Action);

	}

	// Generic code to load the formdata taking Quote scenario into account
	// After this function $this->FormData will be loaded, either from the Quote or MakeL
	function InitialiseSearch()
	{
		// Load FormData
		if (!$this->GetV("QuoteID"))
		{
			$this->MakeL("FormData");

			if ($this->ValidateInput())
				// Create a quote if one has not been written to the database already
				$this->Quote->CreateQuote($this->ProductCode, $this->FormData);
		}
	}

	// Dummy function to make sure it is overridden in child class
	function ValidateInput()
	{
		$this->Error[] = "Input not validated";
		$this->Errors[] = "Input not validated";
		return false;
	}

	function ValidateABN($Val)
	{
		return($this->Localisation->ValidateNum1(1, $Val));
	}

	function CleanseABN($Val)
	{
		return($this->Localisation->CleanseNum1(1, $Val));
	}

	function DisplayABN($Val)
	{
		return($this->Localisation->DisplayNum1(1, $Val));
	}

	function ValidateACN($Val)
	{
		return($this->Localisation->ValidateNum2(1, $Val));
	}

	function CleanseACN($Val)
	{
		return($this->Localisation->CleanseNum2(1, $Val));
	}

	function DisplayACN($Val)
	{
		return($this->Localisation->DisplayNum2(1, $Val));
	}

	function ExtraHeader()
	{

	}
}

?>
