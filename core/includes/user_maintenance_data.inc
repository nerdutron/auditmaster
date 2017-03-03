<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: user_security.inc 9 2006-12-10 05:17:10Z scott                   $ //
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

class user_maintenance_data
{
	function __construct($UserID = 0)
	{
		if ($UserID)
		{
			$this->UserID = $UserID;

			$this->Load($UserID);
		}

	}

	function Load($UserID)
	{
		$this->Data = DBRow(ACC_DB, "SELECT * FROM users WHERE user_id = $UserID");
	}

	function GetData()
	{
		return $this->Data;
	}

	function Save($Data)
	{
		$INQ = new DBInsert();

		if ($this->UserID)
		{
			$INQ->SaveWhere("user_id", $this->UserID);
			$Function = "UPDATE";
		}
		else
		{
			$INQ->Add("FUNCTION", "created_date", "now()");
			$Function = "INSERT";
		}


		$INQ->Save("name", $Data["name"]);
		$INQ->Save("active", $Data["active"]);
		$INQ->Save("login", $Data["login"]);
		$INQ->Save("email", $Data["email"]);
		$INQ->Save("user_type", $Data["user_type"]);
		$INQ->Save("logins_allowed", $Data["logins_allowed"]);
		$INQ->Save("user_phone", $Data["user_phone"]);
		$INQ->Save("user_fax", $Data["user_fax"]);
		$INQ->Save("user_mobile", $Data["user_mobile"]);
		$INQ->Save("auditor", $Data["auditor"]);

		if ($Data["logins_allowed"])
			$INQ->Save("logins_allowed", $Data["logins_allowed"]);
		else
			$INQ->Save("logins_allowed", 0);

		$INQ->Run(ACC_DB, "users", $Function);

		$this->UserID = $INQ->GetInsertID();

		$INQ->EndQuery();			
	}

	function GetUserList()
	{
		$Q = new DBQuery();

		$Q->Run(ACC_DB, "SELECT * FROM users ORDER BY name");

		$Users = array();

		while ($Q->Next())
		{
			$Users[$Q->GetField("user_id")] = $Q->GetArray();
		}

		$Q->EndQuery();

		return $Users;
	}

	function GetOnlineUsers()
	{
		return DBResult(DB, "SELECT c.user_id, name, session_start FROM users u, user_session_current c WHERE u.user_id = c.user_id");
	}
}


function GetUserTypeDrop()
{
	$Q = new DBQuery();

	$Q->Run(ACC_DB, "SELECT admin_id, description FROM admin_levels");
	
	$Levels = array();

	while ($Q->Next())
	{
		$Levels[$Q->GetField("admin_id")] = $Q->GetField("description");
	}

	$Q->EndQuery();

	return $Levels;
}

?>
