<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

//
// database.inc
//
// Database access classes wrapped around PEAR DB
//
// Author: Scott McClintock
// 
// Copyright 2003 MU Systems Pty Ltd
//
//

require_once(PHPLIB_DIR."database.inc");

function LogSQLError($Message, $Continue = 0)
{
	QuickDB::LogSQLError($Message, $Continue);
}

//////////////////////////////////////////////////////////////////////////
// DB_DoQuery functions - actual queries are centralised
//////////////////////////////////////////////////////////////////////////

class DB_RunQuery
{
	function DB_RunQueryNow($DBName, $Query, $SequenceName = '')
	{
		if(defined("TIME_LOG_QUERIES"))
		{
			$Timer = new StopWatch;
			global $QueryCounterVar;
			$QueryCounterVar++;
		}
///////////////////////

		$this->DBHandles = array();
//		DBERR::DetectError($DBName, $this->DBHandles["Connect"]);
//		$this->DBHandles["Query"] = $this->DBHandles["Connect"]->query($Query);
//		DBERR::DetectError($DBName, $this->DBHandles["Query"]);

//    [Type] => mysql
//    [User] => webaccess
//    [Pass] => webacc355
//    [Host] => localhost
//    [Name] => ACCOUNTS_DEV

		$DBParts = ParseConnectDB($DBName);
		$Connection = $DBParts["Type"].":host=".$DBParts["Host"].";dbname=".$DBParts["Name"];
		$Parameters = array();
		$Parameters[PDO::ATTR_ERRMODE] = PDO::ERRMODE_SILENT;
		$Parameters[PDO::ATTR_PERSISTENT] = true;
		try
		{
			$this->DBHandles["Connect"] = @new PDO($Connection, $DBParts["User"], $DBParts["Pass"], $Parameters);
		}
		catch (PDOException $e)
		{
			LogSQLError(array("PDO Database Error", $e->getMessage()));
		}

		$this->DBHandles["Query"] = $this->DBHandles["Connect"]->query($Query);
		$Error = $this->DBHandles["Connect"]->errorInfo();
		if ($Error[0] * 1)
			LogSQLError($Error[2]);

//	echo "\n[".$this->DBHandles["Query"]->rowCount()."]";
//	while ($row = $this->DBHandles["Query"]->fetch(PDO::FETCH_ASSOC))
//		echo "\n".implode(" ", $row);
//

		if(defined("ECHO_QUERIES"))
		{
			echo "$Query<BR>\n";
		}
		if(defined("TIME_LOG_QUERIES"))
		{
			$Duration = $Timer->GetTime($Timer->GetStopDuration());

//			if($Duration != "00:00:00.00")
//			{
				$Message = date("Y-m-d H:i:s")." ($QueryCounterVar) - Duration $Duration - $Query\n";
				if($QueryCounterVar == 1)
					file_put_contents("/tmp/dbclass_query.log",$Message);
				else
					file_put_contents("/tmp/dbclass_query.log",$Message, FILE_APPEND);
//			}
		}

// Can we really get an insert id from an update?
		if (in_array(substr(strtolower($Query), 0, 7), array("insert ","update ","replace")))
		{
//			if(substr($DBName,0,5) == "mssql")
//			{
//				$Temp = $this->DBHandles["Connect"]->query("SELECT @@identity AS id");
//				$RecRow = $Temp->fetchRow(DB_FETCHMODE_ASSOC, 0);
//				$this->DBHandles["InsertID"] = $RecRow["id"];
//			}
//			else
//			{
				$this->DBHandles["InsertID"] = $this->DBHandles["Connect"]->lastInsertId($SequenceName);
//			}
		}
		else
			$this->DBHandles["InsertID"] = 0;
	}

	function __construct($DBName, $Query, $SequenceName = '')
	{
		if (defined("DBLOG"))
		{
			$Start = array_sum(explode(" ", microtime())) * 100;
		}
		$this->DB_RunQueryNow($DBName, $Query, $SequenceName);
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
				$this->DB_RunQueryNow(DBLOG, $LogQuery);
			}
		}
	}

	function EndQuery($NoClose = 0)
	{
		unset($this->DBHandles);
	}
}

function DB_DoQuery($DBName, $Query, $SequenceName = '')
{

	$Handle = new DB_RunQuery($DBName, $Query, $SequenceName);
	unset($DBHandles);
}

//////////////////////////////////////////////////////////////////////////
// General Mysql Database Query Class
/////////////////////////////////////////////////////////////////////////

class DBQuery
{
	var $PDO;		// The PDO object
	var $GetRecRow;		// The currently fetched row
	var $RowCount;		// The current row (starts at -1 so that the first is 0

	function DoQuery($DBName, $Query)
	{
		$this->PDO = new DB_RunQuery($DBName, $Query);
	}

	//
	// Run the given query but don't do any fetching... leave that to GetNextRecord()
	//

	function Run($DBName, $Query)
	{
		$this->RowCount = -1;
		$this->DoQuery($DBName, $Query);
		$this->GetRecResult &= $this->PDO->DBHandles["Query"];
//		$R = &$this->GetRecResult;
//		DBERR::DetectError($DBName, $R);
// LogSQLError($Message);
//		unset($R);
	}

	//
	// Seek to the requested Row
	//

	function Seek($RowNum)
	{
		if ($this->RowCount == $RowNum - 1 && $RowNum <> 0)
		{
			if($RowNum >= $this->GetRecResult->rowCount())
			{
				//echo "END of RESULT - Rownum: ".$RowNum."\n";
				return(FALSE);
			}
		}
		$this->GetRecRow = $this->GetRecResult->fetch(PDO::FETCH_ASSOC, PDO::FETCH_ORI_AB, $RowNum);

		if(!is_array($this->GetRecRow))
		{
			//echo "FETCH FAILED! - Rownum: ".$RowNum."\n";
			return(FALSE);
		}
		else
		{
			$this->RowCount = $RowNum;
			return(TRUE);
		}
	}

	//
	// Reset the counter so that the next fetch is the first record
	//

	function Reset()
	{
		$this->RowCount = -1;
	}

	//
	// Relative Seek
	//

	function Relative($Offset)
	{
		return($this->Seek($this->RowCount + $Offset));
	}
		
	//
	// Get the next record in the current query
	//

	function Next()
	{
		return($this->Relative(1));
	}

	//
	// Get the previous record
	//

	function Previous()
	{
		return($this->Relative(-1));
	}
		
	//
	// Run the given query to get a record which can then be accessed as needed
	//
	//

	function RunOne($DBName, $Query)
	{
		$this->Run($DBName, $Query);
		$this->Next();
	}

	//
	// Return the value of the given column from the record fetched with GetRecord
	//

	function GetField($FieldName)
	{
		if (isset($this->GetRecRow[$FieldName]))
			return($this->GetRecRow[$FieldName]);
	}

	//
	// For the really lazy, this will output the value of the given field
	//

	function PrintField($FieldName)
	{
		echo $this->GetField($FieldName);
	}

	//
	// Return an entire data row ($this->GetRecRow) as an array.
	//

	function GetArray()
	{
		if (is_array($this->GetRecRow))
			return ($this->GetRecRow);
		else
			return (array());
	} 

	//
	// Merge an entire data row ($this->GetRecRow) with an existing array.
	// If $Protect is true we overwrite duplicate labels and the data.
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
		return($this->GetRecResult->rowCount());
	}

	function EndQuery($NoClose = 0)
	{
		if (!$NoClose)
		{
			$this->PDO->EndQuery($NoClose);
		}
		//unset
		unset($this->RowCount);
		unset($this->GetRecResult);
		unset($this->RowCount);
		return;
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
		$Types = explode(" ", $Type);
		if (($Types[1] == "NULL") && ($Value === ""))
			return ("NULL");
		switch ($Types[0])
		{
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

	function Run($DBName, $Table, $QueryType, $NoClose = 0)
	{
		if(substr($DBName,0,5) != "mssql")
		{
			// do a simple query to get the data types
			if (!is_array($GLOBALS["DB_TABLE_TYPE_CACHE"][$Table]))
			{
				$Q = new DBQuery;
				$Q->Run($DBName, "select * from ".$Table." limit 0");
				$GLOBALS["DB_TABLE_TYPE_CACHE"][$Table] = $Q->DataInfo;
				$Q->EndQuery($NoClose);
			}

			foreach ($GLOBALS["DB_TABLE_TYPE_CACHE"][$Table] as $Column)
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

		$PDO = new DB_RunQuery($DBName, $this->Query, $this->SequenceName);
		$this->InsertID = $PDO->DBHandles["InsertID"];
		$PDO->EndQuery($NoClose);
		if (!$NoClose)
			unset($PDO);
//		$DBHandles = DB_RunQuery($DBName, $this->Query, $this->SequenceName);
//		$this->InsertID = $DBHandles["InsertID"];
//		if (!$NoClose)
//		{
//			$DBHandles["Connect"]->disconnect();
//			unset($DBHandles);
//		}

		return true;
	}

	function GetInsertId()
	{
		return $this->InsertID;
	}
}

// QUICK ACCESS FUNCTIONS

function DBRow($DBName, $SQL_QUERY, $NoClose = 0)
{
	return(QuickDB::DBRow($DBName, $SQL_QUERY, $NoClose));
}

function DBGet($DBName, $SQL_QUERY, $NoClose = 0)
{
	return(QuickDB::DBGet($DBName, $SQL_QUERY, $NoClose));
}

function DBResult($DBName, $SQL_QUERY, $NoClose = 0)
{
	return(QuickDB::DBResult($DBName, $SQL_QUERY, $NoClose));
}

function DBResults($DBName, $SQL_QUERY, $NoClose = 0)
{
	return(DBResult($DBName, $SQL_QUERY, $NoClose));
}

function DBList($DBName, $SQL_QUERY, $NoClose = 0)
{
	return(QuickDB::DBList($DBName, $SQL_QUERY, $NoClose));
}

function DBIndexedBlank($DBName, $SQL_QUERY, $NoClose = 0)
{
	return(QuickDB::DBIndexedBlank($DBName, $SQL_QUERY, $NoClose));
}

function DBIndexed($DBName, $SQL_QUERY, $NoClose = 0)
{
	return(QuickDB::DBIndexed($DBName, $SQL_QUERY, $NoClose));
}

// made redundant - actually no... there is complex code out there that relies on this stuff - Scott 1/5/07
// Will come back to this later

/*
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
		$NumColumns = $Q->GetRecResult->columnCount();

		// Now loop through the columns and grab the values
		reset($Q->GetRecRow);
		for($i = 0; $i < $NumColumns && $i < $NumReturnParams; $i++)
		{
			$a = each($Q->GetRecRow);
			$RetVals[$i] = array_shift($a);
		}

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
*/


?>
