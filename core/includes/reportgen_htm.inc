<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: reportgen_htm.inc 9 2006-12-10 05:17:10Z scott                   $ //
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

require_once(ToIncludes("cw")."cw_table.inc");

class reportgen_htm
{
	function __construct()
	{
		$this->Tables = array();
		$this->Columns = array();
		$this->Report = "";
	}

	function NewTable($ID, $Type = "plain")
	{
		$this->Tables[$ID] = new CWTable();
	}

	function AddColumn($ID, $Label, $Align = "left")
	{
		if ($Align == "right")
			$this->Tables[$ID]->AddHeadRight($Label);
		else
			$this->Tables[$ID]->AddHead($Label);
	}

	function AddRow($ID)
	{
		$this->Tables[$ID]->EndRow();
	}

	function AddData($ID, $Data, $Type = "text")
	{
		switch (strtolower($Type))
		{
			case "hidden":
				break;
			case "number":
				$this->Tables[$ID]->AddNumberData($Data);
				break;
			case "money":
				$this->Tables[$ID]->AddMoneyData($Data);
				break;
			case "percent":
				$this->Tables[$ID]->AddPercentData($Data);
				break;
			case "centre":
				$this->Tables[$ID]->AddCentreData($Data);
				break;
			case "text":
			default:
				$this->Tables[$ID]->AddData($Data);
				break;
		}
	}

	function AddTotal($ID, $Data, $Type = "text")
	{
		switch (strtolower($Type))
		{
			case "hidden":
				break;
			case "number":
				$this->Tables[$ID]->AddNumberTotal($Data);
				break;
			case "money":
				$this->Tables[$ID]->AddMoneyTotal($Data);
				break;
			case "percent":
				$this->Tables[$ID]->AddPercentTotal($Data);
				break;
			case "centre":
				$this->Tables[$ID]->AddCentreData("<b>".$Data."</b>");
				break;
			case "total":
				$this->Tables[$ID]->AddData("<b>".$Data."</b>");
				break;
			case "text":
			default:
				$this->Tables[$ID]->AddData("--");
				break;
		}
	}

	function PutTable($ID)
	{
		$this->Report .= $this->Tables[$ID]->Get($ID);
	}

	function AddTitle1($Title)
	{
		$this->Report .= "<h1>".$Title."</h1>";
	}

	function AddTitle2($Title)
	{
		$this->Report .= "<h2>".$Title."</h2>";
	}

	function AddTitle3($Title)
	{
		$this->Report .= "<h3>".$Title."</h3>";
	}

	function AddTitle4($Title)
	{
		$this->Report .= "<h4>".$Title."</h4>";
	}

	function AddText($Text)
	{
		$this->Report .= "<p>".nl2br($Text)."</p>";
	}

	function GetReport()
	{
		return ($this->Report);
	}

	function MIME()
	{
		return ("text/html");
	}
}

?>
