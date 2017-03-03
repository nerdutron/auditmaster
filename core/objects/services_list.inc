<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: services_list.inc 8 2006-12-10 05:06:30Z scott                   $ //
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

class services_list extends system_administration
{
	function __construct()
	{
		parent::__construct();
		$this->Title = "Services List";
		$this->MakeL("EditID");
		$this->Public = array("N"=>"No", "Y"=>"Yes");
	}

	function PageContent()
	{
		$T = new CWTable;
		$T->AlternateRows("row0", "row1");

		$T->AddHead("ID");
		$T->AddHead("Code");
		$T->AddHead("Name");
		$T->AddHead("Public");
		$T->AddHead("Maintain", 2);
		$T->EndRow();

		$Query = "SELECT * FROM services ORDER BY service_name";
		$Q = new DBQuery();
		$Q->Run(ACC_DB, $Query);
		while ($Q->Next())
		{
			$ID = $Q->GetField("service_id");
			if ($ID == $this->EditID)
				$Data = $Q->GetArray();
			else
			{
				$T->AddData($ID);
				$T->AddData($Q->GetField("service_code"));
				$T->AddData($Q->GetField("service_name"));
				$T->AddData($this->Public[$Q->GetField("publish")]);
				$T->AddData("<a href=\"".$this->URL("services_list", array("EditID"=>$ID))."\">edit</a>");
				$T->AddData("<a href=\"".$this->URL("services_objects", array("ViewID"=>$ID))."\">objects</a>");
				$T->EndRow();
			}
		}
		$Q->EndQuery();

		$T->AddHead($Data["service_id"] * 1);
		$T->AddHead(field::GetText("FormData[code]", $Data["service_code"], 8, 8));
		$T->AddHead(field::GetText("FormData[name]", $Data["service_name"], 32, 128));
		$T->AddHead(field::GetDrop("FormData[publish]", $Data["publish"], $this->Public));
		$T->AddHead(field::GetFormSubmit("Save"), 2);
		$T->EndRow();

		field::PutFormStart($this->URL("services_save"));
		if ($Data["service_id"] * 1)
			field::PutHidden("EditID", $Data["service_id"]);
		$T->Put();
		field::PutFormEnd();
	}
}

?>
