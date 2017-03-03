<?php
// zip.inc
//
// Multi-Threaded Processing
//
// Copyright MU Systems Pty Ltd 2007
//
// Author: Murray Shields
//

class threads
{
	function __construct($Priority = 0)
	{
		if (!(defined("THREADS_DB")))
			exit("\n\nTHREADS_DB must be defined.\n\n");
		$this->Threads = array();
		$this->Priority = ($Priority * 1);
	}

	function add($DIR, $Command)
	{
		$this->Threads[] = $this->create($DIR, $Command);
	}

	function create($DIR, $Command)
	{
		$INQ = new DBInsert;
		$INQ->Save("dir", $DIR);
		$INQ->Save("command", $Command);
		$INQ->Save("scheduled", date("Y-m-d H:i:s"));
		$INQ->Save("priority", $this->Priority);
		$INQ->Run(THREADS_DB, "process_thread", "INSERT");
		$ProcessID = $INQ->GetInsertID();
		$INQ->EndQuery();
		return($ProcessID);
	}

	function run($MaxThreads = 2)
	{
		$Query = "SELECT COUNT(*) FROM process_thread WHERE status = 'PROCESS'";
		$Running = DBGet(THREADS_DB, $Query);

		$MaxThreads = $MaxThreads * 1;
		if (!$MaxThreads)
			$MaxThreads = 2;

		if ($Running < $MaxThreads)
		{
			$Query = "SELECT process_id, dir, command FROM process_thread WHERE status = 'SCHEDULE' ORDER BY priority DESC, process_id LIMIT 1";
			$Process = DBRow(THREADS_DB, $Query);
			if ($Process["process_id"] * 1)
			{
				$this->update($Process["process_id"], "PROCESS");
				chdir($Process["dir"]);
				$Command = $Process["command"];
				$Output = `$Command`;
				$this->update($Process["process_id"], "COMPLETE", $Output);
			}
		}
//		if ((date("Hi") * 1) <= 5)
//			$this->clean();
	}

	function clean()
	{
		$Query = "DELETE FROM process_thread WHERE finalised < '".date("Y-m-d H:i:s", strtotime("1 week ago"))."'";
		DB_DoQuery(THREADS_DB, $Query);
	}

	function update($ProcessID, $Status, $Output = "")
	{
		$INQ = new DBInsert;
		$INQ->SaveWhere("process_id", $ProcessID);
		$INQ->Save("status", $Status);
		$INQ->Save("output", $Output);
		if ($Status == "COMPLETE")
			$INQ->Save("finalised", date("Y-m-d H:i:s"));
		$INQ->Run(THREADS_DB, "process_thread", "UPDATE");
		$INQ->EndQuery();
	}

	function wait()
	{
		$Query = "SELECT COUNT(*) FROM process_thread WHERE status = 'COMPLETE' AND process_id IN (".implode(",", $this->Threads).")";;
		while (DBGet(THREADS_DB, $Query) != sizeof($this->Threads))
			sleep(30);

		$Errors = array();
		$Query = "SELECT output FROM process_thread WHERE status = 'COMPLETE' AND process_id IN (".implode(",", $this->Threads).")";;
		$Q = new DBQuery;
		$Q->Run(THREADS_DB, $Query);
		while ($Q->Next())
			$Errors[] = $Q->GetField("output");
		$Q->EndQuery();
		return(implode("\n", $Errors));
	}

//	function check($ProcessID)
//	{
//		$Query = "SELECT COUNT(*) FROM process_thread WHERE status = 'COMPLETE' AND process_id = ".$ProcessID;
//		return(DBGet(THREADS_DB, $Query));
//	}
//
//	function output($ProcessID)
//	{
//		$Query = "SELECT output FROM process_thread WHERE status = 'COMPLETE' AND process_id = ".$ProcessID;
//		return(DBGet(THREADS_DB, $Query));
//	}
}

?>
