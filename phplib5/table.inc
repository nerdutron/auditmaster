<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class Table
{
	function __construct($Class = "", $ID = "", $Extra = "")
	{
		$this->EndTable();
		$this->Class = $Class;
		$this->ID = $ID;
		$this->Extra = $Extra;
	}

	function EndTable()
	{
		$this->Class = "";
		$this->ID = "";
		$this->Extra = "";

		// reset the array that stores the data rows
		$this->Data = array();
		// reset the data row count
		$this->Row = 0;
		$this->RowClass = array();
		$this->RowID = array();

		// alternate row presets
		$this->AlternateRows = array();
		$this->AlternateCount = 0;

		$this->Summary = "";
		$this->Caption = "";
		$this->Join = "\n";
	}

	function AlternateRows()
	{
		$this->AlternateRows = func_get_args();
		$this->AlternateCount = sizeof($this->AlternateRows);
	}

	function SetJoin($Join)
	{
		$this->Join = $Join;
	}

	function AddElement($Element)
	{
		if (!is_array($this->Data[$this->Row]))
			$this->Row++;
		elseif (sizeof($this->Data[$this->Row]))
			$this->Row++;
		$this->Data[$this->Row] = $Element;
		$this->EndRow();
	}

	function StartHead()
	{
		$this->AddElement("<thead>");
	}
	function EndHead()
	{
		$this->AddElement("</thead>");
	}
	function StartFoot()
	{
		$this->AddElement("<tfoot>");
	}
	function EndFoot()
	{
		$this->AddElement("</tfoot>");
	}
	function StartBody()
	{
		$this->AddElement("<tbody>");
	}
	function EndBody()
	{
		$this->AddElement("</tbody>");
	}

	function AddSummary($Summary)
	{
		$this->Summary = $Summary;
	}

	function AddCaption($Caption)
	{
		$this->Caption = $Caption;
	}

	//
	// adds a heading cell to the element
	//
	function AddHead($Data, $Columns = 1, $Rows = 1, $Extra = "", $Class = "")
	{
		$this->AddCell("th", $Data, $Columns, $Rows, $Extra, $Class);
	}

	//
	// adds a data cell to the element
	//
	function AddData($Data, $Columns = 1, $Rows = 1, $Extra = "", $Class = "")
	{
		$this->AddCell("td", $Data, $Columns, $Rows, $Extra, $Class);
	}

	function LoadRows($Data)
	{
		if ($this->Data[$this->Row] != "")
			$this->Row++;
		foreach ($Data as $Row)
		{
			foreach ($Row as $Cell)
				$this->AddCell("td", $Cell, 1, 1);
			$this->Row++;
		}
	}
	function LoadData($Data, $Type = "")
	{
		$Type = strtolower($Type);
		if (in_array($Type, array("head","foot","body")))
			$this->AddElement("<t".$Type.">");
		$this->LoadRows($Data);
		if (in_array($Type, array("head","foot","body")))
			$this->AddElement("</t".$Type.">");
	}

	//
	// actually performs the above additions (not a public function)
	//
	function AddCell($Type, $Data, $Columns, $Rows, $Extra = "", $Class = "")
	{
		$Cell = array();
		$Cell["Type"] = $Type;
		$Cell["Data"] = $Data;
		$Cell["Columns"] = $Columns * 1;
		$Cell["Rows"] = $Rows * 1;
		$Cell["Extra"] = $Extra;
		$Cell["Class"] = $Class;
		$this->Data[$this->Row][] = $Cell;
	}

	//
	// adds a spacer row to an element
	//
	function AddSpacer()
	{
		$this->AddCell("td", "&nbsp", 1, 1);
		$this->EndRow();
	}

	//
	// end the row by incrementing the counter
	//
	function EndRow($Class = "", $ID = "")
	{
		$this->RowClass[$this->Row] = trim($Class);
		$this->RowID[$this->Row] = trim($ID);
		if ($this->Data[$this->Row] != "")
			$this->Row++;
		$this->Data[$this->Row] = array();
	}

	//
	// writes all of the collected data as a tab element
	//
	function Get($Extra = "")
	{
		$Table = "";
		$Extra = trim($Extra);
		if ($Extra)
			$Extra .= " ";

		$Table .= "<table ".$Extra;

		if (trim($this->Class))
			$Table .= " class=\"".$this->Class."\"";
		if (trim($this->ID))
			$Table .= " id=\"".$this->ID."\"";
		if (trim($this->Extra))
			$Table .= " ".$this->Extra;
		if ($this->Summary)
			$Table .= " summary=\"".$this->Summary."\"";
		$Table .= ">".$this->Join;
		if ($this->Caption)
			$Table .= "<caption>".$this->Caption."</caption>".$this->Join;

		// loop through and put each data row, surrounded by <tr> tags
		$i = 0;
		foreach ($this->Data as $r => $Data)
		{
			$ClassAlt = "";
			$ClassRow = "";
			if (is_array($Data))
			{
				$ClassAlt = $this->AlternateRows[$i];
				$i++;
				if ($i == $this->AlternateCount)
					$i = 0;
				if (trim($this->RowClass[$r]))
					$ClassRow = $this->RowClass[$r];
				if (trim($this->RowID[$r]))
					$IDRow = $this->RowID[$r];
				else
					$IDRow = "";

				if (sizeof($Data))
				{
					if (($ClassRow) && ($IDRow))
						$Table .= "<tr id=\"".$IDRow."\" class=\"".$ClassRow."\">".$this->Join;
					else if ($ClassRow)
						$Table .= "<tr class=\"".$ClassRow."\">".$this->Join;
					else if ($IDRow)
						$Table .= "<tr id=\"".$IDRow."\">".$this->Join;
					elseif ($ClassAlt)
						$Table .= "<tr class=\"".$ClassAlt."\">".$this->Join;
					else
						$Table .= "<tr>".$this->Join;

					foreach ($Data as $Row)
					{
						$Class = "";
						if ($Row["Type"] == "td")
						{
							$Class = $ClassAlt;
							if ($ClassRow)
								$Class = $ClassRow;
						}
						if (trim($Row["Class"]))
							$Class = trim($Row["Class"]);

						$Table .= "<";
						$Table .= $Row["Type"];

						if ($Row["Columns"] > 1)
							$Table .= " colspan=\"".$Row["Columns"]."\"";
						if ($Row["Rows"] > 1)
							$Table .= " rowspan=\"".$Row["Rows"]."\"";
						if (trim($Row["Extra"]) !== "")
							$Table .= " ".trim($Row["Extra"]);
						if ($Class)
							$Table .= " class=\"".$Class."\"";
						$Table .= ">";

						if ($Row["Data"] === "")
							$Table .= "&nbsp;";
						else
							$Table .= $Row["Data"];

						$Table .= "</";
						$Table .= $Row["Type"];
						$Table .= ">";
						$Table .= $this->Join;
					}

					$Table .= "</tr>".$this->Join;
				}
			}
			else
				$Table .= $Data.$Join;
		}

		// close the table inside the tab
		$Table .= "</table>".$this->Join;

		return ($Table);
	}

	function Put($Extra = "")
	{
		echo $this->Get($Extra);
	}
}

?>
