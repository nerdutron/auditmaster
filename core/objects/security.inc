<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: site.inc 4 2006-04-27 05:59:23Z scott                            $ //
//                                                                          //
// Copyright (C) 2006 - Creditworks Australia Pty Ltd                       //
//                                                                          //
// This source code is subject to commerical confidentiality and may not be //
// disclosed to unauthorised thrid parties without written permission from  //
// the management of Creditworks Australia Pty Ltd                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

require_once(PHPLIB_DIR."objects.inc");
require_once(ToIncludes("core")."user_security.inc");

class security extends ObjectSecurityOff
{
	function __construct()
	{
		parent::__construct();
	}

	final function LogAuthentication($Reason, $UserID = 0)
	{
		if ($UserID)
		{
			$Query = "SELECT login FROM users WHERE user_id = ".sql::integer($UserID);
			$Login = trim(DBGet(ACC_DB, $Query));
		}
		else
			$Login = trim($this->GetV("OS_LOGIN"));
		if ($Login)
		{
			if ($Reason == "FAILED")
			{
				$Query = "SELECT user_id FROM users WHERE login = '".sql::text(trim($Login))."' ";
				$UserID = DBGet(ACC_DB, $Query);
				if ($UserID * 1)
				{
					$Reason = "PASSWORD";

					$Query  = "SELECT password ";
					$Query .= "FROM users ";
					$Query .= "WHERE login = '".sql::text($Login)."' ";
					$UPASS = DBGet(ACC_DB, $Query);
					$Key = substr($UPASS, 0, 2);
					$Query  = "SELECT COUNT(*) ";
					$Query .= "FROM users ";
					$Query .= "WHERE login = '".sql::text($Login)."' ";
					$Query .= "AND password = encrypt('".sql::text($this->GetV("OS_PASSWORD"))."', '".$Key."') ";
					if (DBGet(ACC_DB, $Query))
						$Reason = "USER";

				}
				else
					$Reason = "LOGIN";
			}
			$INQ = new DBInsert;
			$INQ->Save("user_name", trim($Login));
			$INQ->Save("user_id", ($UserID * 1));
			$INQ->Save("ip_address", $_SERVER["REMOTE_ADDR"]);
			$INQ->Save("recorded", date("Y-m-d H:i:s"));
			$INQ->Save("reason", $Reason);
			$INQ->Run(ACC_DB, "security_logging", "INSERT");
			$INQ->EndQuery();
		}
	}

	final function InitialiseSecurity()
	{
                $this->User = new user_security;
                if ($this->GetCookie(SESSION_COOKIE))
                        $this->User->LoadFromCookie($this->GetCookie(SESSION_COOKIE));

		if (!$this->ValidUser())
		{
			if (strstr(base64_decode($this->GetV("OS_DEST")), "&NC="))
				$AlternatePage = "EXPIRED|LOGIN";
			else
				$AlternatePage = "";
			$this->SPO = new SecurePublicObject(ACC_DB, $this->Action, "secure", 0, "", "", "", $AlternatePage);
			$UserID = $this->SPO->GetID();
			if ($UserID)
			{
				$this->User->LoginFromCookie($UserID);
				if ($this->ValidUser())
				{
					//$this->User->VerifyIPAddress();
					$this->LogAuthentication("SUCCESS", $UserID);
				}
				else
					$this->LogAuthentication("ACCOUNT", $UserID);
			}
			else
				$this->LogAuthentication("FAILED", $UserID);
		}

//		if ($this->ValidUser())
//			$this->DenyAccessHere();

/*
		if ($this->ValidUser() && $this->User->Get("account_id") != 1)
		{
			$Query .= "SELECT COUNT(*) ";
			$Query .= "FROM scheduled_outages ";
			$Query .= "WHERE status = 'ACTIVE' ";
			$Query .= "AND scheduled < '".date("Y-m-d H:i:s")."' ";
			if (DBGet(ACC_DB, $Query))
			{
				$this->User->Logout();
				$this->Redir($this->URL("secure"));
			}
		}
*/
	}

	final function AllowAccess($Object)
	{
		return ($this->User->AllowAccess($Object));
	}

	final function DenyAccess($Object)
	{
		return ($this->User->DenyAccess($Object));
	}

	final function IsSecurityOn()
	{
		return (get_class($this->User) === "user_security");
	}

	final function UserID()
	{
		if ($this->IsSecurityOn())
			return ($this->User->UserID());
		else
			return (0);
	}

	final function ValidUser()
	{
		if ($this->IsSecurityOn())
			return ($this->User->ValidUser());
		else
			return (0);
	}

	final function IsAdmin($Level)
	{
		if ($this->IsSecurityOn())
			return ($this->User->IsAdmin($Level));
		else
			return (0);
	}

	final function NeedsAdmin($Level = 3)
	{
		if ($this->IsSecurityOn())
			return ($this->User->NeedsAdmin($Level));
		else
			return (0);
	}

	final function ObjectPath($Object = "")
	{
		if ($Object == "")
			$Object = CURRENT_ACTION;
		if (!is_array($this->ObjectPaths))
			$this->ObjectPaths = array();

		if (strstr($Object, "."))
		{
			$Parts = explode(".", $Object);
			$Object = $Parts[1];
		}

		if (!is_array($this->ObjectPaths[$Object]))
		{
			$Class = $Object;
			$Classes = array($Class);
			while ($Class != "security")
			{
				$Class = get_parent_class($Class);
				$Classes[] = $Class;
			}

			$Path = "";
			$this->ObjectPaths[$Object] = array();
			$Classes = array_reverse($Classes);
			foreach ($Classes as $Class)
			{
				$Vars = get_class_vars($Class);
				if ($Vars["class_path"])
					$Path = $Vars["class_path"].".";
					$this->ObjectPaths[$Object][] = $Path.$Class;
			}
		}
		return ($this->ObjectPaths[$Object]);
	}
}

?>
