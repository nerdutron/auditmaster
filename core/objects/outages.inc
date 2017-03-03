<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: outages.inc 8 2006-12-10 05:06:30Z scott                       $   //
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

require_once(ToObjects("core")."system_administration.inc");

class outages extends system_administration
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "Scheduled Outages";
	}

	function PageContent()
	{
		$Query  = "";
		$Query .= "SELECT * ";
		$Query .= "FROM scheduled_outages ";
		$Query .= "WHERE status = 'ACTIVE' ";
		$Query .= "ORDER BY scheduled ";

		echo "<h2>Scheduled Outages</h2>";

		$T = new CWTable;
		$T->AddHead("Description");
		$T->AddHead("From");
		$T->AddHead("Duration");
		$T->AddHead("Status");
		$T->AddHead("&nbsp;", 2);
		$T->EndRow();

		$T->AlternateMouseOver();
		$Q = new DBQuery;
		$Q->Run(ACC_DB, $Query);
		while ($Q->Next())
		{
			$ID = $Q->GetField("outage_id");
			$T->AddData("Unavailable due to <i>".$Q->GetField("description")."</i>");
			$T->AddData(date("d/m/Y h:ia", CreateTimeStamp($Q->GetField("scheduled"))));
			$T->AddData($Q->GetField("duration"));
			$T->AddData($Q->GetField("status"));
			if ($Q->GetField("status") == "ACTIVE")
			{
				if ($Q->GetField("scheduled") < date("Y-m-d H:i:s"))
					$T->AddData("<a href=\"".$this->URL("outages_close", array("OutageID"=>$ID))."\">CLOSE</a>", 2);
				else
				{
					$T->AddData("<a href=\"".$this->URL("outages_edit", array("OutageID"=>$ID))."\">Edit</a>");
					$T->AddData($this->ActionLink($this->URL("outages_del", array("OutageID"=>$ID)), "Are you sure you want to delete this Scheduled Outage?"));
				}
			}
			else
					$T->AddData("&nbsp;", 2);
			$T->EndRow();
		}
		$Q->EndQuery();
		$T->AddData("<a href=\"".$this->URL("outages_edit")."\">Schedule Outage</a>", 6);
		$T->EndRow();
		$T->Put();
	}
}

?>
