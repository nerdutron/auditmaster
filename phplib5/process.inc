<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."mailq.inc");
require_once(PHPLIB_DIR."script.inc");
require_once(PHPLIB_DIR."database.inc");
require_once(PHPLIB_DIR."stopwatch.inc");

class Process
{
	function __construct($HostName = "primary", $HostType = "primary")
	{
		$this->HostType = $HostType;
		$this->HostName = $HostName;
	}

	function ProcessList($ProcessID = 0)
	{
		$Fields = array("queue_id", "delivery", "target", "status", "summary", "object_name", "object_path", "created", "owner", "host", "priority", "queued", "after", "processed", "delivered", "archived", "keep_until");
		$T = new Table;
		foreach ($Fields as $Field)
			$T->AddHead($Field);
		$T->EndRow();
		$Query  = "";
		$Query .= "SELECT ";
		$Query .= implode(", ", $Fields)." ";
		$Query .= "FROM process_queue ";
		if ($ProcessID)
			$Query .= "WHERE queue_id = ".sql::integer($ProcessID)." ";
		$Query .= "ORDER BY queue_id ";
		$Q = new DBQuery;
		$Q->Run(PROCESS_DB, $Query);
		while ($Q->Next())
		{
			foreach ($Fields as $Field)
				$T->AddData($Q->GetField($Field));
			$T->EndRow();
		}
		$Q->EndQuery();
		$T->Put();
	}

	function ProcessShow($ProcessID)
	{
		$this->ProcessList($ProcessID);
	}

	function ProcessLoad($ID)
	{
		$this->ID = $ID;
		$Q = new DBQuery;
		$Q->RunOne(PROCESS_DB, "select * from process_queue where queue_id = ".$this->ID);
		$this->Status = $Q->GetField("status");
		$this->Target = $Q->GetField("target");
		$this->Stored = $Q->GetField("created");
		$this->Delivery = $Q->GetField("delivery");
		$this->Priority = $Q->GetField("priority");
		$this->Summary = $Q->GetField("summary");
		$this->Generated = $Q->GetField("processed");
		$this->Delivered = $Q->GetField("delivered");
		$this->KeepUntil = $Q->GetField("keep_until");

		switch($this->Stored)
		{
			case "db":
				require_once($Q->GetField("object_path"));
				$this->Document = unserialize($Q->GetField("object"));
				break;
			default:
				$this->FileName = $Q->GetField("object");
				require_once($Q->GetField("object_path"));
				$this->Document = unserialize(file_get_contents($this->FileName));
				break;
		}
		$Q->EndQuery();
	}

	function _ProcessSave($Action)
	{
		if ($this->Status == "delete" && $Action == "delivered")
			$this->ProcessDelete($this->ID);
		else
		{
			$Sleep = $this->Document->Get("sleep") * 1;
			$INQ = new DBInsert;
			$INQ->SaveWhere("queue_id", $this->ID);
			$INQ->Save("filename", $this->Document->Get("filename"));
			switch($this->Stored)
			{
				case "db":
					$INQ->Save("object", serialize($this->Document));
					break;
				default:
					unlink($this->FileName);
					file_put_contents($this->FileName, serialize($this->Document));
					$INQ->Save("object", $this->FileName);
					break;
			}
			if ($Sleep > 0)
			{
				// maximum sleep is 1 week
				if ($Sleep > 604800)
					$Sleep = 604800;
				$Sleep = date("Y-m-d H:i:s", strtotime($Sleep." seconds"));
				$INQ->Save("after", $Sleep);
			}
			elseif ($this->Status == "advance" || $Action == "delivered")
				$INQ->Save("status", "");
			$INQ->Save($Action, date("Y-m-d H:i:s"));
			$INQ->Run(PROCESS_DB, "process_queue", "UPDATE");
			$INQ->EndQuery();
		}
		unset($this->ID);
		unset($this->Status);
		unset($this->Target);
		unset($this->Delivery);
		unset($this->Document);
	}

	function _CreateFileName()
	{
		$FileName = CreateUniqueKey(16);
		if (is_file(PROCESS_PATH.$FileName))
			$FileName = $this->_CreateFileName();
		return (PROCESS_PATH.$FileName);
	}

	function ProcessAdvance($ID)
	{
		$INQ = new DBInsert;
		$INQ->SaveWhere("queue_id", $ID);
		$INQ->Save("status", "advance");
		$INQ->Run(PROCESS_DB, "process_queue", "UPDATE");
		$INQ->EndQuery();
	}

	function ProcessDelete($ID, $HostName = "")
	{
		$Q = new DBQuery;
		$Query = "SELECT created, object FROM process_queue WHERE queue_id = ".$ID." AND host = '".addslashes($HostName)."'";
		$Data = DBRow(PROCESS_DB, $Query);
		if (sizeof($Data))
		{
			$Query = "delete from process_queue where queue_id = ".$ID;
			DB_DoQuery(PROCESS_DB, $Query);
			if ($Data["created"] != "db")
				if (is_file($Data["object"]))
					unlink($Data["object"]);
		}
	}

	function ProcessDeleteArchived()
	{
			$Q = new DBQuery;
			$Query = "select queue_id from process_queue where archived > '0000-00-00 00:00:00' and keep_until < '".date("Y-m-d")."'";
			$Q->Run(PROCESS_DB, $Query);
			while ($Q->Next())
				$this->ProcessDelete($Q->GetField("queue_id"));
			$Q->EndQuery();
	}

	function ProcessArchive($ID)
	{
		$INQ = new DBInsert;
		$INQ->SaveWhere("queue_id", $ID);
		$INQ->Save("status", "");
		$INQ->Save("archived", date("Y-m-d H:i:s"));
		$INQ->Run(PROCESS_DB, "process_queue", "UPDATE");
		$INQ->EndQuery();
	}

	function ProcessCreate($Document, $Object, $Summary, $Delivery = "", $Target = "", $Mode = "", $Priority = 0, $OwnerID = 0, $Stored = "db", $HostName = "", $Delay = "", $KeepUntil = "")
	{
		$INQ = new DBInsert;
		switch(strtoupper($Delivery))
		{
			case "EMAIL":
				$INQ->Save("delivery", "EMAIL");
				$INQ->Save("target", $Target);
				break;
			case "PRINT":
				$INQ->Save("delivery", "PRINT");
				$INQ->Save("target", $Target);
				break;
			case "PROCESS":
				$INQ->Save("delivery", "PROCESS");
				$INQ->Save("target", "");
				break;
			default:
				$INQ->Save("delivery", "");
				$INQ->Save("target", "");
				break;
		}
		$INQ->Save("queued", date("Y-m-d H:i:s"));
		switch (strtolower($Mode))
		{
			case "delete":
				$INQ->Save("status", "delete");
				break;
			case "automatic":
				$INQ->Save("status", "automatic");
				break;
			case "process":
				$INQ->Save("status", "advance");
				$INQ->Save("processed", date("Y-m-d H:i:s"));
				break;
			default:
				$INQ->Save("status", "advance");
				break;
		}
		$INQ->Save("summary", $Summary);
		$INQ->Save("object_name", get_class($Document));
		$INQ->Save("object_path", $Object);
		switch(strtolower($Stored))
		{
			case "db":
				$INQ->Save("created", "db");
				$INQ->Save("object", serialize($Document));
				break;
			case "fs":
				$INQ->Save("created", "fs");
				$FileName = $this->_CreateFileName();
				file_put_contents($FileName, serialize($Document));
				$INQ->Save("object", $FileName);
				break;
		}
		$INQ->Save("priority", $Priority);
		$INQ->Save("owner", $OwnerID);
		$INQ->Save("host", $HostName);
		if ($Delay)
			$INQ->Save("after", $Delay);
		if ($KeepUntil)
			$INQ->Save("keep_until", $KeepUntil);
		$INQ->Save("seed", serialize($Document));
		$INQ->Run(PROCESS_DB, "process_queue", "INSERT");
		$ID = $INQ->GetInsertId();
		$INQ->EndQuery();
		return ($ID);
	}

	function ProcessRestart($ProcessID, $ReDeliver = 0)
	{
		$Query = "SELECT * FROM process_queue WHERE queue_id = ".$ProcessID;
		$Process = DBRow(PROCESS_DB, $Query);

		$INQ = new DBInsert;

		$INQ->SaveWhere("queue_id", $Process["queue_id"]);
		if (!$NoDeliver)
		{
			$INQ->Save("delivery", "");
			$INQ->Save("target", "");
		}
		$INQ->Save("status", "advance");
		$INQ->Save("processed", "0000-00-00 00:00:00");
		$INQ->Save("delivered", "0000-00-00 00:00:00");
		$INQ->Save("archived", "0000-00-00 00:00:00");

		switch(strtolower($Process["created"]))
		{
			case "db":
				$INQ->Save("object", $Process["seed"]);
				break;
			case "fs":
				file_put_contents($Process["object"], $Process["seed"]);
				break;
		}
		$INQ->Run(PROCESS_DB, "process_queue", "UPDATE");
		$INQ->EndQuery();
	}

	function ProcessGenerate()
	{
		$Timer = new Stopwatch;
		$Q = new DBQuery;
		$INQ = new DBInsert;
		if ($this->HostName)
			$State = 1;
		else
			$State = 0;
		while ($State < 2)
		{
			$Query  = "SELECT queue_id FROM process_queue ";
			$Query .= "WHERE status <> '' ";
			$Query .= "AND processed = '0000-00-00 00:00:00' ";
			$Query .= "AND after < '".date("Y-m-d H:i:s")."' ";
			if ($State == 0)
				$Query .= "AND host = '".$HostName."' ";
			else
				$Query .= "AND host = '' ";
			$Query .= "ORDER BY priority DESC, queue_id ";
			$Query .= "LIMIT 1";

			$Q->RunOne(PROCESS_DB, $Query);
			if (!$Q->NumRows())
			{
				$State++;
			}
			else
			{
				if ($State)
				{
					$INQ->Save("host", "");
					$INQ->SaveWhere("queue_id", $Q->GetField("queue_id"));
					$INQ->Run(PROCESS_DB, "process_queue", "UPDATE");
					$INQ->EndQuery();
				}
				$this->ProcessLoad($Q->GetField("queue_id"));
				echo "\nProcessing: ".$Q->GetField("queue_id")."\n";
				$this->Document->Generate();
				if ($this->Document->Get("abort"))
					$this->ProcessDelete($this->ID);
				else
					$this->_ProcessSave("processed");
				InsertProcessingDelay(1);
			}
			$Q->EndQuery();
			if ($this->HostType == "primary")
				$this->ProcessDeliver(1);

			if ($Timer->GetStopDuration() > 900)
				$State = 2;
		}
	}

	function ProcessDeliverSave($Delivery, $Target)
	{
		$INQ = new DBInsert;
		$INQ->SaveWhere("queue_id", $this->ID);
		switch(strtoupper($Delivery))
		{
			case "EMAIL":
				$INQ->Save("delivery", "EMAIL");
				$INQ->Save("target", $Target);
				break;
			case "PRINT":
				$INQ->Save("delivery", "PRINT");
				$INQ->Save("target", $Target);
				break;
			case "PROCESS":
				$INQ->Save("delivery", "PROCESS");
				$INQ->Save("target", "");
				break;
		}
		if ($this->Status == "")
			$INQ->Save("status", "advance");
		$INQ->Save("delivered", "0000-00-00 00:00:00");
		$INQ->Run(PROCESS_DB, "process_queue", "UPDATE");
		$INQ->EndQuery();
		unset($this->ID);
		unset($this->Status);
		unset($this->Target);
		unset($this->Delivery);
		unset($this->Document);
	}

	function ProcessDownload($ID)
	{
		$this->ProcessLoad($ID);
		$D = $this->Document;
		$Data = $D->Get("data");
		Header("Content-type: ".$D->Get("mime"));
   		Header("Content-Length: ".strlen($Data));
		Header("Content-Disposition: attachment; filename=".$D->Get("filename"));
		echo $Data;
	}

	function ProcessDeliver($Priority = 0)
	{
		if ($this->HostType == "primary")
		{
			$Q = new DBQuery;
			$Query  = "select queue_id from process_queue ";
			$Query .= "where status <> '' ";
			$Query .= "and processed <> '0000-00-00 00:00:00' ";
			$Query .= "and delivered = '0000-00-00 00:00:00' ";
			if ($Priority)
				$Query .= "and priority > 0 ";
			$Query .= "order by priority desc, queue_id";
			$Q->Run(PROCESS_DB, $Query);
			while ($Q->Next())
			{
				$this->ProcessLoad($Q->GetField("queue_id"));
				$D = $this->Document;
				switch ($this->Delivery)
				{
					case "EMAIL":
						$M = new Mail();
						$M->to = $this->Target;
						$M->all_replies($D->Get("from"));
						$M->subject = $D->Get("subject");
						$M->body = $D->Get("body");
						$M->add_attachment($D->Get("data"), $D->Get("filename"), $D->Get("mime"));
						$M->send();
						if (defined("PROCESS_DOCSAVE"))
							file_put_contents(PROCESS_DOCSAVE.$D->Get("filename"), $D->Get("data"));
						$this->_ProcessSave("delivered");
						break;
					case "PRINT":
						$NoPrint = 0;
						if (defined("PROCESS_NOPRINT"))
							$NoPrint = PROCESS_NOPRINT;
						if (!$NoPrint)
						{
							$TmpFile = "/tmp/".CreateUniqueKey(16);
							$fp = fopen($TmpFile, "w");
							fputs($fp, $D->Get("data"), strlen($D->Get("data")));
							fclose($fp);
							$Command = "cat ".$TmpFile." | ".$this->Target;
							exec($Command);
							unlink($TmpFile);
						}
						$this->_ProcessSave("delivered");
						break;
					case "PROCESS":
						$this->_ProcessSave("delivered");
						break;
				}
				InsertProcessingDelay(1);
			}
			$Q->EndQuery();
		}
	}

	function ProcessArchival()
	{
		$Q = new DBQuery;
		$Query  = "select queue_id from process_queue ";
		$Query .= "where status <> '' ";
		$Query .= "and processed <> '0000-00-00 00:00:00' ";
		$Query .= "and delivered <> '0000-00-00 00:00:00' ";
		$Query .= "and archived = '0000-00-00 00:00:00' ";
		$Query .= "order by queue_id";
		$Q->Run(PROCESS_DB, $Query);
		while ($Q->Next())
			$this->ProcessArchive($Q->GetField("queue_id"));
		$Q->EndQuery();
	}

	function ProcessAll($HostName = "")
	{
		if ($HostName)
			$this->RequestProcesses($HostName);
		$this->ProcessGenerate();
		$this->ProcessDeliver();
	}

	function ProcessPurge($Objects)
	{
		$Q = new DBQuery;
		if (is_array($Objects))
		{
			foreach ($Objects as $Object)
			{
				$Query = "SELECT queue_id FROM process_queue WHERE object_name LIKE '".$Object."' AND delivered <> '0000-00-00 00:00:00'";
				$Q->Run(PROCESS_DB, $Query);
				while ($Q->Next())
					$this->ProcessDelete($Q->GetField("queue_id"));
				$Q->EndQuery();
			}
		}
	}

	function SupplyProcesses($HostName)
	{
		if ($HostName)
		{
			$Q = new DBQuery;
			$Query = "select * from process_queue where host = '".$HostName."'";
			$Q->Run(PROCESS_DB, $Query);
			$Processes = array();
			while ($Q->Next())
			{
				$Process = $Q->GetArray();
				switch($Process["created"])
				{
					case "db":
						$Process["object"] = base64_encode($Process["object"]);
						break;
					default:
						$Process["object"] = base64_encode(file_get_contents($Process["object"]));
						break;
				}
				$Process["seed"] = base64_encode($Process["seed"]);
				$Processes[] = $Process;
			}
			$Q->EndQuery();
		}
		else
			$Processes = array();
		return ($Processes);
	}

	function RequestProcesses($SoapServer, $HostName)
	{
		$P = new SoapClient($SoapServer, unserialize(SOAP));
		try
		{
			$Processes = $P->SupplyProcessService($HostName);
		}
		catch(SoapFault $Exception)
		{
			echo $Exception;
			$Processes = array();
		}

		if (!is_array($Processes))
		{
			print_r($Processes);
			return;
		}

		foreach($Processes as $Process)
		{
			$Query = "delete from process_queue where queue_id = ".$Process["queue_id"];
			DB_DoQuery(PROCESS_DB, $Query);

			$INQ = new DBInsert;

			$INQ->Save("queue_id", $Process["queue_id"]);
			$INQ->Save("delivery", $Process["delivery"]);
			$INQ->Save("target", $Process["target"]);
			$INQ->Save("status", $Process["status"]);
			$INQ->Save("summary", $Process["summary"]);
			$INQ->Save("object_name", $Process["object_name"]);
			$INQ->Save("object_path", INCLUDE_DIR.$Process["object_name"].".inc");
			switch($Process["created"])
			{
				case "db":
					$INQ->Save("object", base64_decode($Process["object"]));
					break;
				default:
					$FileName = $this->_CreateFileName();
					file_put_contents($FileName, base64_decode($Process["object"]));
					$INQ->Save("object", $FileName);
					break;
			}
			$INQ->Save("created", $Process["created"]);
			$INQ->Save("owner", $Process["owner"]);
			$INQ->Save("host", "");
			$INQ->Save("priority", $Process["priority"]);
			$INQ->Save("queued", $Process["queued"]);
			$INQ->Save("after", $Process["after"]);
			$INQ->Save("processed", $Process["processed"]);
			$INQ->Save("delivered", $Process["delivered"]);
			$INQ->Save("archived", $Process["archived"]);
			$INQ->Save("keep_until", $Process["keep_until"]);
			$INQ->Save("seed", base64_decode($Process["object"]));
			$INQ->Run(PROCESS_DB, "process_queue", "INSERT");
			$P = new SoapClient($SoapServer, unserialize(SOAP));
			try
			{
				echo $P->TransferProcessConfirmService($HostName, $Process["queue_id"]);
			}
			catch(SoapFault $Exception)
			{
				echo $Exception;
			}
			$INQ->EndQuery();
		}
	}

	function ProcessGet($QueueID)
	{
		$this->ProcessLoad($QueueID);
		return ($this->Document);
	}
}

/*

Every object requires:
 > PDF class as required
 > assumes the presence of PHPLIB and PDF_DIR
 > assumes the script include file has the same name as the class
 > assumes the script is located at INCLUDE_DIR and is fully self-contained
 > data seed caching
 > function Generate() which processes the report
 > function Get() which returns data as follows:
	->Get("data"):
		returns the raw document data
	->Get("from"):
		returns the reply address for an email
	->Get("subject"):
		returns the subject for the email
	->Get("body"):
		returns the body for the email
	->Get("filename"):
		returns the file name to be used for the attachment
	->Get("mime"):
		returns the document's mime type

CREATE TABLE process_queue (
  queue_id int(11) NOT NULL auto_increment,
  delivery varchar(8) NOT NULL default '',
  target varchar(128) NOT NULL default '',
  status varchar(16) NOT NULL default '',
  summary text NOT NULL,
  object_name varchar(128) NOT NULL default '',
  object_path varchar(255) NOT NULL default '',
  object longtext NOT NULL,
  created char(2) NOT NULL default 'db',
  owner int(11) NOT NULL default '0',
  host varchar(32) NOT NULL default '',
  priority tinyint(4) NOT NULL default '0',
  queued datetime NOT NULL default '0000-00-00 00:00:00',
  after datetime NOT NULL default '0000-00-00 00:00:00',
  processed datetime NOT NULL default '0000-00-00 00:00:00',
  delivered datetime NOT NULL default '0000-00-00 00:00:00',
  archived datetime NOT NULL default '0000-00-00 00:00:00',
  keep_until date NOT NULL default '0000-00-00',
  filename varchar(255) NOT NULL default '',
  PRIMARY KEY  (queue_id)
) ENGINE=MyISAM DEFAULT CHARSET=latin1;

*/

?>
