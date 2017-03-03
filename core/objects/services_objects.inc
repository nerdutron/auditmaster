<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: services_objects.inc 8 2006-12-10 05:06:30Z scott                $ //
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

require_once(ToObjects("core")."services_list.inc");

class services_objects extends services_list
{
	function __construct()
	{
		parent::__construct();
		$this->MakeL("ViewID", "Object");
		$Query = "SELECT * FROM services WHERE service_id = ".$this->ViewID;
		$this->Service = DBRow(ACC_DB, $Query);
		$this->SubTitle = $this->Service["service_code"].": ".$this->Service["service_name"];

		$DIR = ToObjects("core");
		$this->Objects = array();
		foreach ($this->FindObjects($DIR) as $Object)
			$this->Objects[$Object] = $Object;

		$Query = "SELECT admin_id, description FROM admin_levels ORDER BY admin_id";
		$Q = new DBQuery;
		$Q->Run(ACC_DB, $Query);
		$this->Types = array();
		while ($Q->Next())
			$this->Types[$Q->GetField("admin_id")] = $Q->GetField("description");
		$Q->EndQuery();
	}

	function FindObjects($DIR, $Files = "", $Prefix = "")
	{
		if (!is_array($Files))
			$Files = array();
		foreach(scandir($DIR) as $File)
		{
			if (substr($File, 0, 1) != ".")
			{
				if (is_dir($DIR.$File) && $Prefix == "")
					$Files = $this->FindObjects($DIR.$File."/", $Files, $File."|");
				if (is_file($DIR.$File))
					$Files[] = $Prefix.substr($File, 0, strlen($File) - 4);
			}
		}
		return ($Files);
	}

	function PageContent()
	{

		$Query = "SELECT * FROM service_objects WHERE service_id = ".$this->ViewID;
		$Q = new DBQuery;
		$Q->Run(ACC_DB, $Query);
		$T = new CWTable;
		$T->AlternateMouseOver();
		$T->AddHead("Object");
		$T->AddHead("Status");
		$T->AddHead("Service Sub Section");
		$T->AddHead("Access Level Req.");
		$T->AddHead("<a href=\"".$this->URL("services_objects", array("ViewID"=>$this->ViewID))."\">new/all</a>", 2);
		$T->EndRow();
		while ($Q->Next())
		{
			$Parts = explode("|", $Q->GetField("secure_object"));
			if ($Parts[1])
				$Path = ToObjects($Parts[0]).$Parts[1].".inc";
			else
				$Path = ToObjects("core").$Parts[0].".inc";
			if (is_file($Path))
				$Status = "Valid";
			else
				$Status = "Missing";

			if ($this->Object == $Q->GetField("secure_object"))
			{
				$Edit = $Q->GetArray();
				if ($Status != "Valid")
					$Edit["missing"] = 1;
//				if (!$Q->GetField("sub_section"))
//					$Edit["root"] = 1;
			}
			else
			{
				$T->AddData($Q->GetField("secure_object"));
				$T->AddData($Status);
				if ($Q->GetField("is_base") == "Y")
					$T->AddData("<i>Service Root Object</i>");
				else
					$T->AddData($Q->GetField("sub_section"));
				$T->AddData($this->Types[$Q->GetField("admin_required")]);
				$URL = $this->URL("services_objects", array("ViewID"=>$this->ViewID,"Object"=>$Q->GetField("secure_object")));
				$T->AddData("<a href=\"".$URL."\">edit</a>");
				$URL = $this->URL("services_objects_del", array("ViewID"=>$this->ViewID,"Object"=>$Q->GetField("secure_object")));
				if ($Q->GetField("is_base") == "N" || ($Q->GetField("is_base") == "Y" && $Q->NumRows() == 1))
					$T->AddData($this->ActionLink($URL, "Are you sure you want to remove security from ".$Q->GetField("secure_object")."?"));
				else
					$T->AddData("&nbsp;");
				$T->EndRow();
			}
		}

		if (!is_array($Edit))
			$Edit = array();
		if (is_array($this->Edit))
			$Edit = array_merge($Edit, $this->Edit);

		if ($Edit["missing"])
			$this->Objects[$Edit["secure_object"]] = $Edit["secure_object"]." [not found]";
		else
			$this->Objects[""] = "";
		$T->AddData(field::GetDrop("Edit[secure_object]", $Edit["secure_object"], $this->Objects), 2);
		if ($Q->NumRows() == 0 || $Edit["is_base"] == "Y")
			$T->AddData("Service Root Object".field::GetHidden("Edit[is_base]", "Y"));
		else
			$T->AddData(field::GetText("Edit[sub_section]", $Edit["sub_section"], 32, 64));
		$T->AddData(field::GetDrop("Edit[admin_required]", $Edit["admin_required"], $this->Types));
		$T->AddData(field::GetFormSubmit("Save"), 2);
		$T->EndRow();

		field::PutFormStart($this->URL("services_objects_save"));
		field::PutHidden("ViewID", $this->ViewID);
		field::PutHidden("Object", $this->Object);
		$T->Put();
		field::PutFormEnd();
		$Q->EndQuery();
		echo "<p><a href=\"".$this->URL("services_list")."\">&lt;&lt; Back to Services List</a></p>";
	}
}

?>
