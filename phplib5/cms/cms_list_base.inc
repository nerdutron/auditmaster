<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/
require_once(PHPLIB_DIR."datefield.inc");

class cms_list_base
{
	function _get($Order, $Status, $Content, $Item = 0)
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
		if ($Order == "SEQ")
			$Query .= "' ORDER BY order_id ASC";
		else
			$Query .= "' ORDER BY target ".$Order;
		if ($Item)
			$Data = DBRow(CMS_DB, $Query);
		else
		{
			$Data = array();
			$Q = new DBQuery;
			$Q->Run(CMS_DB, $Query);
			while ($Q->Next())
				$Data[] = $Q->GetArray();
			$Q->EndQuery();
			if ($Status == "EDIT")
				if (sizeof($Data) == 0)
					$Data = $this->_get("ACTIVE", $Content, $Item);
				else
					$Data["edit"] = 1;
		}
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

	function edit($Control, $Content, $Item = 0)
	{
		$CMSData = $this->get_edit($Content, $Item);
		if (!is_array($CMSData))
			$CMSData = $this->get_www($Content, $Item);
		echo "<h1>";
		if ($Item)
			echo "Edit <i>";
		else
			echo "Add <i>";
		echo DBGet(CMS_DB, "SELECT description FROM content_master WHERE content_id = ".($Content * 1));
		echo "</i> Item</h1>";
		field::PutFormStart("index.htm?Action=cms_popup");
		field::PutHidden("CMSData[status]", $CMSData["status"]);
		field::PutHidden("CMSData[content_id]", $Content);
		field::PutHidden("CMSData[item_id]", ($CMSData["item_id"] * 1));
		echo "<p><b>Title:</b> ";
		field::PutText("CMSData[title]", $CMSData["title"], 65);
		echo "</p>";
		if ($this->order() != "SEQ")
		{
			echo "<p><b>When:</b> ";
				if ($CMSData["target"] == "" || $CMSData["target"] == "0000-00-00 00:00:00")
			$UseDate = date("Y-m-d H:i:s");
			else
				$UseDate = $CMSData["target"];
			$DF = new DateField;
			$DF->GetDateSelect("year", date("Y"), date("Y") + 1);
			echo $DF->CreateDateField($UseDate, "month_short", "hour_12", $Control);
			echo "</p>";
		}
		field::PutArea("CMSData[data]", $CMSData["data"], 65, 15, "id=\"editor\"");
		field::PutFormSubmit("Save", "CMSData[action]");
		field::PutFormEnd();
	}

	function duplicate($Content)
	{
		$INQ = new DBInsert;
		$Query = "SELECT count(*) FROM content_items WHERE status = 'EDIT' AND content_id = ".$Content;
		$Count = DBGet(CMS_DB, $Query);
		if ($Count == 0)
		{
			$Query = "UPDATE content_items SET status = 'EDIT' WHERE status = 'ACTIVE' AND content_id = ".$Content;
			DB_DoQuery(CMS_DB, $Query);
			$Query = "SELECT * FROM content_items WHERE status = 'EDIT' AND content_id = ".$Content;
			$Q = new DBQuery;
			$Q->Run(CMS_DB, $Query);
			while ($Q->Next())
			{
				foreach ($Q->GetArray() as $Key => $Val)
				{
					switch ($Key)
					{
						case "item_id":
							break;
						case "status":
							$INQ->Save("status", "ACTIVE");
							break;
						default:
							$INQ->Save($Key, $Val);
							break;
					}
				}
				$INQ->Run(CMS_DB, "content_items", "INSERT");
				$INQ->EndQuery();
			}
			$Q->EndQuery();
			return (1);
		}
		return (0);
	}

	function save($CMSData)
	{
		$this->duplicate($CMSData["content_id"]);
		$INQ = new DBInsert;

		$INQ->Save("title", $CMSData["title"]);
		$INQ->Save("mime", "text/html");
		$INQ->Save("data", $CMSData["data"]);
		$INQ->Save("status", "EDIT");

		$DF = new DateField;
		$DF->AddDateField($CMSData["Yr"], $CMSData["Mo"], $CMSData["Dy"], $CMSData["Hr"], $CMSData["Mi"], $CMSData["Me"]);
		$INQ->Save("target", $DF->GetDate());
		$INQ->Save("updated", date("Y-m-d H:i:s"));
		if ($CMSData["item_id"])
		{
			$INQ->SaveWhere("content_id", $CMSData["content_id"]);
			$INQ->SaveWhere("item_id", $CMSData["item_id"]);
			$INQ->Run(CMS_DB, "content_items", "UPDATE");
		}
		else
		{
			if ($this->Order())
				$INQ->Save("order_id", (1 + DBGet(CMS_DB, "SELECT max(order_id) FROM content_items WHERE status = 'EDIT' AND content_id = ".$CMSData["content_id"])));
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

		$Query = "UPDATE content_items SET status = 'ACTIVE' WHERE status = 'EDIT' AND content_id = ".$CMSData["content_id"];
		DB_DoQuery(CMS_DB, $Query);
		return (1);
	}

	function delete($Content, $Item)
	{
		$this->duplicate($Content);
		$Query = "DELETE FROM content_items WHERE item_id = ".$Item;
		DB_DoQuery(CMS_DB, $Query);
		return (1);
	}

	function resequence($Content, $Items)
	{
		$Changed = $this->duplicate($Content);
		$INQ = new DBInsert;
		foreach ($Items as $Key => $Item)
		{
			$INQ->SaveWhere("item_id", $Item);
			$INQ->SaveWhere("content_id", $Content);
			$INQ->Save("order_id", ($Key + 1));
			$INQ->Run(CMS_DB, "content_items", "UPDATE");
			$INQ->EndQuery();
		}
		return ($Changed);
	}
}

?>
