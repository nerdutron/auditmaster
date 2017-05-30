<?php
require_once(PHPLIB_DIR."variable.inc");
/*
Copyright: MU Systems Pty Ltd, 2004
*/

/**********************************************************************************/
/*                                                                                */
/*  CONFIGURATION:                                                                */
/*                                                                                */
/*  defined("MYSQLI_DISABLE_CACHE", 1);                                           */
/*  This disables the connection cache for port database connections.             */
/*                                                                                */
/*  DEBUG ONLY:                                                                   */
/*                                                                                */
/*  When the following is used, a detected issue will result in script            */
/*  termination and therefore should never be used in production.                 */
/*                                                                                */
/*  defined("MYSQLI_CONNECT_LIMIT", 10);                                          */
/*  Sets a maximum connect limit to 10. Once this number of open connections is   */
/*  reached, the script terminates with an error.                                 */
/*                                                                                */
/*  defined("MYSQLI_CONNECT_LIMIT", "DEBUG");                                     */
/*  Looks for multiple attempts to open a connection from the same line of code.  */
/*  This is useful for finding code that does not close their connection.         */
/*                                                                                */
/*  defined("MYSQLI_CONNECT_EMAIL", "email@address.com.au");                      */
/*  If there is a connect limit error send it to this email address (rather than  */
/*  display onscreen).                                                            */
/*                                                                                */
/**********************************************************************************/

//
// database.inc
//
// Database access classes wrapped around PEAR DB
//
// Authors: Scott McClintock
//          Peter Hodge
// 
// Copyright 2003 MU Systems Pty Ltd
//
//

//require_once(PHPLIB_DIR."sql.inc");
// $GLOBALS["MYSQLI_PERSISTENT_QUERIES"] = array();
$GLOBALS["MYSQLI_PERSISTENT_LOCATION"] = array();
$GLOBALS["MYSQLI_PERSISTENT_STATUSES"] = array();
$GLOBALS["MYSQLI_PERSISTENT_COMMENCE"] = array();
$GLOBALS["MYSQLI_PERSISTENT_CONNECTS"] = array();

$GLOBALS["MYSQLI_TRANSACTION_CONNECT"] = array();
$GLOBALS["MYSQLI_TRANSACTION_SESSION"] = array();
$GLOBALS["MYSQLI_TRANSACTION_RUNNING"] = array();
$GLOBALS["MYSQLI_TRANSACTION_DB_INFO"] = array();

define("MYSQLI_CONNECTION_TIMEOUT", (60 * 15)); // 15 minutes

function LogSQLError($Message, $Continue = 0)
{
	QuickDB::LogSQLError($Message, $Continue);
}

//////////////////////////////////////////////////////////////////////////
// DB_DoQuery functions - actual queries are centralised
//////////////////////////////////////////////////////////////////////////

function TransactionBegin($DB, $TransactionCode)
{
        if (defined("DATABASE_TRANSACTIONS"))
        {
                if (DATABASE_TRANSACTIONS)
		{
			$DBInfo = ParseConnectDB($DB);
			if (!trim($TransactionCode))
				LogSQLError(array("Fatal MySQLi Transaction Error", "Begin Transaction missing code."));
			$PersistKey = sha1($DBInfo["Host"].$DBInfo["User"].$DBInfo["Pass"].$DBInfo["Name"]);
			if (isset($GLOBALS["MYSQLI_TRANSACTION_CONNECT"]) && isset($GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]) && gettype($GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]) && get_class($GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]))
				LogSQLError(array("Fatal MySQLi Transaction Error", "Begin Transaction '".$TransactionCode."' already in use."));
			$Illegal = array();
			$Allowed = str_split("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
			foreach (str_split($TransactionCode) as $Chr)
			{
				if (!in_array($Chr, $Allowed))
					$Illegal[] = $Chr;
			}
			if (sizeof($Illegal))
				LogSQLError(array("Fatal MySQLi Transaction Error", "Illegal Characters '".implode("', '", $Illegal)."' in '".$TransactionCode."'"));
			$GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode] = new mysqli($DBInfo["Host"], $DBInfo["User"], $DBInfo["Pass"], $DBInfo["Name"]);
			$GLOBALS["MYSQLI_TRANSACTION_SESSION"][$TransactionCode] = $PersistKey;
			$GLOBALS["MYSQLI_TRANSACTION_RUNNING"][$TransactionCode] = 0;
			$GLOBALS["MYSQLI_TRANSACTION_DB_INFO"][$TransactionCode] = $DBInfo;
//	$GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->begin_transaction();
			$Result = $GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->query("BEGIN WORK");
			if ($Result === false || $GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->error)
			{
				$QueryError = str_replace(array($DBInfo["User"], $DBInfo["Pass"]), "******", $Mysqli->error);
				$Message = array("Mysqli query error ".$Mysqli->errno, $QueryError);
				LogSQLError($Message);
			}
			return($TransactionCode);
		}
	}
	return("");
}

function TransactionCommit($TransactionCode)
{
        if (defined("DATABASE_TRANSACTIONS"))
        {
                if (DATABASE_TRANSACTIONS)
		{
			if (!trim($TransactionCode))
				LogSQLError(array("Fatal MySQLi Transaction Error", "Commit Transaction missing code."));
			if (gettype($GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]) && get_class($GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]))
			{
//				$GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->commit();
				$Result = $GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->query("COMMIT");
				if ($Result === false || $GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->error)
				{
					$QueryError = str_replace(array($GLOBALS["MYSQLI_TRANSACTION_DB_INFO"][$TransactionCode]["User"], $GLOBALS["MYSQLI_TRANSACTION_DB_INFO"][$TransactionCode]["Pass"]), "******", $GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->error);
					$Message = array("Mysqli query error ".$GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->errno, $QueryError);
					LogSQLError($Message);
				}
				unset($GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]);
				unset($GLOBALS["MYSQLI_TRANSACTION_SESSION"][$TransactionCode]);
				unset($GLOBALS["MYSQLI_TRANSACTION_RUNNING"][$TransactionCode]);
				unset($GLOBALS["MYSQLI_TRANSACTION_DB_INFO"][$TransactionCode]);
			}
			else
				LogSQLError(array("Fatal MySQLi Transaction Error", "Commit Transaction '".$TransactionCode."' does not exist."));
		}
	}
}

function TransactionRollback($TransactionCode)
{
        if (defined("DATABASE_TRANSACTIONS"))
        {
                if (DATABASE_TRANSACTIONS)
		{
			if (!trim($TransactionCode))
				LogSQLError(array("Fatal MySQLi Transaction Error", "Rollback Transaction missing code."));
			if (gettype($GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]) && get_class($GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]))
			{
//				$GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->rollback();
				$Result = $GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->query("ROLLBACK");
				if ($Result === false || $GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->error)
				{
					$QueryError = str_replace(array($GLOBALS["MYSQLI_TRANSACTION_DB_INFO"][$TransactionCode]["User"], $GLOBALS["MYSQLI_TRANSACTION_DB_INFO"][$TransactionCode]["Pass"]), "******", $GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->error);
					$Message = array("Mysqli query error ".$GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]->errno, $QueryError);
					LogSQLError($Message);
				}
				unset($GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$TransactionCode]);
				unset($GLOBALS["MYSQLI_TRANSACTION_SESSION"][$TransactionCode]);
				unset($GLOBALS["MYSQLI_TRANSACTION_RUNNING"][$TransactionCode]);
				unset($GLOBALS["MYSQLI_TRANSACTION_DB_INFO"][$TransactionCode]);
			}
			else
				LogSQLError(array("Fatal MySQLi Transaction Error", "Rollback Transaction '".$TransactionCode."' does not exist."));
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// DB_DoQuery functions - actual queries are centralised
//////////////////////////////////////////////////////////////////////////

function DB_RunQueryNow($DBName, $Query, $SequenceName = '')
{
	if (defined("MYSQLI_DISABLE_CACHE"))
		$Discrete = MYSQLI_DISABLE_CACHE;
	else
		$Discrete = 0;

	if(defined("TIME_LOG_QUERIES"))
	{
		$Timer = new StopWatch;
		global $QueryCounterVar;
		$QueryCounterVar++;
	}

	$DBInfo = ParseConnectDB($DBName);
	assert($DBInfo["Type"] === "mysql");

	$PersistKey = sha1($DBInfo["Host"].$DBInfo["User"].$DBInfo["Pass"].$DBInfo["Name"]);

	$TransactionCode = "";
	if ($DBInfo["Transaction"])
	{
		if ($GLOBALS["MYSQLI_TRANSACTION_SESSION"][$DBInfo["Transaction"]] == "")
			LogSQLError(array("Fatal MySQLi Transaction Error", "Transaction Connection '".$DBInfo["Transaction"]."'does not exist."));
		if ($GLOBALS["MYSQLI_TRANSACTION_SESSION"][$DBInfo["Transaction"]] != $PersistKey)
			LogSQLError(array("Fatal MySQLi Transaction Error", "Transaction Connection '".$DBInfo["Transaction"]."' incorrect database."));
		if ($GLOBALS["MYSQLI_TRANSACTION_RUNNING"][$DBInfo["Transaction"]])
			LogSQLError(array("Fatal MySQLi Transaction Error", "Transaction Connection '".$DBInfo["Transaction"]."' is currently active."));
		$Mysqli = &$GLOBALS["MYSQLI_TRANSACTION_CONNECT"][$DBInfo["Transaction"]];
		$GLOBALS["MYSQLI_TRANSACTION_RUNNING"][$DBInfo["Transaction"]] = 1;
		$TransactionCode = $DBInfo["Transaction"];
		$PersistID = -1;
	}
	elseif ($Discrete) // bypass the new caching code
	{
		$Mysqli = new mysqli($DBInfo["Host"], $DBInfo["User"], $DBInfo["Pass"], $DBInfo["Name"]);
	}
	else
	{
		$Instantiation = "";
		foreach (debug_backtrace() as $Level)
		{
			if (in_array($Level["class"], array("DBQuery", "DBInsert")))
			{
				if (!strstr($Level["file"], "phplib5.3/database"))
				{
					$Instantiation = $Level["file"]." on line ".$Level["line"];
					break;
				}
			}
		}

		if (!is_array($GLOBALS["MYSQLI_PERSISTENT_LOCATION"][$PersistKey]))
			$GLOBALS["MYSQLI_PERSISTENT_LOCATION"][$PersistKey] = array();
		if (!is_array($GLOBALS["MYSQLI_PERSISTENT_CONNECTS"][$PersistKey]))
			$GLOBALS["MYSQLI_PERSISTENT_CONNECTS"][$PersistKey] = array();
		if (!is_array($GLOBALS["MYSQLI_PERSISTENT_STATUSES"][$PersistKey]))
			$GLOBALS["MYSQLI_PERSISTENT_STATUSES"][$PersistKey] = array();
		if (!is_array($GLOBALS["MYSQLI_PERSISTENT_COMMENCE"][$PersistKey]))
			$GLOBALS["MYSQLI_PERSISTENT_COMMENCE"][$PersistKey] = array();

		$PersistID = 0;
		$Matches = array_keys($GLOBALS["MYSQLI_PERSISTENT_STATUSES"][$PersistKey], "CLOSED");
		foreach ($Matches as $PossibleID)
		{
			if ($GLOBALS["MYSQLI_PERSISTENT_COMMENCE"][$PersistKey][$PossibleID] <= (time() - MYSQLI_CONNECTION_TIMEOUT))
				$GLOBALS["MYSQLI_PERSISTENT_STATUSES"][$PersistKey][$PossibleID] = "IGNORE";
			elseif (!$PersistID)
				$PersistID = $PossibleID;
		}

		if (!$PersistID)
		{
			$PersistID = sizeof($GLOBALS["MYSQLI_PERSISTENT_STATUSES"][$PersistKey]) + 1;
			$GLOBALS["MYSQLI_PERSISTENT_CONNECTS"][$PersistKey][$PersistID] = new mysqli($DBInfo["Host"], $DBInfo["User"], $DBInfo["Pass"], $DBInfo["Name"]);
		}
		$GLOBALS["MYSQLI_PERSISTENT_STATUSES"][$PersistKey][$PersistID] = "ACTIVE";
		$GLOBALS["MYSQLI_PERSISTENT_COMMENCE"][$PersistKey][$PersistID] = time();
		$GLOBALS["MYSQLI_PERSISTENT_LOCATION"][$PersistKey][$PersistID] = $Instantiation;
		$Mysqli = &$GLOBALS["MYSQLI_PERSISTENT_CONNECTS"][$PersistKey][$PersistID];

		if (defined("MYSQLI_CONNECT_LIMIT"))
		{
			$Locations = array();
			foreach ($GLOBALS["MYSQLI_PERSISTENT_STATUSES"] as $Key => $Statuses)
				foreach ($Statuses as $ID => $Status)
					if ($Status == "ACTIVE")
						$Locations[$GLOBALS["MYSQLI_PERSISTENT_LOCATION"][$Key][$ID]]++;
			$ConnectLimit = 0;
			if (MYSQLI_CONNECT_LIMIT == "DEBUG")
			{
				$ConnectionOutput = "";
				foreach ($Locations as $Location => $Count)
				{
					if (trim($Location) && ($Count > 1))
						$ConnectionOutput .= "<p>".$Count." from ".$Location."<p>";
				}
				if (strlen($ConnectionOutput))
				{
					$ConnectLimit = 1;
					if (defined("MYSQLI_CONNECT_EMAIL"))
						mail(MYSQLI_CONNECT_EMAIL, ENV." Connection Limit", $ConnectionOutput);
					else
						echo $ConnectionOutput;
					exit;
				}
			}
			elseif (MYSQLI_CONNECT_LIMIT && ($PersistID > MYSQLI_CONNECT_LIMIT))
			{
				$Locations = array();
				foreach ($GLOBALS["MYSQLI_PERSISTENT_STATUSES"] as $Key => $Statuses)
					foreach ($Statuses as $ID => $Status)
						if ($Status == "ACTIVE")
							$Locations[$GLOBALS["MYSQLI_PERSISTENT_LOCATION"][$Key][$ID]]++;
				$ConnectionOutput = "";
				foreach ($Locations as $Location => $Count)
					if (trim($Location))
						$ConnectionOutput .= "<p>".$Count." from ".$Location."<p>";
				if (defined("MYSQLI_CONNECT_EMAIL"))
					mail(MYSQLI_CONNECT_EMAIL, ENV." Connection Limit", $ConnectionOutput);
				else
					echo $ConnectionOutput;
				exit;
			}
		}
	}

	if ($Mysqli->connect_error)
	{
		$ConnectError = $Mysqli->connect_error;
		$ConnectError = str_replace(array($DBInfo["User"], $DBInfo["Pass"]), "******", $ConnectError);
		$Message = array("Mysqli connect error $Mysqli->connect_errno", $ConnectError);
		LogSQLError($Message);
	}

	// run the query and get the result back
	$Result = $Mysqli->query($Query);
	if ($Result === false || $Mysqli->error)
	{
		$QueryError = str_replace(array($DBInfo["User"], $DBInfo["Pass"]), "******", $Mysqli->error);
		$Message = array("Mysqli query error ".$Mysqli->errno, $QueryError);
		LogSQLError($Message);
	}

	// $Result should be an instance of mysqli_result, or else True
	$Return = array();
	$Return["mysqli"] = &$Mysqli;
	$Return["mysqli_result"] = &$Result;
	$Return["insert_id"] = $Mysqli->insert_id;
	$Return["connection_key"] = $PersistKey;
	$Return["connection_id"] = $PersistID;
	$Return["transaction_code"] = $TransactionCode;

	if(defined("ECHO_QUERIES"))
	{
		echo $Query."<br>\n";
	}
	if(defined("TIME_LOG_QUERIES"))
	{
		$Duration = $Timer->GetTime($Timer->GetStopDuration());
		$Message = date("Y-m-d H:i:s")." (".$QueryCounterVar.") - Duration ".$Duration." - ".$Query."\n";
		if($QueryCounterVar == 1)
			file_put_contents("/tmp/dbclass_query.log", $Message);
		else
			file_put_contents("/tmp/dbclass_query.log", $Message, FILE_APPEND);
	}
	return $Return;
}
function DB_EndQueryNow($ConnectKey, $ConnectID, $TransactionCode)
{
	if (defined("MYSQL_DISABLE_CACHE"))
		$Discrete = MYSQL_DISABLE_CACHE;
	else
		$Discrete = 0;

	if ($TransactionCode)
	{
		$GLOBALS["MYSQLI_TRANSACTION_RUNNING"][$TransactionCode] = 0;
	}
	elseif ($Discrete)
	{
	}
	elseif (strlen($ConnectKey) && $ConnectID)
	{
		if ($GLOBALS["MYSQLI_PERSISTENT_COMMENCE"][$ConnectKey][$ConnectID] <= (time() - MYSQLI_CONNECTION_TIMEOUT))
		{
			$GLOBALS["MYSQLI_PERSISTENT_STATUSES"][$ConnectKey][$ConnectID] = "IGNORE";
		}
		else
		{
			$GLOBALS["MYSQLI_PERSISTENT_STATUSES"][$ConnectKey][$ConnectID] = "CLOSED";
		}
	}
}
function DB_RunQuery($DBName, $Query, $SequenceName = '')
{
	if (defined("DBLOG"))
	{
		$Start = array_sum(explode(" ", microtime())) * 100;
	}
	$DBHandles = DB_RunQueryNow($DBName, $Query, $SequenceName);
	if (defined("DBLOG"))
	{
		$Time = (array_sum(explode(" ", microtime())) * 100) - $Start;
		$Trigger = 0;
		if (defined("DBTRIGGER"))
			$Trigger = DBTRIGGER;
		if ($Time >= $Trigger)
		{
			$LogQuery = "";
			$LogQuery .= "insert into query_log ";
			$LogQuery .= "(datetime, query, duration, sys_load) ";
			$LogQuery .= "values ('";
			$LogQuery .= date("Y-m-d H:i:s");
			$LogQuery .= "', '";
			$LogQuery .= addslashes($Query);
			$LogQuery .= "', ";
			$LogQuery .= $Time;
			$LogQuery .= ", ";
			$a = explode(" ", trim(`cat /proc/loadavg`));
			$LogQuery .= array_shift($a);
			$LogQuery .= ")";
			DB_RunQueryNow(DBLOG, $LogQuery);
		}
	}
	return ($DBHandles);
}
function DB_DoQuery($DBName, $Query, $SequenceName = '')
{
	$Result = DB_RunQueryNow($DBName, $Query, $SequenceName);
	DB_EndQueryNow($Result["connection_key"], $Result["connection_id"], $Result["transaction_code"]);
}

//////////////////////////////////////////////////////////////////////////
// General Mysql Database Query Class
/////////////////////////////////////////////////////////////////////////

class DBQuery
{
	// an instance of mysql_result, containing the query results from Run() or RunOne()
	private $__MysqliResult;

	// the currently fetched row
	private $__RowData;

	// the current row number
	private $__Cursor; // MURRAY
//	private $__Cursor; // PETER

	public $ColumnInfo;

	function __construct()
	{
		$this->WasRun = 0;
	}

	//
	// Run the given query but don't do any fetching... leave that to GetNextRecord()
	//

	function Run($DBName, $Query, $GetColumnInfo = false)
	{
		if ($this->WasRun)
			$this->EndQuery();
		$Result = DB_RunQuery($DBName, $Query);
		$this->__MysqliResult = &$Result["mysqli_result"];
		$this->WasRun = 1;
		$this->__Cursor       = -1;
		$this->__RowData      = null;
		$this->__ConnectKey = $Result["connection_key"];
		$this->__ConnectID = $Result["connection_id"];
		$this->__TransactionCode = $Result["transaction_code"];

		$this->ColumnInfo = false;
		if ($GetColumnInfo)
		{
			$this->ColumnInfo = array();
			while ($FieldInfo = $this->__MysqliResult->fetch_field())
			{
				$this->ColumnInfo[] = array(
					"name"  => $FieldInfo->name,
					"type"  => $FieldInfo->type,
					"len"   => $FieldInfo->len
				);
			}
		}
	}

	//
	// Seek to the requested Row
	//
	// NOTE: a failed seek will not move the internal cursor, but it WILL set the current row data array to null
	//
	function Seek($RowNum) // PETER
	{
		// reset the row data
		$this->__RowData = null;
		// THIS IS TECHINCALLY CORRECT BUT DOES NOT REPLICATE A BUG THAT CURRENTLY EXISTS IN THE OLD MYSQL LIBRARIES
//		$this->__Cursor  = $RowNum; // MURRAY

		// try and move result set pointer to that position
		if (!$this->__MysqliResult->data_seek($RowNum)) // PETER
			return false;

		// THIS REPLICATES AN EXISTING BUG IN THE OLD MYSQL LIBRARIES
		$this->__Cursor  = $RowNum;
		$this->__RowData = $this->__MysqliResult->fetch_assoc();
		return true;
	}

	//
	// Reset the counter so that the next fetch is the first record
	//

	function Reset()
	{
		$this->__Cursor = -1;
		$this->__MysqliResult->data_seek(0);

		// NOTE: technically this should be resetting the row data as well, so that the internal state is always
		// consistent, but it's possible something is doing a Reset() and then a GetField() and expecting to still get
		// the last row - Peter Hodge 2013-03-27
		//$this->__RowData = null;
	}

	//
	// Relative Seek
	//

	function Relative($Offset)
	{
		return $this->Seek($this->__Cursor + $Offset);
	}
		
	//
	// Get the next record in the current query
	//

	function Next()
	{
		// DELIBERATE BUG TO REPLICATE THE OLD WAY OF DOING THINGS
		// IF THERE ARE NO MORE ROWS OF DATA, LEAVE __RowData ON THE LAST ROW, RATHER THAN EMPTY
		// REALISTICALLY THE CODE SHOULD BE:
		//	++$this->__Cursor;
		//	$this->__RowData = $this->__MysqliResult->fetch_assoc();
		//	return is_array($this->_RowData);;
		$NewRow = $this->__MysqliResult->fetch_assoc();
		if (is_array($NewRow))
		{
			$this->__RowData = $NewRow;
			++$this->__Cursor;
			return true;
		}
		else
			return false;
	}

	//
	// Get the previous record
	//

	function Previous()
	{
		return $this->Relative(-1);
	}
		
	//
	// Run the given query to get a record which can then be accessed as needed
	//
	//

	function RunOne($DBName, $Query, $GetColumnInfo = false)
	{
		$this->Run($DBName, $Query, $GetColumnInfo);
		$this->Next();
	}

	//
	// Return the value of the given column from the record fetched with GetRecord
	//

	function GetField($FieldName)
	{

		return $this->__RowData[$FieldName];
	}

	//
	// For the really lazy, this will output the value of the given field
	//

	function PrintField($FieldName)
	{
		echo $this->__RowData[$FieldName];
	}

	//
	// Return an entire data row ($this->__RowData) as an array.
	//

	function GetArray()
	{
		return is_array($this->__RowData) ? $this->__RowData : array();
	} 

	//
	// Merge an entire data row with an existing array. If $Protect is true we overwrite duplicate labels and the data.
	// Otherwise, we overwrite existing labels in the existing array.
	//
	function MergeArray($CurrentData, $Protect = 0)
	{
		if ($Protect)
			return(array_merge($this->GetArray(), $CurrentData));
		else
			return(array_merge($CurrentData, $this->GetArray()));
	}


	function NumRows()
	{
		return $this->__MysqliResult->num_rows;
	}

	function EndQuery()
	{
		if ($this->WasRun)
		{
			DB_EndQueryNow($this->__ConnectKey, $this->__ConnectID, $this->__TransactionCode);
			$this->__MysqliResult->free();
			//unset
			unset($this->__Cursor);
			unset($this->__RowData);
			unset($this->__MysqliResult);
			unset($this->ColumnInfo);
		}
		$this->WasRun = 0;
		return true;
	}

	function __destruct()
	{
		if ($this->WasRun)
			$this->EndQuery();
	}

} // End of Class



///////////////////////////////////////////////////
// Class to facilitate inserting into a mysql database.
///////////////////////////////////////////////////

class DBInsert
{
//
// A 2 dimensional array for
// lists of values for multiple inserts
//
	var $ValueList;

	var $ColumnList;
	var $WhereList;

//
// Variable used to store the number of multiple queries
//
	var $NrQueries=0;

//
// variables for using a select statement in an insert/replace
//
	var $UsingSelect=0;
	var $SelectString;
	var $SelectWhere=0;

//
// Externally set variables
//
	var $Debug=0;
	var $NoRun=0;

//
// The name of a sequence used by PGSQL - Assumed to have been explicitly
// set by the programmer ... not needed by MySQL
//
	var $SequenceName='';

	function EndQuery()
	{
		$this->ValueList = "";
		$this->ColumnList = "";
		$this->WhereList = array();
		$this->NrQueries = 0;
		$this->Query = "";
		$this->UsingSelect=0;
		$this->SelectString="";
		$this->SelectWhere=0;
		$this->ColType = array();
		$this->ColSize = array();
	}

	function AddQuery()
	{
		$this->NrQueries++;
	}

	function AddSelect($TableName, $ColumnList, $WhereString = "")
	{
		$this->SelectString = "SELECT $ColumnList FROM $TableName";
		$this->UsingSelect = 1;
		if($WhereString == "")
			$this->SelectWhere = 0;
		else
		{
			$this->SelectWhere = 1;
			if(strpos(strtoupper($WhereString),"WHERE ") != 0)
				$this->SelectString .= " WHERE";
			$this->SelectString .= " $WhereString";
		}
	}

	function SetupQuery($TableName, $QueryType)
	{
		$Comma = "";
		if($QueryType == "UPDATE")
		{
			$Query = "$QueryType $TableName SET ";
			foreach($this->ValueList[0] as $Column => $Value)
			{
				$Query .= $Comma.$Column." = ".$this->TypeCheck($Value, $this->ColType[$Column]);
				$Comma = ",";
			}

			$Query .= $this->WhereList();

			$this->Query = $Query;
			return true;
		}

		$Columns = "";
		$QueryValues = array();

		for ($i=0;$i<=$this->NrQueries;$i++)
			$QueryValues[$i] = "";

//
// If an insert or replace
//
		foreach($this->ColumnList as $Field => $Val)
		{
			$Columns .= $Comma.$Field;

			for($i=0;$i<=$this->NrQueries;$i++)
				$QueryValues[$i] .= $Comma.$this->TypeCheck($this->ValueList[$i][$Field], $this->ColType[$Field]);
			$Comma = ",";
		}

		$Comma = "";
		$Query = "$QueryType INTO $TableName ($Columns) VALUES ";
		foreach($QueryValues as $QVStr)
		{
			$Query .= $Comma."($QVStr)";

			$Comma = ",";
		}

		$this->Query = $Query;
		return true;
	}

	function TypeCheck($Value, $Type)
	{
		$Types = explode(" ", $Type." ");
		if (($Types[1] == "NULL") && ($Value === ""))
			return ("NULL");
		switch ($Types[0])
		{
			case 'FORCENULL':
				$Value = "NULL";
				break;
			case 'FUNCTION':
				break;

			case 'TINYINT':
			case 'SMALLINT':
			case 'MEDIUMINT':
			case 'INT':
			case 'INT2':
			case 'INT4':
			case 'INT8':
			case 'INTEGER':
			case 'BIGINT':
				if("$Value" == "")
					$Value = "NULL";
				else
					$Value = sql::integer($Value);
				break;

			case 'SERIAL':
			case 'REAL':
			case 'DOUBLE':
			case 'FLOAT':
			case 'DECIMAL':
			case 'NUMERIC':
				if("$Value" == "")
					$Value = "NULL";
				else
					$Value = sql::number($Value);
				break;

			case 'DATE':
			case 'TIME':
			case 'TIMESTAMP':
			case 'DATETIME':
				$Value = "'".sql::stamp($Value)."'";
				break;

			case 'AUTO':
				$Value = 0;
				break;

			case 'CHAR':
			case 'STRING':
			case 'TINYBLOB':
			case 'BLOB':
			case 'MEDIUMBLOB':
			case 'LONGBLOB':
			case 'TINYTEXT':
			case 'TEXT':
			case 'MEDIUMTEXT':
			case 'LONGTEXT':
			case 'VARCHAR':
			default:
				$Value = "'".sql::text($Value)."'";
		}
		return $Value;
	}

	function Save ($Field, $Value)
	{
		$this->Add ("", $Field, $Value);
	}

	function Add ($Type, $Field, $Value)
	{
		$this->ColType[$Field] = str_replace(" ", "", strtoupper($Type));

		$this->ValueList[$this->NrQueries][$Field] = $Value;
		if($this->NrQueries == 0)
			$this->ColumnList[$Field] = 1;
		else
		{
			if($this->ColumnList[$Field] != 1)
			{
				LogSQLError(array("Fatal Error", "'".$Field."' was not included in earlier inserts."));
			}
		}
	}

	function SaveWhere ($Field, $Value, $Relationship = "=")
	{
		$this->AddWhere("", $Field, $Value, $Relationship);
	}

	function AddWhere($Type, $Field, $Value, $Relationship = "=")
	{
		$this->ColType[$Field] = str_replace(" ", "", strtoupper($Type));
		$this->WhereList[$Field]["val"] = $Value;
		$this->WhereList[$Field]["rel"] = $Relationship;
	}

	function WhereList()
	{
		$WhereList = "";
		if (is_array($this->WhereList))
		{
			foreach ($this->WhereList as $Field => $Data)
			{
				if ($WhereList != "")
					$WhereList .= " AND ";
				else
					$WhereList = " WHERE ";
				$WhereList .= $Field." ".$Data["rel"]." ".$this->TypeCheck($Data["val"], $this->ColType[$Field]);
			}
		}
		return ($WhereList);
	}

	function Run($DBName, $Table, $QueryType)
	{
		if(substr($DBName,0,5) != "mssql")
		{
			// NOTE: using a 
			static $DB_TABLE_TYPE_CACHE = array();

			// do a simple query to get the data types
			if (!isset($DB_TABLE_TYPE_CACHE[$DBName][$Table]))
			{
				$Q = new DBQuery;
				$Q->Run($DBName, "select * from ".$Table." limit 0", true);
				$DB_TABLE_TYPE_CACHE[$DBName][$Table] = $Q->ColumnInfo;
				$Q->EndQuery();
			}

			foreach ($DB_TABLE_TYPE_CACHE[$DBName][$Table] as $Column)
			{
				if ($this->ColType[$Column["name"]] == "NULL")
					$this->ColType[$Column["name"]] = strtoupper($Column["type"])." NULL";
				if ($this->ColType[$Column["name"]] == "")
					$this->ColType[$Column["name"]] = strtoupper($Column["type"]);
				$this->ColSize[$Column["name"]] = $Column["len"];
			}
		}

		if(!($QueryType == "INSERT" || $QueryType == "UPDATE" || $QueryType == "REPLACE"))
		{
			LogSQLError(array("Warning", "Invalid Query Type '".$QueryType."'."), 1);
			return false;
		}

		if($QueryType == "UPDATE" && $this->NrQueries > 0)
		{
			LogSQLError(array("Warning", "Cannot update with multiple queries."), 1);
			return false;
		}

		if($QueryType == "UPDATE" && $this->UsingSelect == 1)
		{
			LogSQLError(array("Warning", "Cannot update with a select data fetch."), 1);
			return false;
		}

		if(!$this->SetupQuery($Table,$QueryType))
		{
			LogSQLError(array("Warning", "NO Query Values."), 1);
			return false;
		}

		if ($this->WhereList() != "" && $QueryType != "UPDATE") 
		{
			LogSQLError(array("Warning", "Where clause supplied with a '".$QueryType."' query."), 1);
			return false;
		}

		if ($this->Debug == 1)
		{
			echo ($this->NrQueries+1)." Queries<BR>\n";
			echo $this->Query."<BR>\n";
		}

		if ($this->NoRun == 1)
			return true;

		$Result = DB_RunQuery($DBName, $this->Query, $this->SequenceName);
		$this->__ConnectKey = $Result["connection_key"];
		$this->__ConnectID = $Result["connection_id"];
		$this->__TransactionCode = $Result["transaction_code"];
		$this->InsertID = $Result["insert_id"];
		DB_EndQueryNow($this->__ConnectKey, $this->__ConnectID, $this->__TransactionCode);

		return true;
	}

	function GetInsertId()
	{
		return $this->InsertID;
	}
}

// QUICK ACCESS FUNCTIONS

function DBRow($DBName, $SQL_QUERY)
{
	return(QuickDB::DBRow($DBName, $SQL_QUERY));
}

function DBGet($DBName, $SQL_QUERY)
{
	return(QuickDB::DBGet($DBName, $SQL_QUERY));
}

function DBResult($DBName, $SQL_QUERY)
{
	return(QuickDB::DBResult($DBName, $SQL_QUERY));
}

function DBResults($DBName, $SQL_QUERY)
{
	return(DBResult($DBName, $SQL_QUERY));
}

function DBList($DBName, $SQL_QUERY)
{
	return(QuickDB::DBList($DBName, $SQL_QUERY));
}

function DBIndexedBlank($DBName, $SQL_QUERY)
{
	return(QuickDB::DBIndexedBlank($DBName, $SQL_QUERY));
}

function DBIndexed($DBName, $SQL_QUERY)
{
	return(QuickDB::DBIndexed($DBName, $SQL_QUERY));
}

// made redundant - actually no... there is complex code out there that relies on this stuff - Scott 1/5/07
// Will come back to this later

function DBLookup($DBName, $SQL_QUERY, $RetVals)
{
	// First we check to see if any return parameters were passed
	// The exercise is pointless if they weren't!
	$NumReturnParams = count($RetVals);

	if($NumReturnParams == 0)
	{
		LogSQLError(array("Fatal Error", "Insufficient Return Parameters Passed."));
	}

	$Q = new DBQuery;
	$Q->Run($DBName, $SQL_QUERY);

	if ($Q->Next())
	{
		// Check out how many columns are in the result
		$RowData = $Q->GetArray();
		$FieldNames = array_keys($RowData);
		$NumReturnParams = min($NumReturnParams, count($RowData));
		for ($i = 0; $i < $NumReturnParams; ++$i)
			$RetVals[$i] = $RowData[$FieldNames[$i]];

		$Q->EndQuery();
		return(1);
	}
	else
	{
		$Q->EndQuery();
		return(0);
	}
}

function DBLookupArray($DBName, $SQL_QUERY, &$RetArray)
{
	$Q = new DBQuery;
	$Q->Run($DBName, $SQL_QUERY);

	if($Q->Next())
	{
		$RetArray = $Q->GetArray();
		$Q->EndQuery();
		return($RetArray);
	}
	else
	{
		$Q->EndQuery();
		return($RetArray);
	}
}

function DBLookupList($DBName, $SQL_QUERY, &$RetArray)
{
	$RetArray = array();
	$Q = new DBQuery;
	$Q->Run($DBName, $SQL_QUERY);
	for($i=0; $Q->Next(); $i++)
		$RetArray[$i] = $Q->GetArray();
	$Q->EndQuery();
	return($i);
}

function DBAssociative($DBName, $SQL_QUERY)
{
	return(DBRow($DBName, $SQL_QUERY));
}

function DBInsert($DBName, $Table, $Data, $AutoCol = 0, $Test = 0)
{
	$ID = 0;
	$INQ = new DBInsert;
	if ($AutoCol && strstr($DBName, "postgres"))
		$INQ->SequenceName = $Table."_".$AutoCol;
	if ($Test)
	{
		$INQ->NoRun = 1;
		$INQ->Debug = 1;
	}
	foreach ($Data as $Field => $Value)
		$INQ->Save($Field, $Value);
	$INQ->Run($DBName, $Table, "INSERT");
	if ($AutoCol)
		$ID = $INQ->GetInsertId();
	$INQ->EndQuery();
	return ($ID);
}

?>
