<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: cw_table.inc 9 2006-12-10 05:17:10Z scott                        $ //
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
//////////////////////////////////////////////////////////////////////////////

require_once(PHPLIB_DIR."table.inc");

class CWTable extends Table
{
	function __construct($Class = "", $ID = "")
	{
		if ($Class == "")
			$Class = "outer";
		parent::__construct($Class, $ID);
	}

	function AlternateMouseOver()
	{
		$this->AlternateRows("row0m", "row1m");
	}

	function AlternateNormal()
	{
		$this->AlternateRows("row0", "row1");
	}

	function GetStyle($Align = "right", $Bold = 0)
	{
		$Style  = "style=\"text-align: ";
		if (in_array(strtolower($Align), array("left", "right". "center", "centre")))
			$Style .= $Align;
		else
			$Style .= "right";
		$Style .= "; ";
		if ($Bold)
			$Style .= "font-weight: bold; ";
		$Style .= "\"";
		return($Style);
	}

	function MakeStyle($Data, $Bold = 0)
	{
		$Style = "style=\"text-align: right; ";
		if ($Data < 0)
			$Style .= "color: #FF0000; ";
		else
			$Style .= "color: #000000; ";
		if ($Bold)
			$Style .= "font-weight: bold; ";
		$Style .= "\"";
		return($Style);
	}

	function MakeNumber($Data)
	{
		return(number_format($Data));
	}

	function MakePercent($Data, $Dec = 2)
	{
		return(number_format($Data, $Dec)."%");
	}

	function MakeMoney($Data)
	{
		return("\$".number_format($Data, 2));
	}

	function AddMoneyData($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->MakeStyle($Data);
		$Data = $this->MakeMoney($Data);
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}

	function AddMoneyTotal($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->MakeStyle($Data, 1);
		$Data = $this->MakeMoney($Data);
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}

	function AddPercentData($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle();
		$Data = $this->MakePercent($Data);
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}

	function AddPercentTotal($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle("", 1);
		$Data = $this->MakePercent($Data);
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}

	function AddPercent0Data($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle();
		$Data = $this->MakePercent($Data, 0);
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}

	function AddPercent0Total($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle("", 1);
		$Data = $this->MakePercent($Data, 0);
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}
	function AddPercent2Data($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$this->AddPercentData($Data, $Columns, $Rows, $Extra);
	}

	function AddPercent2Total($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$this->AddPercentTotal($Data, $Columns, $Rows, $Extra);
	}

	function AddNumberData($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle();
		$Data = $this->MakeNumber($Data);
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}

	function AddNumberTotal($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle("", 1);
		$Data = $this->MakeNumber($Data);
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}

	function AddCentreData($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle("center");
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}

	function AddCentreTotal($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle("center", 1);
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}

	function AddHeadRight($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle();
		$this->AddCell("th", $Data, $Columns, $Rows, $Style);
	}

	function AddDataRight($Data, $Columns = 1, $Rows = 1, $Extra = "")
	{
		$Style = $this->GetStyle();
		$this->AddCell("td", $Data, $Columns, $Rows, $Style);
	}
}

?>
