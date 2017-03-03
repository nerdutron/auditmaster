<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: reportgen.inc 9 2006-12-10 05:17:10Z scott                       $ //
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

require_once(PHPLIB_DIR."mailq.inc");

class reportgen
{
	function __construct()
	{
		$this->Reset();
	}

	function Reset()
	{
		$this->TableDef = array();
		$this->ReportList = array();
		$this->ReportClass = array();
		$this->TableGroupTotals = array();
		$this->TableGrandTotals = array();
		$this->TableGrouping = array();
		$this->LastGroupID = array();
	}

	function AddReport($Type)
	{
		$Type = strtolower($Type);
		if (!in_array($Type, $this->ReportList))
		{
			$File = ToIncludes("cw")."reportgen_".$Type.".inc";
			if (is_file($File))
			{
				require_once($File);
				$this->ReportList[] = $Type;
				$Class = "reportgen_".$Type;
				$this->ReportClass[$Type] = new $Class;
			}
		}
	}

	function NewTable($ID, $Style = "plain")
	{
		$this->TableDef[$ID] = array();
		$this->TableData[$ID] = array();
		$this->TableGroupTotals[$ID] = array();
		$this->TableGrandTotals[$ID] = array();
		$this->TableGrouping[$ID] = 0;
		$this->LastGroupID[$ID] = "";
		foreach ($this->ReportList as $Report)
			$this->ReportClass[$Report]->NewTable($ID);
	}

	function AddColumn($ID, $Column)
	{
		$this->TableDef[$ID][] = $Column;
		foreach ($this->ReportList as $Report)
		{
			switch (strtolower($Column["type"]))
			{
				case "hidden":
					break;
				case "number":
				case "money":
				case "percent":
					$this->ReportClass[$Report]->AddColumn($ID, $Column["label"], "right");
					break;
				case "centre":
					$this->ReportClass[$Report]->AddColumn($ID, $Column["label"], "centre");
					break;
				case "text":
				default:
					$this->ReportClass[$Report]->AddColumn($ID, $Column["label"], "left");
					break;
			}
		}
		if ($Column["group"])
			$this->TableGrouping[$ID] = 1;
	}

	function AddColumns($ID, $Columns)
	{	
		foreach ($Columns as $Column)
			$this->AddColumn($ID, $Column);
	}

	function AddRow($ID)
	{
		foreach ($this->ReportList as $Report)
			$this->ReportClass[$Report]->AddRow($ID);
	}

	function AddData($ID, $Data)
	{
		foreach ($Data as $Row)
		{
			if ($this->TableGrouping[$ID])
			{
				$GroupID = array();
				foreach ($this->TableDef[$ID] as $c => $Column)
				{
					if ($Column["group"])
						$GroupID[] = $Row[$c];
				}
				$GroupID = implode("|", $GroupID);
				if (($GroupID != $this->LastGroupID[$ID]) && $this->LastGroupID[$ID])
				{
					$this->_ProcessTotals($ID, $this->TableGroupTotals[$ID], "Group Total");
					$this->TableGroupTotals[$ID] = "";
				}

				$this->LastGroupID[$ID] = $GroupID;
			}

			$this->AddRow($ID);
			foreach ($this->TableDef[$ID] as $c => $Column)
			{
				if ($Column["type"] != "hidden")
					foreach ($this->ReportList as $Report)
						$this->ReportClass[$Report]->AddData($ID, $Row[$c], $Column["type"]);
				if (in_array(strtolower($Column["type"]), array("number", "money", "percent")) && $Column["total"])
				{
					$this->TableGroupTotals[$ID][$c] = $this->TableGroupTotals[$ID][$c] + $Row[$c];
					$this->TableGrandTotals[$ID][$c] = $this->TableGrandTotals[$ID][$c] + $Row[$c];
				}
			}
		}
	}

	function _ProcessTotals($ID, $Totals, $Set)
	{
		if (is_array($Totals))
		{
			if (sizeof($Totals))
			{
				foreach ($this->ReportList as $Report)
				{
					$this->AddRow($ID);
					foreach ($this->TableDef[$ID] as $c => $Column)
					{
						if (strtolower($Column["type"]) != "hidden")
						{
							if (in_array(strtolower($Column["type"]), array("number", "money", "percent")) && $Column["total"])
								$this->ReportClass[$Report]->AddTotal($ID, $Totals[$c], $Column["type"]);
							else
							{
								if ($c)
									$this->ReportClass[$Report]->AddTotal($ID, "", "text");
								else
									$this->ReportClass[$Report]->AddTotal($ID, $Set, "total");
							}
						}
					}
				}
			}
		}
	}

	function PutTable($ID)
	{
		if ($this->TableGrouping[$ID])
			$this->_ProcessTotals($ID, $this->TableGroupTotals[$ID], "Group Total");
		$this->_ProcessTotals($ID, $this->TableGrandTotals[$ID], "Grand Total");
		foreach ($this->ReportList as $Report)
		{
			$this->ReportClass[$Report]->PutTable($ID);
		}
	}

	function AddTitle1($Title)
	{
		foreach ($this->ReportList as $Report)
			$this->ReportClass[$Report]->AddTitle1($Title);
	}

	function AddTitle2($Title)
	{
		foreach ($this->ReportList as $Report)
			$this->ReportClass[$Report]->AddTitle2($Title);
	}

	function AddTitle3($Title)
	{
		foreach ($this->ReportList as $Report)
			$this->ReportClass[$Report]->AddTitle3($Title);
	}

	function AddTitle4($Title)
	{
		foreach ($this->ReportList as $Report)
			$this->ReportClass[$Report]->AddTitle4($Title);
	}

	function AddText($Text)
	{
		foreach ($this->ReportList as $Report)
			$this->ReportClass[$Report]->AddText($Text);
	}

	function _ChooseType($Type = "")
	{
		$Type = strtolower($Type);
		if (in_array($Type, $this->ReportList))
			$Report = $Type;
		elseif (sizeof($this->ReportList) == 1)
			$Report = $this->ReportList[0];
		elseif (sizeof($this->ReportList) == 0)
			exit("Error: No generated reports are available for collection.");
		elseif (sizeof($this->ReportList) > 1)
			exit("Error: Multiple generated reports are available for collection: please specify.");
		return ($Report);
	}

	function MIME($Type = "")
	{
		$Type = strtolower($Type);
		$Report = $this->_ChooseType($Type);
		return($this->ReportClass[$Report]->MIME());
	}

	function GetReport($Type = "")
	{
		$Type = strtolower($Type);
		$Report = $this->_ChooseType($Type);
		return($this->ReportClass[$Report]->GetReport());
	}

	function MakeFileName($Description, $MIME)
	{
		$Parts = explode("/", $MIME);
		$EXT = array_pop($Parts);
		$Description = str_replace(array("'","\"","/","\\", "\r"), "", $Description);
		$Description = str_replace(array(" ","\n","\t"), "_", $Description);
		return($Description.".".$EXT);
	}

	function CacheReport($OwnerID, $Description, $Type = "")
	{
		$Type = strtolower($Type);
		$Report = $this->_ChooseType($Type);
		$INQ = new DBInsert;
		$INQ->Save("owner_id", $OwnerID);
		$INQ->Save("description", $Description);
		$INQ->Save("data", gzcompress($this->GetReport($Report)));
		$INQ->Save("mime", $this->MIME($Report));
		$INQ->Save("expiry", date("Y-m-d H:i:s", strtotime("+ 2 hours")));
		$INQ->Run(CW_DB, "report_cache", "INSERT");
		$CacheID = $INQ->GetInsertID();
		$INQ->EndQuery();
		return($CacheID);
	}

	function SendReport($Recipient, $Sender, $Subject, $Body, $Description, $Type = "")
	{
		$Type = strtolower($Type);
		$Report = $this->_ChooseType($Type);
		$E = new MailQ;
		$E->AddEmail($Subject, $Body, $Sender, "REPORTGEN", date("Y-m-d H:i:s", strtotime("+ 1 month")));
		$E->AddRecipients($Recipient);
		$MIME = $this->MIME($Report);
		$E->AddDocument($this->GetReport($Report), $MIME, $this->MakeFileName($Description, $MIME));
		$E->Send();
	}

	function GetFromCache($CacheID, $OwnerID)
	{
		$Query = "SELECT data, description, mime FROM report_cache WHERE cache_id = ".($CacheID * 1)." AND owner_id = ".($OwnerID * 1);
		$Report = DBRow(CW_DB, $Query);
		$Report["data"] = gzuncompress($Report["data"]);
		return ($Report);
	}

	function DisplayFromCache($CacheID, $OwnerID)
	{
		$Report = $this->GetFromCache($CacheID, $OwnerID);
		Header("Content-type: ".$Report["mime"]);
		echo $Report["data"];
		exit;
	}

	function DownloadFromCache($CacheID, $OwnerID)
	{
		$Report = $this->GetFromCache($CacheID, $OwnerID);
		Header("Content-type: ".$Report["mime"]);
		Header("Content-Length: ".strlen($Report["data"]));
		Header("Content-Disposition: attachment; filename=".$this->MakeFileName($Report["description"], $Report["mime"]));
		echo $Report["data"];
		exit;
	}

	function SendFromCache($CacheID, $OwnerID, $Recipient, $Sender, $Subject, $Body)
	{
		$Report = $this->GetFromCache($CacheID, $OwnerID);
		$E = new MailQ;
		$E->AddEmail($Subject, $Body, $Sender, "REPORTGEN", date("Y-m-d H:i:s", strtotime("+ 1 month")));
		$E->AddRecipients($Recipient);
		$E->AddDocument($Report["data"], $Report["mime"], $this->MakeFileName($Report["description"], $Report["mime"]));
		$E->Send();
	}

	function DataFetch($DB, $Query)
	{
		$AccountID = 0;
		$Data = array();
		$Q = new DBQuery;
		$Q->Run($DB, $Query);
		while ($Q->Next())
		{
			if (!sizeof($Data))
				$Data[] = array_keys($Q->GetArray());
			$Row = array();
			foreach ($Q->GetArray() as $Field)
				$Row[] = $Field;
			$Data[] = $Row;
		}
		$Q->EndQuery();
		return ($Data);
	}
}

?>
