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

class user_security
{
	function __construct($UserID = 0)
	{
		$this->_Reset();

		$this->_AdminLevels = array();
		$Query = "SELECT * FROM admin_levels ORDER BY admin_id";
		$Q = new DBQuery;
		$Q->Run(ACC_DB, $Query);
		while ($Q->Next())
			$this->_AdminLevels[$Q->GetField("admin_id")] = $Q->GetField("description");
		$Q->EndQuery();

		$this->Rules = array();
		$this->Rules["username"] = array();
		$this->Rules["username"]["size_min"] = 3;
		$this->Rules["username"]["size_max"] = 16;
		$this->Rules["username"]["alpha_min"] = 0;
		$this->Rules["username"]["alpha_max"] = 16;
		$this->Rules["username"]["num_min"] = 0;
		$this->Rules["username"]["num_max"] = 16;
		$this->Rules["username"]["invalid_"] = " ?\"\'`~|\\\$%[]{}()+=/<>,.!@#^&*";
		$this->Rules["password"] = array();
		$this->Rules["password"]["size_min"] = 6;
		$this->Rules["password"]["size_max"] = 16;
		$this->Rules["password"]["alpha_min"] = 1;
		$this->Rules["password"]["alpha_max"] = 14;
		$this->Rules["password"]["num_min"] = 1;
		$this->Rules["password"]["num_max"] = 3;
		$this->Rules["password"]["invalid_"] = " ?\"\'`~|\\\$%[]{}()+=/<>,.!@#^&*";

		if ($UserID)
			$this->LoadFromID($UserID);
	}

	function _Reset()
	{
		$this->UserValid = 0;
		$this->_User = array();
		$this->_Permissions = array();
	}

	function _ValidateByRules($Set, $String)
	{
		$LEN = strlen($String);
		$INT = 0;
		$CHR = 0;
		$Chars = str_split($String);
		foreach ($Chars AS $Char)
		{
			if ($Char)
			{
				if (strstr("1234567890", $Char))
					$INT++;
				if (strstr("ABCDEFGHIJKLMNOPQRSTUVWXYZ", strtoupper($Char)))
					$CHR++;
			}
		}
		$Errors = array();
		foreach($this->Rules[$Set] as $Rule => $Data)
		{
			$Parts = explode("_", $Rule);
			switch($Parts[0])
			{
				case "size":
					switch ($Parts[1])
					{
						case "min":
							if ($LEN < $Data)
								$Errors[] = ucfirst($Set)." must be at least ".$Data." characters.";
							break;
						case "max":
							if ($LEN > $Data)
								$Errors[] = ucfirst($Set)." may not be more than ".$Data." characters.";
							break;
					}
					break;
				case "alpha":
					switch ($Parts[1])
					{
						case "min":
							if ($CHR < $Data)
								$Errors[] = ucfirst($Set)." must contain at least ".$Data." alpha characters.";
							break;
						case "max":
							if ($CHR > $Data)
								$Errors[] = ucfirst($Set)." may not contain more than ".$Data." alpha characters.";
							break;
					}
					break;
				case "num":
					switch ($Parts[1])
					{
						case "min":
							if ($INT < $Data)
								$Errors[] = ucfirst($Set)." must contain at least ".$Data." numeric characters.";
							break;
						case "max":
							if ($INT > $Data)
								$Errors[] = ucfirst($Set)." may not contain more than ".$Data." numeric characters.";
							break;
					}
					break;
				case "invalid":
					$Invalid = array();
					$Chars = str_split($Data);
					$Source = array("<", ">", "&", "'", "\"", " ");
					$Encode = array("&lt;", "&gt;", "&amp;", "&apos;", "&quot;", "&lt;space&gt;");
					foreach ($Chars AS $Char)
						if (strstr($String, $Char))
							$Invalid[] = str_replace($Source, $Encode, $Char);
					if (sizeof($Invalid))
						$Errors[] = "The following characters are not allowed: ".implode(" ", $Invalid);
					break;
			}
		}
		return($Errors);
	}

	function LoadFromLogin($User, $Pass)
	{
		$Query  = "";
		$Query .= "SELECT ";
		$Query .= "user_id ";
		$Query .= "FROM users ";
		$Query .= "WHERE active = 1 ";
		$Query .= "AND login = '".addslashes($User)."' ";
		$Query .= "AND password = encrypt('".addslashes($Pass)."', SUBSTRING(password,1,2)) ";
		$UserID = DBGet(ACC_DB, $Query);
		if ($UserID)
			$this->LoadFromID($UserID);
	}

	function _CreateCookie()
	{
		$Cookie = CreateUniqueKey(36);
		$Query = "SELECT COUNT(*) FROM user_session WHERE os_key = '".addslashes($Cookie)."'";
		if (DBGet(ACC_DB, $Query))
			$Cookie = $this->_CreateCookie();
		$this->UserCookie = $Cookie;
		return ($Cookie);
	}

	function LoginFromCookie($UserID)
	{
		$Cookie = $this->_CreateCookie();
		$INQ = new DBInsert;
		$INQ->Save("user_id", $UserID);
		$INQ->Save("os_key", $Cookie);
		$INQ->Save("session_start", date("Y-m-d H:i:s"));
		$INQ->Save("session_expires", date("Y-m-d H:i:s", strtotime(SESSION_EXPIRY)));
		$INQ->Run(ACC_DB, "user_session_current", "INSERT");
		$INQ->EndQuery();

		$INQ->SaveWhere("user_id", $UserID);
		$INQ->Save("os_key", "");
		$INQ->Save("last_login_time", date("Y-m-d H:i:s"));
		$INQ->Run(ACC_DB, "users", "UPDATE");
		$INQ->EndQuery();

		$Path = dirname($_SERVER["REQUEST_URI"]);
		if ($Path != "/")
			$Path .= "/";
		$Parts = explode(":", $_SERVER["HTTP_HOST"]);
		$Host = array_shift($Parts);
		setcookie(SESSION_COOKIE, $Cookie, 0, $Path, $Host);

		$this->UserCookie = $Cookie;
		$this->LoadFromID($UserID);
	}


	function LoadFromCookie($Cookie)
	{
		$this->UserCookie = $Cookie;
		$Query  = "";
		$Query .= "SELECT ";
		$Query .= "user_id, session_data ";
		$Query .= "FROM user_session_current ";
		$Query .= "WHERE os_key = '".addslashes($this->UserCookie)."' ";
		$User = DBRow(ACC_DB, $Query);
		$UserID = $User["user_id"];
		if ($User["session_data"] != NULL)
		$this->UserSession = unserialize($User["session_data"]);
		if ($UserID)
		{
			$INQ = new DBInsert;
			$INQ->Save("session_expires", date("Y-m-d H:i:s", strtotime(SESSION_EXPIRY)));
			$INQ->SaveWhere("user_id", $UserID);
			$INQ->SaveWhere("os_key", $this->UserCookie);
			$INQ->Run(ACC_DB, "user_session_current", "UPDATE");
			$INQ->EndQuery();
			$this->LoadFromID($UserID);
		}
	}

	function LoadFromID($UserID)
	{
		$this->_Reset();
		$Query  = "";
		$Query .= "SELECT ";
		$Query .= "user_id, name, email, user_type, auditor ";
		$Query .= "FROM users ";
		$Query .= "WHERE active = 1 ";
		$Query .= "AND user_id = ".($UserID * 1);
		$Data = DBRow(ACC_DB, $Query);
		if ($Data["user_id"])
		{
			$this->UserValid = 1;

			$DoLoad = 1;
			if (is_array($this->UserSession))
			{
				if (is_array($this->UserSession["_User"]))
				{
					$DoLoad = 0;
				}
			}

			if ($DoLoad)
			{
				$this->_User = $Data;

				if ($this->UserCookie)
				{
					$UserSession = array();
					$UserSession["_LogoID"] = $this->_LogoID;
					$UserSession["_BackGround"] = $this->_BackGround;
					$UserSession["_ForeGround"] = $this->_ForeGround;
					$INQ = new DBInsert;
					$INQ->Save("session_data", serialize($UserSession));
					$INQ->SaveWhere("user_id", $UserID);
					$INQ->SaveWhere("os_key", $this->UserCookie);
					$INQ->Run(ACC_DB, "user_session_current", "UPDATE");
					$INQ->EndQuery();
				}
			}
			else // used the cached data
			{
				$this->_LogoID = $this->UserSession["_LogoID"];
				$this->_BackGround = $this->UserSession["_BackGround"];
				$this->_ForeGround = $this->UserSession["_ForeGround"];
			}

			$this->_SelectPermissions("user", $this->Get("user_id"));
		}
		else // a valid user was not found
			$this->Data = array();
	}

	function GetLogoID()
	{
		return ($this->_LogoID);
	}

	function _FindLogo($ID)
	{
		$this->_LogoID = 0;
		$this->_ForeGround = "";
		$this->_BackGround = "";
		$Query = "SELECT foreground, background FROM account_logos WHERE account_id = ".$ID;
		$Data = DBRow(ACC_DB, $Query);
		if (sizeof($Data))
		{
			$this->_LogoID = $ID;
			$this->_ForeGround = $Data["foreground"];
			$this->_BackGround = $Data["background"];
		}
	}

	function GetBackground()
	{
		return($this->_BackGround);
	}

	function GetForeground()
	{
		return($this->_ForeGround);
	}

	function ValidUser()
	{
		return ($this->UserValid);
	}

	function Get($Field)
	{
		return($this->_User[$Field]);
	}

	function UserID()
	{
		return($this->Get("user_id"));
	}

	function Name()
	{
		return($this->Get("name"));
	}

	function Email()
	{
		return($this->Get("email"));
	}

	function IsAdmin($Level)
	{
		if ($this->Get("user_type") == $Level)
			return (1);
		else
			return (0);
	}

	function NeedsAdmin($Level = 2)
	{
		if ($this->Get("user_type") >= $Level)
			return (1);
		else
			return (0);
	}

	function FindUserName($UserID)
	{
		$Query  = "SELECT name FROM users WHERE user_id = ".($UserID * 1);
		return (DBGet(ACC_DB, $Query));
	}

	function FindAdminLevel($Level)
	{
		return ($this->_AdminLevels[$Level]);
	}

	function GetAccountDivision($DivisionID)
	{ 
		return ($this->_Divisions[$DivisionID]["account_name"]);
	}

	function Logout()
	{
		$INQ = new DBInsert;

		$INQ->SaveWhere("os_key", $this->UserCookie);
		$INQ->Save("os_key", CreateUniqueKey(35));
		$INQ->Save("session_expires", date("Y-m-d H:i:s", strtotime("1 second ago")));
		$INQ->Save("session_data", "");
		$INQ->Run(ACC_DB, "user_session_current", "UPDATE");
		$INQ->EndQuery();

		$INQ->SaveWhere("user_id", $this->UserID());
		$INQ->Save("os_key", CreateUniqueKey(31));
		$INQ->Run(ACC_DB, "users", "UPDATE");
		$INQ->EndQuery();
	}

	function _SelectPermissions($EntityType, $EntityID)
	{
		$Query  = "";
		$Query .= "SELECT secure_object, type ";
		$Query .= "FROM service_objects_permissions ";
		$Query .= "WHERE entity_type = '".$EntityType."' ";
		$Query .= "AND entity_id = ".$EntityID." ";
		$Q = new DBQuery;
		$Q->Run(ACC_DB, $Query);
		while ($Q->Next())
		{
			$Type = $Q->GetField("type");
			$Object = $Q->GetField("secure_object");
			$Current = $this->_Permissions[$Object];
			if ($Current == "")
				$this->_Permissions[$Object] = $Type;
			elseif ($Current != $Type)
				$this->_Permissions[$Object] = "BOTH";
		}
		$Q->EndQuery();
	}

	function AllowAccess($Object)
	{
		if ($this->NeedsAdmin(2))
			return(1);

		$Query = "SELECT COUNT(*) FROM service_objects WHERE secure_object = '".addslashes($Object)."' ";
		if (!DBGet(ACC_DB, $Query))
			return(1);

		$Query  = "";
		$Query .= "SELECT COUNT(*) ";
		$Query .= "FROM service_objects so, services s, service_access sa ";
		$Query .= "WHERE so.secure_object = '".addslashes($Object)."' ";
		$Query .= "AND so.admin_required <= ".$this->Get("user_type")." ";
		$Query .= "AND so.service_id = s.service_id ";
		$Query .= "AND s.publish = 'Y'";
		$Query .= "AND so.service_id = sa.service_id ";
		$Query .= "AND sa.account_id = ".$this->Get("master_account_id")." ";
		return (DBGet(ACC_DB, $Query));
	}

	function PermissionAllow($Object)
	{
		if ($this->_Permissions[$Object] == "ALLOW")
			return(1);
		else
			return(0);
	}

	function PermissionDeny($Object)
	{
		if (in_array($this->_Permissions[$Object], array("DENY","BOTH")))
			return(1);
		else
			return(0);
	}

	function LogPasswordChange($PasswordID, $Source)
	{
		$INQ = new DBInsert;
		$INQ->Save("password_id", $PasswordID);
		$INQ->Save("account_id", $this->Get("account_id"));
		$INQ->Save("user_id", $this->Get("user_id"));
		$INQ->Save("changed", date("Y-m-d H:i:s"));
		$INQ->Save("source", $Source);
		$INQ->Run(ACC_DB, "password_log", "INSERT");
		$INQ->EndQuery();
	}

	function PasswordSave($Old, $New1, $New2)
	{
		$Errors = array();

		$Query = " FROM users WHERE user_id = ".$this->UserID()." ";
		$DBPass = DBGet(ACC_DB, "SELECT password ".$Query);
		$Key = substr($DBPass, 0, 2);
		$Query .= "AND password = encrypt('".addslashes($Old)."', '".$Key."') ";
		if ($this->UserID() != DBGet(ACC_DB, "SELECT user_id ".$Query))
			$Errors[] = "Old password is not correct";

		if ($New1 != $New2)
			$Errors[] = "New password does not match.";

		if (sizeof($Errors) == 0 && !$New1)
			$Errors[] = "A new password was not supplied.";

		if (sizeof($Errors) == 0)
			$Errors = $this->_ValidateByRules("password", $New1);

		if (sizeof($Errors) == 0)
		{
			//$this->LogPasswordChange($this->UserID(), "USER");
			$INQ = new DBInsert;
			$INQ->SaveWhere("user_id", $this->UserID());
			$INQ->Add("FUNCTION", "password", "encrypt('".$New1."')");
			$INQ->Run(ACC_DB, "users", "UPDATE");
			$INQ->EndQuery();
		}
		return($Errors);
	}

	function UserPasswordCreate($UserID, $Notify = 1)
	{
		$Query = "SELECT email, name, login FROM users WHERE user_id = ".$UserID;
		$Data = DBRow(ACC_DB, $Query);

		//$this->LogPasswordChange($UserID, "ADMIN");
		$Password = strtolower(CreateUniqueKey(8));
		$INQ = new DBInsert;
		$INQ->SaveWhere("user_id", $UserID);
		$INQ->Add("FUNCTION", "password", "encrypt('".$Password."')");
		$INQ->Run(ACC_DB, "users", "UPDATE");
		$INQ->EndQuery();

		$Body  = "";
		$Body .= "New Password created for ".$Data["name"].":\n\n";
		$Body .= "Login Name: ".$Data["login"]."\n";
		$Body .= "Password: ".$Password."\n\n";

		if ($Notify)
		{
			mail($Data["email"], "New ".COMPANY_NAME." Login Password", $Body);

			if ($this->Get("email") != $Data["email"])
				mail($this->Get("email"), "New ".COMPANY_NAME." Login Password", $Body);
		}
	}

	function _EditCheck($UserID)
	{
		$Check = 0;
		switch ($this->Get("user_type"))
		{
			case 0: // Unprivileged
			case 1: // Privileged
				$Check = 0;
				break;
			case 2: // Division Admin
				if (!$UserID)
					$Check = 1;
				else
				{
					$Query  = "";
					$Query .= "SELECT COUNT(*) ";
					$Query .= "FROM users ";
					$Query .= "WHERE user_id = ".($UserID * 1)." ";
					$Query .= "AND account_id = ".$this->AccountID();
					$Check = DBGet(ACC_DB, $Query);
				}
				break;
			case 3: // Account Admin
				if (!$UserID)
					$Check = 1;
				else
				{
					$Query  = "";
					$Query .= "SELECT COUNT(*) ";
					$Query .= "FROM users";
					$Query .= "WHERE user_id = ".($UserID * 1);
					$Check = DBGet(ACC_DB, $Query);
				}
				break;
			case 4: // Staff
			case 5: // Admin
				$Check = 1;
				break;
		}
		return ($Check);
	}

	function UserEdit($UserID, $AccountID, $Form = "")
	{
		if (!$this->_EditCheck($UserID))
			return (0);

		$Query  = "";
		$Query .= "SELECT user_id, account_id, active, name, login, email, user_type, unrestricted_ip ";
		$Query .= "FROM users ";
		$Query .= "WHERE user_id = ".($UserID * 1)." ";
		$User = DBRow(ACC_DB, $Query);
		if (!is_array($User))
			$User = array();

		if (is_array($Form))
			$User = array_merge($User, $Form);

		if ($User["account_id"])
			$AccountID = $User["account_id"];
		if (!$AccountID)
			$AccountID = $this->AccountID();

		$T = new CWTable;

		$T->AddHead("ID");
		$T->AddData(($User["user_id"] ? $User["user_id"] : "New User"));
		$T->EndRow();

		$T->AddHead("Account");
		$T->AddData($this->FindAccountName($this->MasterID()));
		$T->EndRow();

		$Divisions = $this->ListDivisions();
		$T->AddHead("Division");
		if ($this->NeedsAdmin(3))
			$T->AddData(field::GetDrop("FormData[account_id]", $AccountID, $Divisions));
		else
			$T->AddData(field::GetHidden("FormData[account_id]", $AccountID).$Divisions[$AccountID]);
		$T->EndRow();

		$T->AddHead("Status");
		$T->AddData(field::GetDrop("FormData[active]", $User["active"], array(1=>"Active",0=>"Disabled")));
		$T->EndRow();

		$T->AddHead("Full Name");
		$T->AddData(field::GetText("FormData[name]", $User["name"], 64, 128));
		$T->EndRow();

		$T->AddHead("Login");
		$T->AddData(field::GetText("FormData[login]", $User["login"], 16, 16));
		$T->EndRow();

		$T->AddHead("Email");
		$T->AddData(field::GetText("FormData[email]", $User["email"], 64, 64));
		$T->EndRow();

		$Query  = "";
		$Query .= "SELECT admin_id, description ";
		$Query .= "FROM admin_levels ";
		$Query .= "WHERE admin_id <= ".$this->Get("user_type")." ";
		if ($AccountID > 1)
			$Query .= "AND admin_id < 4 ";
		$Query .= "ORDER BY admin_id";
		$Admins = field::GetListQuery(ACC_DB, $Query);
		$T->AddHead("Admin");
		$T->AddData(field::GetDrop("FormData[user_type]", $User["user_type"], $Admins));
		$T->EndRow();

		if (!$UserID)
		{
			$T->AddHead("Notice");
			$T->AddData(field::GetCheck("FormData[user_notice]", $User["user_notice"], 1));
		}

		if ($this->NeedsAdmin(3))
		{
			$T->AddHead("User Access");
			$T->AddData(field::GetDrop("FormData[unrestricted_ip]", $User["unrestricted_ip"], $this->AllowIP));
		}

		$T->EndRow();

		field::PutFormStart(site::URL(str_replace("_edit", "_save", CURRENT_ACTION)));
		field::PutHidden("EditUserID", $User["user_id"]);
		field::PutHidden("AccountID", $AccountID);
		field::PutHidden("FormData[user_id]", $User["user_id"]);
		field::PutHidden("FormData[account_id]", $AccountID);
		$T->Put();
		field::PutFormSubmit("Save");
		field::PutFormEnd();
	}

	function UserSave($FormData)
	{
		$Errors = array();

		$Errors = $this->_ValidateByRules("username", $FormData["login"]);

		if (trim($FormData["name"]) == "")
			$Errors[] = "A full name is required.";
		if (trim($FormData["login"]) == "")
			$Errors[] = "A login name is required.";
		if (trim($FormData["email"]) == "")
			$Errors[] = "An email address is required.";
		else
		{
			$Fail = 0;
			$FormData["email"] = trim($FormData["email"]);
			if (substr_count($FormData["email"], " "))
				$Fail = 1;
			if (!substr_count($FormData["email"], "."))
				$Fail = 1;
			if (substr_count($FormData["email"], "@") != 1)
				$Fail = 1;
			if (substr_count($FormData["email"], "@", 1, (strlen($FormData["email"]) - 2)) != 1)
				$Fail = 1;
			if ($Fail)
				$Errors[] = "The supplied email address does not appear to be valid.";
		}

		$Query = "SELECT COUNT(*) FROM users WHERE login = '".addslashes($FormData["login"])."' ";
		if ($FormData["user_id"])
			$Query .= "AND user_id <> ".($FormData["user_id"] * 1);
		if (DBGet(ACC_DB, $Query))
			$Errors[] = "The login name is already in use.";

		if (sizeof($Errors))
			return ($Errors);

		$INQ = new DBInsert;
		$INQ->Save("account_id", $FormData["account_id"]);
		$INQ->Save("active", $FormData["active"]);
		$INQ->Save("user_type", $FormData["user_type"]);
		$INQ->Save("name", $FormData["name"]);
		$INQ->Save("login", $FormData["login"]);
		$INQ->Save("email", $FormData["email"]);

		if ($this->NeedsAdmin(3))
			$INQ->Save("unrestricted_ip", $FormData["unrestricted_ip"]);

		if ($FormData["user_id"])
		{
			$INQ->SaveWhere("user_id", $FormData["user_id"]);
			$INQ->Run(ACC_DB, "users", "UPDATE");
		}
		else
		{
			$INQ->Save("account_id", $FormData["account_id"]);
			$INQ->Save("password", strtolower(CreateUniqueKey(8)));
			$INQ->Run(ACC_DB, "users", "INSERT");
			$UserID = $INQ->GetInsertID();

			$this->UserPasswordCreate($UserID, $FormData["user_notice"]);
		}
		$INQ->EndQuery();

		return(0);
	}

	function PasswordChangeLog($PassID)
	{
		$Query  = "";
		$Query .= "SELECT ";
		$Query .= "sp.user_id, sp.account_id, sp.name, sp.login, sp.email, sp.user_type ";
		$Query .= "FROM users sp, account a ";
		$Query .= "WHERE a.master_account_id = ".$this->MasterID()." ";
		$Query .= "AND a.account_id = sp.account_id ";
		$Query .= "AND sp.user_id = ".($PassID * 1)." ";
		$User = DBRow(ACC_DB, $Query);

		if (is_array($User))
		{
			echo "<h1>Password Change Log</h1>";

			$T = new CWTable;
			$T->AddHead("Account");
			$T->AddData($this->FindAccountName($User["account_id"]));
			$T->AddHead("User");
			$T->AddData($User["name"]);
			$T->AddHead("Login");
			$T->AddData($User["login"]);
			$T->Put();

			$Query = "SELECT * FROM password_log WHERE password_id = ".$User["user_id"]." ORDER BY changed DESC";
			$Q = new DBQuery();
			$Q->Run(ACC_DB, $Query);

			$T = new CWTable;
			while ($Q->Next())
			{
				$Log  = "";
				$Log .= "Changed <b>";
				$Log .= date("d/m/Y", CreateTimeStamp($Q->GetField("changed")));
				$Log .= "</b> at <b>";
				$Log .= date("h:ia", CreateTimeStamp($Q->GetField("changed")));
				$Log .= "</b> by ";
				switch ($Q->GetField("source"))
				{
					case "ADMIN":
						$Log .= "Administrator ";
						break;
					case "USER":
						$Log .= "User ";
						break;
				}
				$Log .= "<i>";
				$Log .= $this->FindUserName($Q->GetField("user_id"));
				$Log .= "</i> from <i>";
				$Log .= $this->FindAccountName($Q->GetField("account_id"));
				$Log .= "</i>";
				$T->AddData($Log);
				$T->EndRow();
			}
			$Q->EndQuery();
			$T->Put();
		}
		else
		{
			echo "<script>";
			echo "window.close();";
			echo "</script>";
		}


/*
mysql> desc password_log;
+------------+-------------+------+-----+---------------------+----------------+
| Field      | Type        | Null | Key | Default             | Extra          |
+------------+-------------+------+-----+---------------------+----------------+
| change_id  | int(11)     | NO   | PRI | NULL                | auto_increment |
| account_id | int(11)     | NO   |     | 0                   |                |
| user_id    | int(11)     | NO   |     | 0                   |                |
| changed    | datetime    | NO   |     | 0000-00-00 00:00:00 |                |
| source     | varchar(16) | NO   |     | NULL                |                |
+------------+-------------+------+-----+---------------------+----------------+
5 rows in set (0.00 sec)
*/


	}

	function UserList($AccountID, $Type) // STAFF | ACCOUNT | DIVISION | FULL | SEARCH
	{
		if (!$this->ValidUser() || !$this->NeedsAdmin(2))
			return (0);
		$Q = new DBQuery;

		$T = new CWTable;
		$T->AlternateMouseOver();

		$AccountDetails = $this->GetAccountDetails($AccountID);

		$T->AddData("<h2>Users for Account: ".$this->AccountName()."</h2>", 11, 1, "", "layout");
		$T->EndRow();

		if (in_array($Type, array("ACCOUNT")) && $AccountDetails["parent_account_id"])
		{
			$ID = $AccountDetails["parent_account_id"];
			$Name = $this->FindAccountName($ID);
			$T->AddHead("Parent Account", 11);
			$T->EndRow();
			$T->AddData("&nbsp;");
			$T->AddData($Name, 4);
			$T->AddData("Active");
			$T->AddData("&nbsp;");
			$Query = "SELECT COUNT(*) FROM users WHERE account_id = ".$ID;
			$T->AddData("[<a href=\"".site::URL(CURRENT_ACTION, array("AccountID"=>$ID))."\">view ".DBGet(ACC_DB, $Query)." staff</a>]", 4);
			$T->EndRow();
		}

		$T->AddHead("ID");
		$T->AddHead("Account");
		$T->AddHead("Full Name");
		$T->AddHead("Email");
		$T->AddHead("Login");
		$T->AddHead("Status");
		$T->AddHead("Admin");
		$T->AddHead("Control", 4);
		$T->EndRow();

		$Query  = "";
		$Query .= "SELECT user_id, account_id, active, name, login, email, user_type, unrestricted_ip ";
		$Query .= "FROM users ";
		$Query .= "WHERE account_id = ".$AccountID." ";
		$Query .= "ORDER BY name ";
		$Q->Run(ACC_DB, $Query);
		while ($Q->Next())
		{
			$ID = $Q->GetField("user_id");
			$T->AddData($ID);
			$T->AddData($this->FindAccountName($Q->GetField("account_id")));
			$T->AddData($Q->GetField("name"));
			$T->AddData($Q->GetField("email"));
			$T->AddData($Q->GetField("login"));
			if ($Q->GetField("active"))
				$T->AddData("Active and ".(($Q->GetField("unrestricted_ip") == "Y") ? "Unr" : "R")."estricted");
			else
				$T->AddData("Inactive");
			$T->AddData($this->FindAdminLevel($Q->GetField("user_type")));
			$T->AddData("<a href=\"".site::URL(CURRENT_ACTION."_edit", array("EditUserID"=>$ID,"AccountID"=>$AccountID))."\">edit</a>");
			if ($Q->GetField("active"))
			{
				$T->AddData(site::ActionLink(site::URL(CURRENT_ACTION."_reset", array("EditUserID"=>$ID,"AccountID"=>$AccountID)), "Are you sure you want to reset the password for ".$Q->GetField("name")."?", "reset password"));
				$Link  = "";
				$Link .= "<a href=\"javascript:PopBox('";
				$Link .= site::URL(CURRENT_ACTION."_change_log", array("PassID"=>$ID));
				$Link .= "')\">";
				$Link .= "<img ".site::Icon("lock")." />";
				$Link .= "</a>";
				$T->AddData($Link);

				$Query = "SELECT COUNT(*) FROM user_session_current WHERE user_id = ".$ID;
				if (DBGet(ACC_DB, $Query))
					$T->AddData("<img ".site::Icon("user_in")." alt=\"User is currenty logged in\"/>");
				else
					$T->AddData("<img ".site::Icon("user_out")." alt=\"User is currenty logged out\"/>");
			}
			else
				$T->AddData("--", 3);
			$T->EndRow();
		}
		$T->AddData("[<a href=\"".site::URL(CURRENT_ACTION."_edit", array("AccountID"=>$AccountID))."\">add new</a>]", 11);
		$T->EndRow();
		$Q->EndQuery();

		if (in_array($Type, array("ACCOUNT", "DIVISION")))
		{
			$Query  = "";
			$Query .= "SELECT ";
			$Query .= "account_id, account_name, account_status, unrestricted_ip ";
			$Query .= "FROM account ";
			$Query .= "WHERE master_account_id = ".$this->MasterID()." ";
			$Query .= "AND parent_account_id = ".$AccountID." ";
			$Query .= "ORDER BY account_name";
			$Q->Run(ACC_DB, $Query);
			if ($Q->NumRows())
			{
				$T->AddHead("Child Accounts", 11);
				$T->EndRow();
				while ($Q->Next())
				{
					$T->AddData("&nbsp;");
					$T->AddData($Q->GetField("account_name"), 4);
					if ($Q->GetField("account_status") == "ACTIVE")
						$T->AddData("Active and ".(($Q->GetField("unrestricted_ip") == "Y") ? "Unr" : "R")."estricted");
					else
						$T->AddData("Inactive");
					$T->AddData(ucfirst(strtolower($Q->GetField("account_status"))));
					$T->AddData("&nbsp;");
					$Query = "SELECT COUNT(*) FROM users WHERE account_id = ".$Q->GetField("account_id");
					$T->AddData("[<a href=\"".site::URL(CURRENT_ACTION, array("AccountID"=>$Q->GetField("account_id")))."\">view ".DBGet(ACC_DB, $Query)." staff</a>]", 3);
					$T->EndRow();
				}
			}
			$Q->EndQuery();
		}

		$T->Put();
	}

	function UserListByStaff()
	{
		if (!$this->ValidUser() || !$this->NeedsAdmin(4))
			return (0);

		if ($this->Get("effective_account_id"))
			$this->AccountUnset();

		$this->UserList($this->Get("account_id"), "STAFF");
	}

	function UserListByAccount($AccountID = 0)
	{
		$AccountID = $AccountID * 1;
		if (!$this->ValidUser())
			return (0);
		if (!$this->NeedsAdmin(2))
			return (0);

		if ($this->IsAdmin(2))
		{
			$AccountID = $this->AccountID();
			$this->UserList($AccountID, "DIVISION");
		}

		if ($this->NeedsAdmin(3))
		{
			if (!$AccountID)
				$AccountID = $this->AccountID();
			$this->UserList($AccountID, "ACCOUNT");
		}
	}

	function IsAuditor()
	{
		if ($this->Get("auditor") == "Y")
			return (1);
		else
			return (0);
	}
}

?>
