<?php
/*
Copyright: MU Systems Pty Ltd, 2009

DROP TABLE IF EXISTS `csrf_master`;
CREATE TABLE `csrf_master` (
  `csrf_cookie` varchar(32) NOT NULL,
  `csrf_token` varchar(8) NOT NULL,
  `csrf_expiry` datetime NOT NULL,
  PRIMARY KEY (`csrf_cookie`,`csrf_token`),
  KEY `csrf_token` (`csrf_token`)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

*/

require_once(PHPLIB_DIR."database.inc");
require_once(PHPLIB_DIR."tools.inc");

class csrf_clean
{
	public function __construct()
	{
		if (defined("CSRF_CONFIGURATION"))
		{
			if(is_file(CSRF_CONFIGURATION))
			{
				if (defined("CSRF_DB"))
					$DB = constant("CSRF_DB");
				if (!trim($DB))
					$DB = WEB_DB;
				$Query = "DELETE FROM csrf_master WHERE csrf_expiry < '".date("Y-m-d H:i:s", strtotime("8 hours ago"))."'";
				DB_DoQuery($DB, $Query);
			}
		}
	}
}

class csrf
{
	// PUBLIC INTERFACE

	public function __construct($SuppliedToken = "")
	{
		$this->SuppliedToken = $SuppliedToken;
		$this->_EnableCSRF = 0;
		$this->_CSRFStatus = 0;
		$this->_CSRFExpired = 0;
		$this->_CSRFDifferent = 0;
		$this->_CSRFApproved = 0;
		if (defined("CSRF_CONFIGURATION"))
		{
			if(is_file(CSRF_CONFIGURATION))
			{
				$this->_EnableCSRF = 1;
				$this->Config = array();
				$this->Config["DB"] = $this->_GetConstantValue("CSRF_DB", WEB_DB);
				$this->Config["Cookie"] = $this->_GetConstantValue("CSRF_COOKIE_LENGTH", 32);
				$this->Config["Token"] = $this->_GetConstantValue("CSRF_TOKEN_LENGTH", 8);
				$this->Config["Expiry"] = strtotime($this->_GetConstantValue("CSRF_EXPIRY_STRING", "+4 hours"));
				$Parts = explode(":", $_SERVER["HTTP_HOST"]);
				$this->Config["Host"] = array_shift($Parts);
				$this->Config["Path"] = dirname($_SERVER["REQUEST_URI"]);
				$this->Config["Name"] = $this->_GetConstantValue("CSRF_COOKIE_NAME", "CSRF_COOKIE");
				if ($this->Config["Path"] != "/")
					$this->Config["Path"] .= "/"; 
				$this->Config["Objects"] = $this->_GetObjects();
			}
		}

		if ($this->_EnableCSRF && $_SERVER["HTTP_HOST"])
		{
			$this->_Initialise();
			$this->_Check();
// echo "<p>Supplied Token: ".$this->SuppliedToken."]";
// echo "<p>Enable: ".$this->_EnableCSRF."]";
// echo "<p>Status: ".$this->_CSRFStatus."]";
// echo "<p>Expired: ".$this->_CSRFExpired."]";
// echo "<p>Different: ".$this->_CSRFDifferent."]";
// echo "<p>Approved: ".$this->_CSRFApproved."]";
		}
	}

	public function Token()
	{
		return($this->_CurrentToken);
	}

	public function Approved()
	{
		return($this->_CSRFApproved);
	}

	public function Failed()
	{
		if ($this->_CSRFApproved)
			return(0);
		else
			return(1);
	}

	public function Expired()
	{
		return($this->_CSRFExpired);
	}

	public function Different()
	{
		return($this->_CSRFDifferent);
	}

	public function DirectoryAnalysis($ObjectDirectory, $ObjectPrepend = "")
	{
		$FoundObjects = array();
		$FullPath = $ObjectDirectory."/objects/";
		echo "\n\n\n";
		if (is_dir($FullPath))
		{
			$d = dir($FullPath);
			while (false !== ($File = $d->read()))
			{
				$Parts = explode(".", $File);
				$Ext = array_pop($Parts);
				$File = implode(".", $Parts);
				if (($Ext == "inc") && trim($File))
				{
					if ($ObjectPrepend)
						$FoundObjects[] = $ObjectPrepend.".".$File;
					else
						$FoundObjects[] = $File;
				}
			}
			$d->close();
			sort($FoundObjects);

			$ConfigObjects = array();
			foreach($this->Config["Objects"] as $Object)
			{
				$Parts = explode(".", $Object);
				if (sizeof($Parts) == 2)
				{
					if ($ObjectPrepend == $Parts[0])
						$ConfigObjects[] = $Object;
				}
				else
				{
					if ($ObjectPrepend == "")
						$ConfigObjects[] = $Object;
				}
			}
			sort($ConfigObjects);

			echo "\n\nUNPROTECTED OBJECTS (these are present in the config file)\n\n";
			echo implode("\n", array_intersect($ConfigObjects, $FoundObjects));
			echo "\n\nPROTECTED OBJECTS (these are NOT present in the config file)\n\n";
			echo implode("\n", array_diff($FoundObjects, $ConfigObjects));
			echo "\n\nMISSING FILES (in the config file but missing from the file system)\n\n";
			echo implode("\n", array_diff($ConfigObjects, $FoundObjects));
		}
		else
			echo "Invalid Directory Path Supplied";
		echo "\n\n\n";
	}

	// PRIVATE functions

	private function _GetConstantValue($Name, $Default)
	{
		if (defined($Name))
		{
			$Value = constant($Name);
			if (trim($Value))
				return($Value);
		}
		return($Default);
	}

	private function _GetObjects()
	{
		$Objects = array();
		$Lines = file(CSRF_CONFIGURATION);
		foreach ($Lines as $Line)
		{
			$Line = trim($Line);
			if (trim($Line))
			{
				if(!strstr($Line, "#"))
					$Objects[] = $Line;
			}
		}
		return($Objects);
	}

	private function _CreateUnique($Type)
	{
		switch($Type)
		{
			case "cookie":
				$Length = $this->Config["Cookie"];
				break;
			case "token":
				$Length = $this->Config["Token"];
				break;
		}
		if (!$Length)
			exit("<h1>FATAL ERROR: invalid token or cookie length supplied.</h1>");
		$KEY = CreateUniqueKey($Length);
		$Query = "SELECT COUNT(*) FROM csrf_master WHERE csrf_".$Type." = '".$KEY."'";
		if (DBGet($this->Config["DB"], $Query))
			$KEY = $this->_CreateUnique($Type);
		return ($KEY);
	}

	private function _Delete($Cookie)
	{
			$Query = "DELETE FROM csrf_master WHERE csrf_cookie = '".sql::text($_COOKIE[$this->Config["Name"]])."'";
			DB_DoQuery(DB, $Query);
			$this->_UnsetCookie();
	}

	private function _CreateCSRF()
	{
		$Cookie = $this->_CreateUnique("cookie");
		$Token  = $this->_CreateUnique("token");
		$this->_SetCookie($Cookie, $this->Config["Expiry"]);
		$INQ = new DBInsert;
		$INQ->Save("csrf_cookie", $Cookie);
		$INQ->Save("csrf_token",  $Token);
		$INQ->Save("csrf_expiry", date("Y-m-d H:i:s", $this->Config["Expiry"]));
		$INQ->Run($this->Config["DB"], "csrf_master", "INSERT");
		$INQ->EndQuery();
		$this->_CurrentToken = $Token;
	}

	private function _Check()
	{
// echo "<p>Status: ".$this->_CSRFStatus."]";
// echo "<p>Expired: ".$this->_CSRFExpired."]";
// echo "<p>Different: ".$this->_CSRFDifferent."]";
// echo "<p>Approved: ".$this->_CSRFApproved."]";
// echo "<hr />";
		$this->_CSRFApproved = 1;
		if ($this->_EnableCSRF)
			if (!in_array($_REQUEST["Action"], $this->Config["Objects"]))
				$this->_CSRFApproved = $this->_CSRFStatus;
// echo "<p>Status: ".$this->_CSRFStatus."]";
// echo "<p>Expired: ".$this->_CSRFExpired."]";
// echo "<p>Different: ".$this->_CSRFDifferent."]";
// echo "<p>Approved: ".$this->_CSRFApproved."]";
// echo "<hr />";
	}

	private function _Initialise()
	{
		if ($_COOKIE[$this->Config["Name"]])
		{
			$Query = "SELECT csrf_token FROM csrf_master WHERE csrf_cookie = '".sql::text($_COOKIE[$this->Config["Name"]])."'";
			$StoredToken = DBGet($this->Config["DB"], $Query);
// echo "<p>Stored Token: ".$StoredToken."]";
			if ($StoredToken !== $this->SuppliedToken)
				$this->_CSRFDifferent = 1;
			else
			{
				$Query = "SELECT csrf_expiry FROM csrf_master WHERE csrf_token = '".sql::text($this->SuppliedToken)."'";
				$StoredExpiry = DBRow($this->Config["DB"], $Query);
				if ($StoredExpiry < date("Y-m-d H:i:s"))
					$this->_CSRFExpired = 1;
			}
			if (($this->_CSRFExpired + $this->_CSRFDifferent) == 0)
				$this->_CSRFStatus = 1;
			if ($this->_CSRFStatus)
			{
				$INQ = new DBInsert;
				$INQ->SaveWhere("csrf_cookie", $Cookie);
				$INQ->SaveWhere("csrf_token",  $Token);
				$INQ->Save("csrf_expiry", date("Y-m-d H:i:s", $this->Config["Expiry"]));
				$INQ->Run($this->Config["DB"], "csrf_master", "UPDATE");
				$INQ->EndQuery();
				$this->_CurrentToken = $this->SuppliedToken;
			}
			else
			{
			//	$this->_CreateCSRF();
				$this->_CurrentToken = $StoredToken;
			}
		}
		else
		{
			$this->_CreateCSRF();
		}
	}

	private function _SetCookie($Cookie, $Expires, $Path)
	{
		setcookie($this->Config["Name"], $Cookie, $Expires, $this->Config["Path"], $this->Config["Host"], ($_SERVER["HTTPS"] == "on"), true);
	}

	private function _UnsetCookie()
	{
		$Path = explode("/", substr($this->Config["Path"], 1, strlen($this->Config["Path"]) - 2));
		while (sizeof($Path))
		{
			$Delete = "/".implode("/", $Path)."/";
			if ($Delete == "//")
				$Delete = "/";
			setcookie($this->Config["Name"], "", (time() - 3600), $Delete, $this->Config["Host"], ($_SERVER["HTTPS"] == "on"), true);
			array_pop($Path);
		}
	}
}

?>
