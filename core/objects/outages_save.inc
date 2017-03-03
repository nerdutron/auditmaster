<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: outages_save.inc 8 2006-12-10 05:06:30Z scott                   $  //
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

require_once(ToObjects("core")."outages_edit.inc");
require_once(PHPLIB_DIR."ajax_date.inc");

class outages_save extends outages_edit
{
	function __construct()
	{
		parent::__construct();
		$this->Errors = array();
		if (!trim($this->Outage["description"]))
			$this->Errors[] = "You must provide a description.";
		if ($this->Outage["ScheduledDate"]." ".$this->Outage["ScheduledTime"] < date("Y-m-d H:i:s"))
			$this->Errors[] = "You cannot schedule an outage for a date in the past.";

		if (!sizeof($this->Errors))
		{
			$INQ = new DBInsert;
			$INQ->Save("user_id", $this->UserID());
			$INQ->Save("description", $this->Outage["description"]);
			$INQ->Save("scheduled", $this->Outage["ScheduledDate"]." ".$this->Outage["ScheduledTime"]);
			$INQ->Save("duration", $this->Outage["duration"]);
			if ($this->OutageID)
			{
				$INQ->SaveWhere("outage_id", $this->OutageID);
				$INQ->Run(ACC_DB, "scheduled_outages", "UPDATE");
			}
			else
			{
				$INQ->Save("status", "ACTIVE");
				$INQ->Run(ACC_DB, "scheduled_outages", "INSERT");
			}
			$INQ->EndQuery();
			$this->Redir($this->URL("outages"));
		}
	}

	function PageContent()
	{
		$this->ShowErrors($this->Errors);
		parent::PageContent();
	}
}

?>
