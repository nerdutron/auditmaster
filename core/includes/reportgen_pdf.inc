<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: reportgen_pdf.inc 9 2006-12-10 05:17:10Z scott                   $ //
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

include_once(ToIncludes("cw")."class.report.inc");

class reportgen_pdf
{
	function __construct()
	{
		$this->Data = array();
		$this->Columns = array();
		$this->Generated = 0;
		$this->PDF = new report;
		$this->PDF->ChooseHeaders("none");
		$this->PDF->StartPDF("a4", "landscape");
	}

	function NewTable($ID, $Type = "plain")
	{
		if ($this->Generated)
			return(0);
		$this->PDF->NewTable($ID);
		$this->PDF->TableOption($ID, "showLines", 0);
		$this->PDF->TableOption($ID, "showHeadings", 1);
		$this->PDF->TableOption($ID, "shaded", 0);
		$this->PDF->TableOption($ID, "fontSize", 10);
		$this->PDF->TableOption($ID, "titleFontSize", 12);
		$this->PDF->TableOption($ID, "xPos", "left");
		$this->PDF->TableOption($ID, "xOrientation", "right");
//		$this->PDF->TableOption($ID, "width", $this->PDF->ez["pageWidth"] - $this->PDF->ez["leftMargin"] - $this->PDF->ez["rightMargin"]);
		$this->PDF->TableOption($ID, "splitRows", 0);
		$this->PDF->TableOption($ID, "protectRows", 5); 
		$this->Data[$ID] = array();
	}

	function AddColumn($ID, $Label, $Align = "left")
	{
		if ($this->Generated)
			return(0);
		$this->PDF->TableAddColumn($ID, "<b>".$Label."</b>", $Align);
	}

	function AddRow($ID)
	{
		if ($this->Generated)
			return(0);
		if (sizeof($this->Data[$ID]))
		{
			$this->PDF->TableAddDataRow($ID, $this->Data[$ID]);
			$this->Data[$ID] = array();
		}
	}

	function AddData($ID, $Value, $Type = "text")
	{
		if ($this->Generated)
			return(0);
		switch (strtolower($Type))
		{
			case "hidden":
				break;
			case "number":
				$this->Data[$ID][] = number_format($Value);
				break;
			case "money":
				$this->Data[$ID][] = "\$".number_format($Value, 2);
				break;
			case "percent":
				$this->Data[$ID][] = number_format($Value, 2)."%";
				break;
			case "text":
			default:
				$this->Data[$ID][] = $Value;
				break;
		}
	}

	function AddTotal($ID, $Value, $Type = "text")
	{
		if ($this->Generated)
			return(0);
		switch (strtolower($Type))
		{
			case "hidden":
				break;
			case "number":
				$this->Data[$ID][] = "<b>".number_format($Value)."</b>";
				break;
			case "money":
				$this->Data[$ID][] = "<b>\$".number_format($Value, 2)."</b>";
				break;
			case "percent":
				$this->Data[$ID][] = "<b>".number_format($Value, 2)."%</b>";
				break;
			case "total":
				$this->Data[$ID][] = "<b>".$Value."</b>";
				break;
			case "text":
			default:
				$this->Data[$ID][] = "--";
				break;
		}
	}

	function PutTable($ID)
	{
		if ($this->Generated)
			return(0);
		$this->AddRow($ID);
		$this->PDF->ezText("", 10);
		$this->PDF->PublishTable($ID);
		$this->PDF->ezText("", 10);
	}

	function AddTitle1($Title)
	{
		if ($this->Generated)
			return(0);
		$this->PDF->DisplayTitle($Title, 1);
	}

	function AddTitle2($Title)
	{
		if ($this->Generated)
			return(0);
		$this->PDF->DisplayTitle($Title, 2);
	}

	function AddTitle3($Title)
	{
		if ($this->Generated)
			return(0);
		$this->PDF->DisplayTitle($Title, 3);
	}

	function AddTitle4($Title)
	{
		if ($this->Generated)
			return(0);
		$this->PDF->DisplayTitle($Title, 4);
	}

	function AddText($Text)
	{
		if ($this->Generated)
			return(0);
		$this->PDF->ezText($Text, 12);
	}

	function GetReport()
	{
		if ($this->Generated)
			return ($this->Report);
		$this->Generated = 1;
		$this->Report = $this->PDF->PDFGetData();
		return ($this->Report);
	}

	function MIME()
	{
		return ("application/pdf");
	}
}

?>
