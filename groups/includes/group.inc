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


class Group
{
	var $group_type;
	var $SaveFields = array("name", "location_desc", "location", "status", "group_type");

	function __construct($GroupID = 0)
	{
		if ($GroupID)
			$this->_Load($GroupID);
		else
			$this->group_id = 0;
	}

	function _Load($GroupID)
	{
		$Query = "SELECT * FROM groups WHERE group_id = $GroupID";

		$Data = DBRow(DATA_DB, $Query);

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

	function SaveGroupType($Name, $GroupTypeID = 0)
	{
		$INQ = new DBInsert;

		$INQ->Save("type_desc", $Name);

		if ($GroupTypeID)
		{
			$INQ->SaveWhere("type_id", $GroupTypeID);
			$INQ->Run(DATA_DB, "group_type", "UPDATE");
		}
		else
		{
			$INQ->Run(DATA_DB, "group_type", "INSERT");
			$this->group_type_id = $INQ->GetInsertID();	
		}

		$INQ->EndQuery();
	}

	function Save()
	{
		$INQ = new DBInsert;

		foreach($this->SaveFields as $ID)
		{
			$INQ->Save($ID, $this->$ID);
		}

		if ($this->group_id)
		{
			$INQ->SaveWhere("group_id", $this->group_id);
			$INQ->Run(DATA_DB, "groups", "UPDATE");
		}
		else
		{
			$INQ->Run(DATA_DB, "groups", "INSERT");
			$this->person_id = $INQ->GetInsertID();	
		}

		$INQ->EndQuery();
	}

	function GetFields($Fields)
	{
		
	}
}


function GetGroupTypeList()
{
	$Q = new DBQuery();

	$Query = "SELECT * FROM group_type";

	$Q->Run(DATA_DB, $Query);
	
	$Result = array();
	$Result[""] = "";

	while ($Q->Next())
	{
		$Result[$Q->GetField("type_id")] = $Q->GetField("type_desc");
	}

	$Q->EndQuery();

	return $Result;
}


function GetGroupList($GroupType = 0)
{
	$Q = new DBQuery();

	$Query = "SELECT * FROM groups";

	if ($GroupType)
		$Query .= " WHERE group_type = $GroupType";

	$Q->Run(DATA_DB, $Query);
	
	$Result = array();
	$Result[""] = "";

	while ($Q->Next())
	{
		$Result[$Q->GetField("group_id")] = $Q->GetField("name");
	}

	$Q->EndQuery();

	return $Result;
}

function GetGroupDetailList($GroupType = 0, $GroupID = 0)
{
	$Q = new DBQuery();

	$Query = "SELECT * FROM groups g, group_status gs, group_type gt WHERE g.status = gs.status_id AND g.group_type = gt.type_id";

	if ($GroupType)
		$Query .= " AND group_type = $GroupType";
	else if ($GroupID)
		$Query .= " AND group_id = $GroupID";

	$Query .= " ORDER BY name";

	$Q->Run(DATA_DB, $Query);
	
	$Result = array();

	while ($Q->Next())
	{
		$Result[$Q->GetField("group_id")] = $Q->GetArray();
	}

	$Q->EndQuery();

	return $Result;
}


function GetGroupStatusList()
{
	$Q = new DBQuery();

	$Query = "SELECT * FROM group_status";

	$Q->Run(DATA_DB, $Query);
	
	$Result = array();
	$Result[""] = "";

	while ($Q->Next())
	{
		$Result[$Q->GetField("status_id")] = $Q->GetField("status_desc");
	}

	$Q->EndQuery();

	return $Result;
}


function GetGroupMembersFull($GroupID)
{
	$Q = new DBQuery();

	$Query = "SELECT g.person_id, first_name, surname, role_desc, note, added FROM group_person g, person p, role r";
	$Query .= " WHERE g.person_id = p.person_id AND g.role_id = r.role_id AND group_id = $GroupID";
	$Query .= " ORDER BY surname, first_name";

	$Q->Run(DATA_DB, $Query);
	
	$Result = array();

	while ($Q->Next())
	{
		$Row = array();

		$Row["person_id"] = $Q->GetField("person_id");
		$Row["person_name"] = $Q->GetField("first_name")." ".$Q->GetField("surname");
		$Row["person_role"] = $Q->GetField("role_desc");
		$Row["note"] = $Q->GetField("note");
		$Row["added"] = $Q->GetField("added");

		$Result[] = $Row;
	}

	$Q->EndQuery();

	return $Result;
}


function DeleteMember($GroupID, $PersonID)
{
	$Query = "DELETE FROM group_person WHERE group_id = $GroupID AND person_id = $PersonID";

	DB_DoQuery(DATA_DB, $Query);

}


?>
