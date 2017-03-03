<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: palette.inc 9 2006-12-10 05:17:10Z scott                         $ //
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

require_once(CHARTING);

class palette
{
	function __construct()
	{
		foreach (palette::colours() as $Name => $Colour)
			define($Name, hexdec($Colour));
	}

	function colours()
	{
		$O	= "00";
		$L = "77";
		$M = "BB";
		$H = "DD";
		$A = "FF";

		$Palette = array();

		$Palette["BRIGHT_RED"]		= $A.$L.$L;
		$Palette["BRIGHT_GREEN"]	= $L.$A.$L;
		$Palette["BRIGHT_BLUE"]		= $L.$L.$A;
		$Palette["BRIGHT_AMBER"]	= $A.$A.$L;
		$Palette["BRIGHT_PURPLE"]	= $A.$L.$A;
		$Palette["BRIGHT_AQUA"]		= $L.$A.$A;

		$Palette["OFF_RED"]			= $A.$H.$H;
		$Palette["OFF_GREEN"]		= $H.$A.$H;
		$Palette["OFF_BLUE"]		= $H.$H.$A;
		$Palette["OFF_AMBER"]		= $A.$A.$H;
		$Palette["OFF_PURPLE"]		= $A.$H.$A;
		$Palette["OFF_AQUA"]		= $H.$A.$A;

		$Palette["PALE_RED"]		= $A.$M.$M;
		$Palette["PALE_GREEN"]		= $M.$A.$M;
		$Palette["PALE_BLUE"]		= $M.$M.$A;
		$Palette["PALE_AMBER"]		= $A.$A.$M;
		$Palette["PALE_PURPLE"]		= $A.$M.$A;
		$Palette["PALE_AQUA"]		= $M.$A.$A;

		$Palette["MID_RED"]			= $A.$L.$M;
		$Palette["MID_GREEN"]		= $M.$A.$L;
		$Palette["MID_BLUE"]		= $L.$M.$A;
		$Palette["MID_AMBER"]		= $A.$M.$L;
		$Palette["MID_PURPLE"]		= $M.$L.$A;
		$Palette["MID_AQUA"]		= $L.$A.$M;

		$Palette["FULL_RED"]		= $A.$O.$M;
		$Palette["FULL_GREEN"]		= $M.$A.$O;
		$Palette["FULL_BLUE"]		= $O.$M.$A;
		$Palette["FULL_AMBER"]		= $A.$M.$O;
		$Palette["FULL_PURPLE"]		= $M.$O.$A;
		$Palette["FULL_AQUA"]		= $O.$A.$M;

		$Palette["DARK_RED"]		= $M.$O.$L;
		$Palette["DARK_GREEN"]		= $L.$M.$O;
		$Palette["DARK_BLUE"]		= $O.$L.$M;
		$Palette["DARK_AMBER"]		= $M.$L.$O;
		$Palette["DARK_PURPLE"]		= $L.$O.$M;
		$Palette["DARK_AQUA"]		= $O.$M.$L;

		$Palette["WHITE"]			= $A.$A.$A;
		$Palette["OFF_GREY"]		= $H.$H.$H;
		$Palette["PALE_GREY"]		= $M.$M.$M;
		$Palette["DARK_GREY"]		= $L.$L.$L;
		$Palette["BLACK"]			= $O.$O.$O;

		return ($Palette);
	}

	function trans($Colour, $Percent = 50)
	{
		$Transparency = hexdec(dechex(round(256 / 100 * $Percent))."000000");
		return ($Transparency + $Colour);
	}

	function show()
	{
		$W = "<span style=\"color: #FFFFFF;\"><b>Bold</b> White</span>";
		$B = "<span style=\"color: #000000;\"><b>Bold</b> Black</span>";

		echo "<table>";
		$i = 0;
		foreach (palette::colours() as $Name => $Colour)
		{
			$i++;
			if ($i == 1)
				echo "<tr>";
			echo "<td>";
			echo "<b>".$Name."</b>";
			echo "<div style=\"background-color: #".$Colour.";\">".$W."<br />".$B."</div>0x";
			echo $Colour;
			echo "<br />&nbsp;";
			echo "</td>";
			if ($i == 6)
			{
				echo "</tr>";
				$i = 0;
			}
		}
		echo "</table>";
	}
}

new palette;

define("CHART_BACKGROUND",	OFF_AMBER);
define("CHART_BORDER",		BLACK);
define("CHART_TITLE_BAR",	DARK_AMBER);
define("CHART_TITLE_TEXT",	WHITE);
define("CHART_FILL",		WHITE);
define("CHART_AXES",		DARK_GREY);
define("CHART_ALERT",		DARK_RED);
define("CHART_AREA",		PALE_BLUE);

?>
