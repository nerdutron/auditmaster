<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: outages_edit.inc 8 2006-12-10 05:06:30Z scott                  $   //
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

require_once(ToObjects("core")."outages.inc");
require_once(PHPLIB_DIR."ajax_date.inc");

class outages_edit extends outages
{
	function __construct()
	{
		parent::__construct();
		$this->MakeL("OutageID", "FormData");

		if ($this->OutageID)
		{
			$Query  = "";
			$Query .= "SELECT * ";
			$Query .= "FROM scheduled_outages ";
			$Query .= "WHERE outage_id = ".$this->OutageID." ";
			$this->Outage = DBRow(ACC_DB, $Query);
			$Parts = explode(" ", $this->Outage["scheduled"]);
			$this->Outage["ScheduledDate"] = $Parts[0];
			$this->Outage["ScheduledTime"] = $Parts[1];
		}
		else
		{
			$this->Outage = array();
			$this->Outage["ScheduledDate"] = date("Y-m-d", strtotime("+ 1 day"));
			$this->Outage["ScheduledTime"] = date("H:00:00");
		}
		if (is_array($this->FormData))
			$this->Outage = array_merge($this->Outage, $this->FormData);

		$this->JXD = new ajax_date("index.php?Action=outages_edit&");
		$this->O_F = $this->JXD->define("FormData[ScheduledDate]", $this->Outage["ScheduledDate"], "Scheduled Start", "jS F Y");
		$this->JXD->response();
	}

	function Head()
	{
		$this->JXD->head();
	}

	function PageContent()
	{
		$Times = array();
		for ($h=0;$h<=23;$h++)
		{
			foreach (array(0,30) as $m)
			{
				$s = mktime($h, $m, 0);
				$Times[date("H:i:s", $s)] = date("h:ia", $s);
			}
		}

		$Duration = array();
		for ($h=0;$h<=23;$h++)
		{
			switch($h)
			{
				case 0:
					$Duration["00:00:00"] = "Unknown";
					break;
				case 1:
					$Duration["01:00:00"] = "1 hour";
					break;
				default:
					$Duration[date("H:i:s", mktime($h, 0, 0))] = $h." hours";
					break;
			}
		}

		echo "<h2>Scheduled Outages</h2>";

		$T = new CWTable;

		$T->AddHead("Description");
		$T->AddData("Unavailable due to ".field::GetText("FormData[description]", $this->Outage["description"], 64, 64), 4);
		$T->EndRow();

		$T->AddHead("Scheduled");
		$T->AddData($this->JXD->content($this->O_F));
		$T->AddData(field::GetDrop("FormData[ScheduledTime]", $this->Outage["ScheduledTime"], $Times));
		$T->AddHead("Duration");
		$T->AddData(field::GetDrop("FormData[duration]", $this->Outage["duration"], $Duration));
		$T->EndRow();

		$T->AddData(field::GetFormSubmit("Save"), 5);
		$T->EndRow();

		field::PutFormStart($this->URL("outages_save"));
		field::PutHidden("OutageID", ($this->OutageID * 1));
		$T->Put();
		field::PutFormEnd();
	}
}

?>
