<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class sequence
{
	function __construct($DB, $Table, $Sequence, $Keys, $SetCol = "", $SetID = "")
	{
		$this->DB = $DB;
		$this->Table = $Table;
		$this->Sequence = $Sequence;
		if (is_array($Keys))
			$this->Keys = $Keys;
		else
			$this->Keys = array($Keys);
		if (is_array($SetCol))
			$this->SetCol = $SetCol;
		elseif($SetCol)
			$this->SetCol = array($SetCol);
		else
			$this->SetCol = array();
		if (is_array($SetID))
			$this->SetID = $SetID;
		elseif($SetID)
			$this->SetID = array($SetID);
		else
			$this->SetID = array();
	}

	function GetLastID()
	{
		$Query  = "";
		$Query .= "SELECT MAX(";
		$Query .= $this->Sequence;
		$Query .= ") FROM ";
		$Query .= $this->Table;
		if (sizeof($this->SetCol))
		{
			$Sets = array();
			foreach ($this->SetCol as $i => $SetCol)
			{
				$Set  = "";
				$Set .= $this->SetCol[$i];
				$Set .= " = '";
				$Set .= addslashes($this->SetID[$i]);
				$Set .= "'";
				$Sets[] = $Set;
			}
			$Query .= " WHERE ".implode(" AND ", $Sets);
		}
		$MaxSeq = DBGet($this->DB, $Query);
		return($MaxSeq);
	}

	function WriteSequence($KeyValues, $Sequence)
	{
		$INQ = new DBInsert;
		foreach ($this->Keys as $Key);
			$INQ->SaveWhere($Key, $KeyValues[$Key]);
		if (sizeof($this->SetCol))
			foreach ($this->SetCol as $i => $SetCol)
				$INQ->SaveWhere($this->SetCol[$i], $this->SetID[$i]);
		$INQ->Save($this->Sequence, $Sequence);
		$INQ->Run($this->DB, $this->Table, "UPDATE");
		$INQ->EndQuery();
	}

	function Expand()
	{
		$Query  = "";
		$Query .= "UPDATE ";
		$Query .= $this->Table;
		$Query .= " SET ";
		$Query .= $this->Sequence;
		$Query .= " = (";
		$Query .= $this->Sequence;
		$Query .= " * 5)";
		if (sizeof($this->SetCol))
		{
			$Sets = array();
			foreach ($this->SetCol as $i => $SetCol)
			{
				$Set  = "";
				$Set .= $this->SetCol[$i];
				$Set .= " = '";
				$Set .= addslashes($this->SetID[$i]);
				$Set .= "'";
				$Sets[] = $Set;
			}
			$Query .= " WHERE ".implode(" AND ", $Sets);
		}
		DB_DoQuery($this->DB, $Query);
	}

	function GetKeys($Sequence)
	{
		$Query  = "";
		$Query .= "SELECT ";
		$Query .= implode(", ", $this->Keys);
		$Query .= " FROM ";
		$Query .= $this->Table;
		if (sizeof($this->SetCol))
		{
			$Sets = array();
			foreach ($this->SetCol as $i => $SetCol)
			{
				$Set  = "";
				$Set .= $this->SetCol[$i];
				$Set .= " = '";
				$Set .= addslashes($this->SetID[$i]);
				$Set .= "'";
				$Sets[] = $Set;
			}
			$Query .= " WHERE ".implode(" AND ", $Sets);
			$Query .= " AND ";
		}
		else
			$Query .= " WHERE ";
		$Query .= $this->Sequence;
		$Query .= " = ";
		$Query .= $Sequence;
		return(DBRow($this->DB, $Query));
	}

	function Promote($Sequence)
	{
		$this->Expand();
		$Sequence = ($Sequence * 5);
		$KeyValues = $this->GetKeys($Sequence);
		$Sequence = $Sequence - 6;
		$this->WriteSequence($KeyValues, $Sequence);
		$this->Compress();
	}

	function Demote($Sequence)
	{
		$this->Expand();
		$Sequence = ($Sequence * 5);
		$KeyValues = $this->GetKeys($Sequence);
		$Sequence = $Sequence + 6;
		$this->WriteSequence($KeyValues, $Sequence);
		$this->Compress();
	}

	function Insert($Sequence)
	{
		if ($Sequence > $this->GetLastID())
			return($Sequence);
		$this->Expand();
		$Sequence = ($Sequence * 5) - 1;
		return($Sequence);
	}

	function Compress()
	{
		$Query  = "";
		$Query .= "SELECT ";
		$Query .= implode(", ", $this->Keys);
		$Query .= ", ";
		$Query .= $this->Sequence;
		$Query .= " FROM ";
		$Query .= $this->Table;
		if (sizeof($this->SetCol))
		{
			$Sets = array();
			foreach ($this->SetCol as $i => $SetCol)
			{
				$Set  = "";
				$Set .= $this->SetCol[$i];
				$Set .= " = '";
				$Set .= addslashes($this->SetID[$i]);
				$Set .= "'";
				$Sets[] = $Set;
			}
			$Query .= " WHERE ".implode(" AND ", $Sets);
		}
		$Query .= " ORDER BY ";
		$Query .= $this->Sequence;
		$Q = new DBQuery;
		$Q->Run($this->DB, $Query);
		$i = 0;
		while ($Q->Next())
		{
			$i++;
			if ($Q->GetField($this->Sequence) != $i)
			{
				$KeyValues = array();
				foreach ($this->Keys as $Key)
					$KeyValues[$Key] = $Q->GetField($Key);
				$this->WriteSequence($KeyValues, $i);
			}
		}
		$Q->EndQuery();
	}

	function Append()
	{
		return($this->GetLastID() + 1);
	}
}

?>
