<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

class cms_item_base
{
	function _get($Status, $Content, $Item = 0)
	{
		$Query  = "";
		$Query .= "SELECT * FROM content_items WHERE content_id = ";
		$Query .= $Content;
		if ($Item)
		{
			$Query .= " AND item_id = ";
			$Query .= $Item;
		}
		$Query .= " AND STATUS = '";
		$Query .= $Status;
		$Query .= "'";
		$Data = DBRow(CMS_DB, $Query);
		if ($Status == "EDIT")
			if (sizeof($Data) == 0)
				$Data = $this->_get("ACTIVE", $Content, $Item);
			else
				$Data["edit"] = 1;
		return ($Data);
	}

	function get_www($Content, $Item = 0)
	{
		$this->Data = $this->_get("ACTIVE", $Content, $Item);
		return($this->Data);
	}

	function get_edit($Content, $Item = 0)
	{
		$this->Data = $this->_get("EDIT", $Content, $Item);
		return($this->Data);
	}

	function edit($Content, $Item = 0)
	{
		$CMSData = $this->get_edit($Content, $Item);
		echo "<h1>Edit <i>";
		echo DBGet(CMS_DB, "SELECT description FROM content_master WHERE content_id = ".($Content * 1));
		echo "</i> Content</h1>";
		field::PutFormStart("index.htm?Action=cms_popup");
		field::PutHidden("CMSData[status]", $CMSData["status"]);
		field::PutHidden("CMSData[content_id]", $Content);
		field::PutHidden("CMSData[item_id]", ($CMSData["item_id"] * 1));
		field::PutArea("CMSData[data]", $CMSData["data"], 65, 20, "id=\"editor\"");
		echo "<br />";
		field::PutFormSubmit("Save", "CMSData[action]");
		field::PutFormEnd();
	}

	function save($CMSData)
	{
		$INQ = new DBInsert;
		$INQ->Save("title", "");
		$INQ->Save("mime", "text/html");
		$INQ->Save("data", $CMSData["data"]);
//		$INQ->Save("data", base64_encode(gzcompress($CMSData["data"])));
		$INQ->Save("status", "EDIT");
		$INQ->Save("target", "0000-00-00 00:00:00");
		$INQ->Save("updated", date("Y-m-d H:i:s"));
		if (($CMSData["status"] == "EDIT") && $CMSData["item_id"])
		{
			$INQ->SaveWhere("content_id", $CMSData["content_id"]);
			$INQ->SaveWhere("item_id", $CMSData["item_id"]);
			$INQ->Run(CMS_DB, "content_items", "UPDATE");
		}
		else
		{
			$INQ->Save("content_id", $CMSData["content_id"]);
			$INQ->Run(CMS_DB, "content_items", "INSERT");
		}
		$INQ->EndQuery();
		return (1);
	}

	function launch($CMSData)
	{
		$Query = "DELETE FROM content_items WHERE status = 'ACTIVE' AND content_id = ".$CMSData["content_id"];
		DB_DoQuery(CMS_DB, $Query);

		$INQ = new DBInsert;
		$INQ->Save("status", "ACTIVE");
		$INQ->Save("updated", date("Y-m-d H:i:s"));
		$INQ->SaveWhere("content_id", $CMSData["content_id"]);
		$INQ->SaveWhere("item_id", $CMSData["item_id"]);
		$INQ->Run(CMS_DB, "content_items", "UPDATE");
		$INQ->EndQuery();
		return (1);
	}

//	function list() // not always necessary
//	{
//	}

//	function delete() // not always necessary
//	{
//	}
}

?>
