<?php 
////////////////////////////////////////////////////////////////////////
//                                                                    //
// DB.inc - a replacement for Pear DB in respect of its use in        //
//          database.inc.                                             //
//                                                                    //
// Author: Scott McClintock - January 2010                            //
//                                                                    //
// 20/01/2010 - Draft version - has MySQL built in for convenience    //
//                              sake ... will probably put in its own //
//                              file in due course.                   //
//                                                                    //
////////////////////////////////////////////////////////////////////////

define('DB_FETCHMODE_ASSOC', 1);

class db_error
{
	var $ErrorText;

	function setMessage($String)
	{
		$this->ErrorText = $String;
	}

	function getMessage()
	{
		return($this->ErrorText);
	}

	function getDebugInfo()
	{
		return("Some debug stuff.... gotta work this bit out");
	}
}

class DB
{
	function getDBDetails($DBName, &$DBType, &$DBUser, &$DBPassword, &$DBDatabase)
	{
		// Breakup $DBName into components

		$D1 = explode("//", $DBName);
		$DBType = str_replace("://", "", $D1[0]);
		$D2 = explode(":", $D1[1]);
		$DBUser = $D2[0];
		$D3 = explode("@", $D2[1]);
		$DBPassword = $D3[0];
		$D4 = explode("/", $D3[1]);
		$DBHost = $D4[0];
		$DBDatabase = $D4[1];
	}

	function connect($DBName)
	{

		DB::getDBDetails($DBName, &$DBType, &$DBUser, &$DBPassword, &$DBDatabase);

		if($DBType == "mysql")
		{
			$DB = new DBMysql;

			if(!($DB->connect($DBHost, $DBUser, $DBPassword)))
			{
			
				$Result = new db_error;
				$Result->setMessage("Could Not Connect to Database: ".$DBName);
			}
			else
			{
				$Result = $DB;
			}
		}
		else
		{
			// Invalid Database Type

			$Result = new db_error;
			$Result->setMessage("Invalid Database Type: ".$DBType);
		}
		return($Result);
	}

	function disconnect($Connection)
	{
		$Connection->disconnect();
	}
}

class DBCommon
{
	function connect()
	{
	}

	function query()
	{
	}

	function fetchRow($IndexMode = DB_FETCHMODE_ASSOC, $RowNum = "")
	{
	}

	function insertId()
	{
	}

	function tableInfo()
	{
	}

	function numRows()
	{
	}

	function free()
	{
	}

	function disconnect()
	{
	}
}

class DBMySQL extends DBCommon
{
	var $DBHandle;
	var $DBResult;
	var $DBDatabase;

	function connect($DBHost, $DBUser, $DBPassword, $DBDatabase)
	{
		if(!($this->DBHandle = mysql_connect($HostName, $DB_USER, $DB_PASSWORD)))
		{
			return(FALSE);
		}
		else
		{
			$this->DBDatabase = $DBDatabase;
			return(TRUE);
		}
	}

	function query($Query)
	{
		if(!($Result = mysql_db_query($this->DBDatabase, $Query, $this->DBHandle)))
		{
			$Result = new db_error;
			$Result->setMessage("Query Failed: ".$Query);
			return($Result);
		}
		else
		{
			$this->DBResult = $Result;
			return($this->DBResult);
		}
	}

	function fetchRow($IndexMode = DB_FETCHMODE_ASSOC, $RowNum = "")
	{
	}

	function insertId()
	{
		return(mysql_insert_id($this->DBHandle));
	}

	function tableInfo()
	{
	}

	function numRows()
	{
		return(mysql_num_rows($this->DBResult));
	}

	function free()
	{
		mysql_free_result($this->DBResult);
	}
}
