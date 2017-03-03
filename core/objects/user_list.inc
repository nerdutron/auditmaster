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

class user_list extends secure
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "Users";

		$this->Users = user_maintenance_data::GetUserList();
	}

	function PageContent($Message = "")
	{
		if ($Message)
		{
			$this->ShowMessage($Message);
		}

		$T = new Table();

		$Colspan = 10;

		$T->AddHead("Users", $Colspan);
		$T->EndRow();

		$T->AddHead("Name");
		$T->AddHead("Login");
		$T->AddHead("Email Address");
		$T->AddHead("Phone");
		$T->AddHead("Mobile");
		$T->AddHead("Active");
		$T->AddHead("Auditor");
		$T->AddHead("Last Login");
		$T->AddHead("");
		$T->AddHead("");
		$T->EndRow();

		foreach ($this->Users as $User)
		{
			$T->AddData($User["name"]);
			$T->AddData($User["login"]);
			$T->AddData($User["email"]);
			$T->AddData($User["user_phone"]);
			$T->AddData($User["user_mobile"]);

			if ($User["active"])
				$T->AddData("Yes");
			else
				$T->AddData("No");

			if ($User["auditor"] == "Y")
				$T->AddData("Yes");
			else
				$T->AddData("No");

			if ($User["last_login_time"] != "0000-00-00 00:00:00")
				$T->AddData(date("d/m/Y", strtotime($User["last_login_time"])));
			else
				$T->AddData("Never");

			$T->AddData("<a href='".$this->URL("user_maintenance", array("EUserID" => $User["user_id"]))."'>Edit</a>");
			$T->AddData("<a href='".$this->URL("user_password_reset", array("EUserID" => $User["user_id"]))."'>Reset Password</a>");
			$T->EndRow();
		}

		$T->AddHead("<center><a href='".$this->URL("user_maintenance")."'>Add New</a>", $Colspan);
		$T->EndRow();

		DivBox::PutBoxStart("form_block");
		$T->Put();
		DivBox::PutBoxEnd();

		$this->ShowCurrentUsers();
	}

	function ShowCurrentUsers()
	{
		$T = new Table();

		$Users = user_maintenance_data::GetOnlineUsers();

		$T->AddHead("<center>Current Users</center>", 2);
		$T->EndRow();
		$T->AddHead("Name");
		$T->AddHead("Logged in Since");
		$T->EndRow();

		foreach ($Users as $User)
		{
			$T->AddData($User["name"]);
			$T->AddData(date("H:i d/m/Y", strtotime($User["session_start"])));
			$T->EndRow();
		}

		DivBox::PutBoxStart("current_user_display");
		$T->Put();
		DivBox::PutBoxEnd();
	}
}

?>
