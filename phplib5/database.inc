<?php
require_once(PHPLIB_DIR."variable.inc");
/*
Copyright: MU Systems Pty Ltd, 2004
*/

//
// database.inc
//

require_once(PHPLIB_DIR."sql.inc");

//if (defined("DATABASE_ENGINE"))
//	$Engine = DATABASE_ENGINE;
//else
//	$Engine = "PEAR";
//
//switch ($Engine)
//{
//	case "MYSQLI":
		require_once(PHPLIB_DIR."database_mysqli.inc");
//		break;
//	case "PDO":
//		require_once(PHPLIB_DIR."database_pdo.inc");
//		break;
//	case "PEAR":
//	default:
//		require_once(PHPLIB_DIR."database_pear.inc");
//		break;
//}

function ParseConnectDB($DB)
{
	$Data = array();
	$Data["Transaction"] = "";
	if (defined("DATABASE_TRANSACTIONS"))
	{
		if (DATABASE_TRANSACTIONS)
		{
			$Parts = explode("mysql://", $DB);
			$Data["Transaction"] = trim($Parts[0]);
		}
	}
	$Parts = explode("://", $DB);
	$Data["Type"] = str_replace($Data["Transaction"], "", $Parts[0]);
	$Parts = explode("|", str_replace(array(":", "@", "/"), "|", $Parts[1]));
	$Data["User"] = $Parts[0];
	$Data["Pass"] = $Parts[1];
	$Data["Host"] = $Parts[2];
	$Data["Name"] = $Parts[3];
	return($Data);
}

class QuickDB
{
	static function LogSQLError($Message, $Continue = 0)
	{

		$Classes = array("DBQuery", "DBInsert");
		$Functions = array("DBRow", "DBGet", "DBResult", "DBResults", "DBList", "DBIndexedBlank", "DBIndexed", "DBLookup", "DBLookupArray", "DBLookupList", "DBAssociative", "DBInsert");

		$Instantiation = "";
		foreach (debug_backtrace() as $Level)
		{
			if (in_array($Level["class"], $Classes))
			{
				if (!strstr($Level["file"], "phplib5.3/database"))
				{
					$Instantiation = $Level["file"]." on line ".$Level["line"];
					break;
				}
			}
			if (in_array($Level["function"], $Functions))
			{
				if (!strstr($Level["file"], "phplib5.3/database"))
				{
					$Instantiation = $Level["file"]." on line ".$Level["line"];
					break;
				}
			}
		}

		$Disk = 0;
		if (defined("DB_ERROR_LOGGING"))
			$Disk = 1;
		if ($Disk)
		{
			$Code = date("z")."-".((date("H") * 60 * 60) + (date("i") * 60) + date("s"));
			echo "<p>An unrecoverable system error has occurred. Please contact support and quote the following reference number: ".$Code."</p>";
			$File  = array();
			$File[] = date("Y-m-d H:i:s");
			$File[] = $Code;
			foreach ($Message as $i => $Line)
			{
				if ($i & 1)
					$File[] = $Line;
				else
					$File[] = strtoupper($Line);
			}
			$File[] = $Instantiation;
			$fp = fopen(DB_ERROR_LOGGING, "a");
			fwrite($fp, "\n".implode("\t", $File));
			fclose($fp);
		}
		else
		{
			if (is_array($Message))
			{
				foreach ($Message as $i => $Line)
				{
					if ($i & 1)
						echo "<p>".$Line."</p>";
					else
						echo "<h2>".$Line."</h2>";
				}
			}
			echo "<p>".$Instantiation."</p>";
		}
		if (!$Continue)
			die;
	}

	static function DBRow($DBName, $SQL_QUERY, $NoClose = 0)
	{
		$RetArray = array();
		$Q = new DBQuery;
		$Q->Run($DBName, $SQL_QUERY);
		if ($Q->Next())
			$Data = $Q->GetArray();
		else
			$Data = array();
		$Q->EndQuery($NoClose);
		return($Data);
	}

	static function DBGet($DBName, $SQL_QUERY, $NoClose = 0)
	{
		$Q = new DBQuery;
		$Q->Run($DBName, $SQL_QUERY);
		if ($Q->Next())
		{
			$a = $Q->GetArray();
			$Return = array_shift($a);
			$Q->EndQuery($NoClose);
			return($Return);
		}
		else
		{
			$Q->EndQuery($NoClose);
			return("");
		}
	}

	static function DBResult($DBName, $SQL_QUERY, $NoClose = 0)
	{
		$RetArray = array();
		$Q = new DBQuery;
		$Q->Run($DBName, $SQL_QUERY);
		for($i=0; $Q->Next(); $i++)
			$RetArray[$i] = $Q->GetArray();
		$Q->EndQuery($NoClose);
		return($RetArray);
	}

	static function DBResults($DBName, $SQL_QUERY, $NoClose = 0)
	{
		return(QuickDB::DBResult($DBName, $SQL_QUERY, $NoClose));
	}

	static function DBList($DBName, $SQL_QUERY, $NoClose = 0)
	{
		$RetArray = array();
		$Q = new DBQuery;
		$Q->Run($DBName, $SQL_QUERY);
		while ($Q->Next())
		{
			$a = $Q->GetArray();
			$RetArray[] = array_shift($a);
		}
		$Q->EndQuery($NoClose);
		return($RetArray);
	}

	static function DBIndexedBlank($DBName, $SQL_QUERY, $NoClose = 0)
	{
		$RetArray = array(""=>"");
		$Q = new DBQuery;
		$Q->Run($DBName, $SQL_QUERY);
		while ($Q->Next())
		{
			$a = $Q->GetArray();
			$Key = array_shift($a);
			$Val = array_shift($a);
			$RetArray[$Key] = $Val;
		}
		$Q->EndQuery($NoClose);
		return($RetArray);
	}

	static function DBIndexed($DBName, $SQL_QUERY, $NoClose = 0)
	{
		$RetArray = array();
		$Q = new DBQuery;
		$Q->Run($DBName, $SQL_QUERY);
		while ($Q->Next())
		{
			$a = $Q->GetArray();
			$Key = array_shift($a);
			$Val = array_shift($a);
			$RetArray[$Key] = $Val;
		}
		$Q->EndQuery($NoClose);
		return($RetArray);
	}
}

// QuickDB::LogSQLError($Message, $Continue);
// return(QuickDB::DBRow($DBName, $SQL_QUERY, $NoClose));
// return(QuickDB::DBGet($DBName, $SQL_QUERY, $NoClose));
// return(QuickDB::DBResult($DBName, $SQL_QUERY, $NoClose));
// return(QuickDB::DBResults($DBName, $SQL_QUERY, $NoClose));
// return(QuickDB::DBList($DBName, $SQL_QUERY, $NoClose));
// return(QuickDB::DBIndexedBlank($DBName, $SQL_QUERY, $NoClose));
// return(QuickDB::DBIndexed($DBName, $SQL_QUERY, $NoClose));

?>
