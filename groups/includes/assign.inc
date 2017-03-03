<?php
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// $Id:: header.txt 13 2006-12-10 13:10:12Z scott                         $ //
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


class Assign
{
	var $group_type;
	var $SaveFields = array("person_id", "group_id", "role_id", "note");

	function __construct($GroupID = 0, $PersonID = 0)
	{
		
		if ($GroupID)
			$this->group_id = $GroupID;

		if ($PersonID)
			$this->person_id = $PersonID;

		if ($this->group_id && $this->person_id)
			$this->_Load();
	}

	function _Load()
	{
		$Q = new DBQuery();

		$Q->RunOne(DATA_DB, "SELECT * FROM group_person WHERE group_id = ".$this->group_id." AND person_id = ".$this->person_id);

		$Data = $Q->GetArray();

		$Q->EndQuery();

		$this->LoadFromArray($Data);
	}

	function LoadFromArray($Data)
	{
		foreach ($Data as $ID => $Val)
			$this->$ID = $Val;
	}


	function GetArray()
	{
		$Result = array();

		foreach($this->SaveFields as $ID)
		{
			$Result[$ID] = $this->$ID;
		}

		return $Result;
	}


	function Save()
	{
		$INQ = new DBInsert;

		foreach($this->SaveFields as $ID)
		{
			$INQ->Save($ID, $this->$ID);
		}

		$INQ->Add("FUNCTION", "added", "now()");

		$INQ->Run(DATA_DB, "group_person", "INSERT");
		$this->person_id = $INQ->GetInsertID();	

		$INQ->EndQuery();
	}

	function GetFields($Fields)
	{
		
	}
}


function GetRoleList()
{
	$Q = new DBQuery();

	$Query = "SELECT * FROM role";

	$Q->Run(DATA_DB, $Query);
	
	$Result = array();
	$Result[""] = "";

	while ($Q->Next())
	{
		$Result[$Q->GetField("role_id")] = $Q->GetField("role_desc");
	}

	$Q->EndQuery();

	return $Result;
}


?>
