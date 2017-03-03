<?php
/*
Copyright: MU Systems Pty Ltd, 2008
*/

require_once(PHPLIB_DIR."variable.inc");

class query_list extends variable
{
	function __construct()
	{
		parent::__construct();
		$this->MatchModes = array();
		$this->MatchModes[] = "EQUALS";
		$this->MatchModes[] = "LIKE";
		$this->MatchModes[] = "NOT";
		$this->MatchModes[] = "IN";
		$this->MatchModes[] = "NOT IN";
		$this->MatchModes[] = "BETWEEN";
		$this->MatchModes[] = "LESS THAN";
		$this->MatchModes[] = "LESS THAN EQUAL";
		$this->MatchModes[] = "MORE THAN";
		$this->MatchModes[] = "MORE THAN EQUAL";

		$this->EndQuery();
	}

	function EndQuery()
	{
		$this->Joins = array();
		$this->Tables = array();
		$this->TableKeys = array();
		$this->TableDisplay = array();

		$this->Limit = 0;
		$this->Order = "";
		$this->OrderTable = "";
		$this->Filters = array();

		$this->Index = -1;
		$this->Query = "";
		$this->Results = array();
	}

	function NewQuery($Table)
	{
		$this->EndQuery();
		$this->Tables[] = $Table;
		$this->TableKeys[$Table] = "t0";
	}

	function _CheckTable($Table)
	{
		$Table = trim($Table);
		if ($Table)
		{
			if (in_array($Table, $this->Tables))
				return($Table);
			else
				exit("\n\nFATAL ERROR: Supplied table has not been defined.\n\n");
		}
		else
			return($this->Tables[0]);
	}

	function _NewTable($Table)
	{
		$Table = trim($Table);
		if ($Table)
		{
			if (!in_array($Table, $this->Tables))
				return($Table);
		}
		exit("\n\nFATAL ERROR: Supplied table has not been defined.\n\n");
	}

	function Join($SourceTable, $SourceColumn, $TargetTable, $TargetColumn)
	{
		if ($this->_CheckTable($SourceTable))
		{
			if ($this->_NewTable($TargetTable))
			{
				$Join = array();
				$Join["source_table"]  = $SourceTable;
				$Join["source_column"] = $SourceColumn;
				$Join["target_table"]  = $TargetTable;
				$Join["target_column"] = $TargetColumn;
				$this->Joins[] = $Join;
				$k = sizeof($this->Tables);
				$this->Tables[$k] = $TargetTable;
				$this->TableKeys[$TargetTable] = "t".$k;
			}
		}
	}

	function SetLimit($Limit)
	{
		$this->Limit = trim($Limit);
		$this->PagerSize = 0;
	}

	function SetOrder($Order, $Table = "")
	{
		$this->Order = trim($Order);
		$this->OrderTable = $this->_CheckTable($Table);
	}

	function Filter($Field, $Match, $Value, $Table = "")
	{
		if (!in_array($Match, $this->MatchModes))
			exit("\n\nFATAL ERROR: Invalid Filter Match type supplied.\n\n");
		$Filter = array();
		$Filter["field"] = $Field;
		$Filter["match"] = $Match;
		$Filter["value"] = $Value;
		$Filter["table"] = $this->_CheckTable($Table);
		$this->Filters[] = $Filter;
	}

	function Run($DB)
	{
		$this->Query  = "";
		$this->Query .= "SELECT ";
		$this->Query .= "* ";
		$this->Query .= "FROM ";
		$Tables = array();
		foreach ($this->Tables as $Table)
			$Tables[] = $Table." ".$this->TableKeys[$Table];
		$this->Query .= implode(", ", $Tables);
		$this->Query .= " WHERE ";

		$Selects = array();

		foreach ($this->Joins as $Join)
		{
			$Select = "";
			$Select .= $this->TableKeys[$Join["source_table"]].".".$Join["source_column"];
			$Select .= " = ";
			$Select .= $this->TableKeys[$Join["target_table"]].".".$Join["target_column"];
			$Selects[] = $Select;
		}

		foreach ($this->Filters as $Filter)
		{
			$Select = "";
			switch(strtoupper($Filter["match"]))
			{
				case "EQUALS":
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " = '";
					$Select .= sql::text($Filter["value"]);
					$Select .= "'";
					$Selects[] = $Select;
					break;
				case "LIKE":
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " LIKE '";
					$Select .= sql::text($Filter["value"]);
					$Select .= "'";
					$Selects[] = $Select;
					break;
				case "NOT":
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " <> '";
					$Select .= sql::text($Filter["value"]);
					$Select .= "'";
					$Selects[] = $Select;
					break;
				case "IN":
					$Values = array();
					foreach ($Filter["value"] as $Value)
						$Values[] = sql::text($Value);
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " IN (";
					$Select .= implode(", ", $Values);
					$Select .= ")";
					$Selects[] = $Select;
					break;
				case "NOT IN":
					$Values = array();
					foreach ($Filter["value"] as $Value)
						$Values[] = sql::text($Value);
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " NOT IN (";
					$Select .= implode(", ", $Values);
					$Select .= ")";
					$Selects[] = $Select;
					break;
				case "BETWEEN":
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " BETWEEN '";
					$Select .= sql::text($Filter["value"][0]);
					$Select .= "' AND '";
					$Select .= sql::text($Filter["value"][1]);
					$Select .= "'";
					$Selects[] = $Select;
					break;
				case "LESS THAN":
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " < '";
					$Select .= sql::text($Filter["value"]);
					$Select .= "'";
					$Selects[] = $Select;
					break;
				case "LESS THAN EQUAL":
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " <= '";
					$Select .= sql::text($Filter["value"]);
					$Select .= "'";
					$Selects[] = $Select;
					break;
				case "MORE THAN":
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " > '";
					$Select .= sql::text($Filter["value"]);
					$Select .= "'";
					$Selects[] = $Select;
					break;
				case "MORE THAN EQUAL":
					$Select .= $this->TableKeys[$Filter["table"]].".".$Filter["field"];
					$Select .= " >= '";
					$Select .= sql::text($Filter["value"]);
					$Select .= "'";
					$Selects[] = $Select;
					break;
			}
		}
		$this->Query .= implode(" AND ", $Selects);

		if (trim($this->Order))
			$this->Query .= " ORDER BY ".$this->TableKeys[$this->OrderTable].".".$this->Order;

		if ($this->PagerSize)
		{
			if (!$this->GetPagerRows())
			{
				$Parts = explode(" FROM ", $this->Query);
				array_shift($Parts);
				$Query = "SELECT COUNT(*) FROM ".implode(" FROM ", $Parts);
				$this->SetPagerRows(DBGet($DB, $Query));
			}
			$this->Nav = $this->PagerNavigation($this->PagerURL);
		}

		if ($this->Limit)
			$this->Query .= " LIMIT ".$this->Limit;
		if ($this->PagerSize)
				$this->Query .= $this->GetPagerLimit();

		$this->Results = DBResult($DB, $this->Query);
		$this->Reset();
	}

	function Reset()
	{
		$this->Index = -1;
	}

	function GetRows()
	{
		return($this->Results);
	}

	function NumRows()
	{
		return(sizeof($this->Results));
	}

	function Next()
	{
		$this->Index++;
		if (is_array($this->Results[$this->Index]))
			return(1);
		else
			return(0);
	}

	function GetArray()
	{
		return($this->Results[$this->Index]);
	}

	function GetField($Field)
	{
		return($this->Results[$this->Index][$Field]);
	}

	function DisplayField($Label, $Field, $Function = "")
	{
		$Col = array();
		$Col["label"] = $Label;
		$Col["field"] = $Field;
		$Col["function"] = $Function;
		$this->TableDisplay[] = $Col;
	}

	function Display($Row0 = "", $Row1 = "", $Cols = 1)
	{
		$this->T = new CWTable;

		if (trim($this->Nav))
		{
			$this->T->AddData($this->Nav, $Cols);
			$this->T->EndRow();
		}

		if ($Row0 && $Row1)
			$this->T->AlternateRows($Row0, $Row1);
		foreach ($this->TableDisplay as $Col)
			$this->T->AddHead($Col["label"]);
		$this->T->EndRow();
		$this->Reset();
		while ($this->Next())
		{
			foreach ($this->TableDisplay as $Col)
			{
				if ($Col["function"])
				{
					$FUNC = $Col["function"];
					$this->T->AddData($this->$FUNC($this->GetField($Col["field"])));
				}
				else
					$this->T->AddData($this->GetField($Col["field"]));
			}
			$this->T->EndRow();
		}
		return($this->T->Get());
	}

	function UsePager($Size, $URL)
	{
		$this->Limit = trim(0);
		$this->PagerURL = $URL;
		$this->MakeL("PagerRows", "PagerSize", "PagerPage");
		if (!$this->PagerSize)
		{
			$this->PagerRows = 0;
			$this->PagerSize = $Size;
			$this->PagerPage = 0;
		}
	}

	function SetPagerRows($Rows)
	{
		if ($this->PagerRows != $Rows)
		{
			$this->PagerRows = $Rows;
			$this->PagerPage = 0;
		}
	}

	function GetPagerRows()
	{
		return($this->PagerRows);
	}

	function GetPagerLimit()
	{
		$Limit  = "";
		$Limit .= " LIMIT ";
		if ($this->PagerPage)
		{
			$Limit .= ($this->PagerPage * $this->PagerSize);
			$Limit .= ",";
		}
		$Limit .= $this->PagerSize;
		return($Limit);
	}

	function PagerNavigation($URL, $IsAjax = 0)
	{
		$Box1 = "<span class=\"button_box\">";
		$Box2 = "</span>";

		if (strstr($URL, "?"))
			$URL .= "&PagerRows=".$this->PagerRows;
		else
			$URL .= "?PagerRows=".$this->PagerRows;

		$Pages = ceil($this->PagerRows / $this->PagerSize);

		$HREF  = "";
		$HREF .= "<a class=\"button_box\" href=\"";
		if ($IsAjax)
			$HREF .= "javascript:GetAJAX('";
		$HREF .= $URL;
		$HREF .= "&PagerSize=";
		$HREF .= $this->PagerSize;
		$HREF .= "&PagerPage=PageXX";
		if ($IsAjax)
			$HREF .= "')";
		$HREF .= "\">";

		$Navigation = "";
		if ($this->PagerPage)
		{
			$Navigation .= str_replace("PageXX", 0, $HREF);
			$Navigation .= "[&lt;&lt;";
			$Navigation .= "</a>";
			$Navigation .= " ";
			$Navigation .= str_replace("PageXX", ($this->PagerPage - 1), $HREF);
			$Navigation .= "&lt;";
			$Navigation .= "</a>";
		}
		else
		{
			$Navigation .= $Box1;
			$Navigation .= "[&lt;&lt;";
			$Navigation .= $Box2;
			$Navigation .= " ";
			$Navigation .= $Box1;
			$Navigation .= "&lt;";
			$Navigation .= $Box2;
		}
		$Navigation .= " ";
		$Navigation .= "Page ";
		$Navigation .= ($this->PagerPage + 1);
		$Navigation .= " of ";
		$Navigation .= $Pages;
		$Navigation .= " ";


		if ($this->PagerPage < ($Pages - 1))
		{
			$Navigation .= str_replace("PageXX", ($this->PagerPage + 1), $HREF);
			$Navigation .= "&gt;";
			$Navigation .= "</a>";
			$Navigation .= " ";
			$Navigation .= str_replace("PageXX", ($Pages - 1), $HREF);
			$Navigation .= "&gt;&gt;]";
			$Navigation .= "</a>";
		}
		else
		{
			$Navigation .= $Box1;
			$Navigation .= "&gt;";
			$Navigation .= $Box2;
			$Navigation .= " ";
			$Navigation .= $Box1;
			$Navigation .= "&gt;&gt;]";
			$Navigation .= $Box2;
		}

		$Sizes = array(10, 25, 50, 100);
		if ($this->PagerRows > $Sizes[0])
		{
			$HREF  = "";
			$HREF .= "<a class=\"button_box\" href=\"";
			if ($IsAjax)
				$HREF .= "javascript:GetAJAX('";
			$HREF .= $URL;
			$HREF .= "&PagerPage=0";
			$HREF .= "&PagerSize=SizeXX";
			if ($IsAjax)
				$HREF .= "')";
			$HREF .= "\">";

			$Navigation .= " ";
			$Navigation .= "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
			$Navigation .= " ";
			$Navigation .= "View Per Page:";

			$LastSize = 0;
			foreach ($Sizes as $Size)
			{
				$Navigation .= " ";
				if ($this->PagerSize != $Size)
					$Navigation .= str_replace("SizeXX", $Size, $HREF);
				else
					$Navigation .= $Box1;
				$Navigation .= "$Size";
				if ($this->PagerSize != $Size)
					$Navigation .= "</a>";
				else
					$Navigation .= $Box2;
				$LastSize = $Size;
				if ($this->PagerRows < $LastSize)
					return($Navigation);;
			}
		}
		return($Navigation);
	}
}

?>
