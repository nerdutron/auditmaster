<?php
/*
Copyright: MU Systems Pty Ltd, 2004
*/

require_once(PHPLIB_DIR."tools.inc");
require_once(PHPLIB_DIR."database.inc");

class Script
{
	function __construct($DB, $ScriptCode, $Config = "")
	{
		$this->DB = $DB;
		$this->ScriptCode = $ScriptCode;
		$this->INQ = new DBInsert;

		if (!is_array($Config))
			$Config = array();
		foreach ($Config as $Key => $Val)
			$Config[$Key] = trim($Val);

		$this->RestartMax = ($Config["restart_maximum"] ? $Config["restart_maximum"] : 1);
		$this->ScriptSens = ($Config["load_sensitivity"] ? $Config["load_sensitivity"] : 3);
		$this->Every = ($Config["every"] ? $Config["every"] : 0);
		$this->Delay = ($Config["delay"] ? $Config["delay"] : 0);
		$this->Timeout = ($Config["timeout"] ? $Config["timeout"] : 300);
		$this->Description = ($Config["description"] ? $Config["description"] : "NOTIFICATION");

		$this->NotificationEmail = $Config["email"];
		$this->NotificationPager = $Config["pager"];
		$this->Log = $Config["log_file"];
		$this->LogCount = $Config["log_count"];
		$this->Count = 0;
		if ($this->Every * $this->Delay)
			$this->UseTimeout = 1;
		else
			$this->UseTimeout = 0;
	}

	function GetLogFile()
	{
		$Log = "";
		if ($this->LogCount && is_file($this->Log))
		{
			$Command = "tail -n ".$this->LogCount." ".$this->Log;
			$Log .= "\n\n\n============================================================\n\n";
			$Log .= `$Command`;
		}
		return ($Log);
	}

	function Open()
	{
		$Q = new DBQuery;
		$Query = "select max(log_id) as log_id from script_log where script_code = '".addslashes($this->ScriptCode)."'";
		$Q->RunOne($this->DB, $Query);
		$LogID = $Q->GetField("log_id");
		if ($LogID)
		{
			$Q->EndQuery();
			$Query = "select * from script_log where log_id = ".$LogID;
			$Q->RunOne($this->DB, $Query);
		}
		$this->ProcessID = $Q->GetField("process_id");
		$this->ProcessName = $Q->GetField("process_name");
		$Restarts = $Q->GetField("is_restart");

		if ($Q->GetField("active_status") == 3)
		{
			echo "\n\nThis script has crashed too many times. Please check for process errors before resetting logs.\n\n";
			exit();
		}

		if ($Q->GetField("active_status"))
		{
			$Duration = mktime() - CreateTimeStamp($Q->GetField("date_time"));
			if ($Q->GetField("active_status") > 1)
				$Duration = $Duration + $this->Timeout;

			if ($this->ProcessName != $this->GetProcessName($this->ProcessID))
			{

				if ($Restarts < $this->RestartMax)
				{
					$Message = "Restart number ".($Restarts + 1)." out of ".$this->RestartMax.".";
					if ($this->NotificationEmail)
						mail($this->NotificationEmail, $this->Description, $this->Description." ".$Message.$this->GetLogFile());

					$this->LogEntry(1, ($Restarts + 1),  $this->GetProcessID(), $this->GetProcessName($this->GetProcessID()));
					echo "\n\n".$Message."\n\n";
					return(0);
				}
				else
				{
					$Message = "Restart Maximum of ".($this->RestartMax)." Exceeded. Please check for process errors before resetting logs.";
					if ($this->NotificationEmail)
						mail($this->NotificationEmail, $this->Description, $this->Description." ".$Message.$this->GetLogFile());
					if ($this->NotificationPager)
						mail($this->NotificationPager, $this->Description, "<SMS>Script ".$this->ScriptCode." ".$Message."</SMS>");

					$this->LogEntry(3, $Restarts, 0, "");
					echo "\n\n".$Message."\n\n";
					exit();
				}
			}

			echo "\n\nA process is currently running (started ".$Duration." seconds ago). Please try again later.\n\n";
	
			if (($Q->GetField("active_status") == 1) && ($Duration > $this->Timeout))
			{
				$Minutes = $this->Timeout / 60;
				$Message = " Timeout: currently at ".$Minutes." minutes.";
				if ($this->NotificationEmail)
					mail($this->NotificationEmail, $this->Description, $this->Description.$Message);
				if ($this->NotificationPager)
					mail($this->NotificationPager, $this->Description, "<SMS>Script ".$this->ScriptCode.$Message."</SMS>");

				$this->LogEntry(2, $Restarts, $this->ProcessID, $this->ProcessName);
			}
			exit();
		}
		$Q->EndQuery();

		$this->LogEntry(1, 0, $this->GetProcessID(), $this->GetProcessName($this->GetProcessID()));
	}

	function LogEntry($Status, $Restart, $ProcessID, $ProcessName)
	{
		$this->INQ->Save("log_id", 0);
		$this->INQ->Save("script_code", $this->ScriptCode);
		$this->INQ->Save("active_status", $Status);
		$this->INQ->Save("is_restart", $Restart);
		$this->INQ->Save("date_time", date("Y-m-d H:i:s"));
		$this->INQ->Save("process_id", $ProcessID);
		$this->INQ->Save("process_name", $ProcessName);
		$this->INQ->Run($this->DB, "script_log", "INSERT");
		$this->INQ->EndQuery();
	}

	function Increment()
	{
		$Delay = InsertProcessingDelay($this->ScriptSens);

		if ($this->UseTimeout)
		{
			$this->Count++;
			if ($this->Count >= $this->Every)
			{
				$this->Count = 0;
				sleep($this->Delay);
			}
		}

		return($Delay);
	}

	function Close()
	{
		$Q = new DBQuery;
		$Query = "select max(log_id) as log_id from script_log where script_code = '".addslashes($this->ScriptCode)."'";
		$Q->RunOne($this->DB, $Query);
		$LogID = $Q->GetField("log_id");
		$Q->EndQuery();
		
		$Query = "select * from script_log where log_id = ".$LogID;
		$Q->RunOne($this->DB, $Query);

		if ($Q->GetField("active_status") >= 2)
		{
			if ($Q->GetField("is_restart") == $this->RestartMax)
				$Message = " Crash: Reset Now Complete.";
			else
			{
				$Message = " Timeout: Processing Now Complete.";
				if ($this->NotificationPager)
					mail($this->NotificationPager, $this->Description, "<SMS>Script ".$this->ScriptCode.$Message."</SMS>");
			}
			if ($this->NotificationEmail)
				mail($this->NotificationEmail, $this->Description, $this->Description.$Message);
		}
		$this->LogEntry(0, 0, 0, "");
	}

	function GetProcessID()
	{
		return(getmypid());
	}

	function GetProcessName($ProcessID)
	{
		$ProcessName = "";
		$ProcessFile = "/proc/".$ProcessID."/cmdline";
		if (is_file($ProcessFile))
			$ProcessName = `cat $ProcessFile`;
		return($ProcessName);
	}

}

// automatically insert a script delay based on the current load and script sensitivity
function InsertProcessingDelay($Sensitivity = 3)
{
	// ensure the sensitivity is valid, else reset to the default
	if (!in_array($Sensitivity, array(1,2,3,4,5)))
		$Sensitivity = 3;

	// retrieve the current system load
	$CurrentLoad = array_shift(explode(" ", trim(`cat /proc/loadavg`)));

	// work out the delay multiplier (based on the current load)
	$Multiplier = floor($CurrentLoad / 2);
	if ($Multiplier > 5)
		$Multiplier = 5;

	// the sleep delay (in seconds) is a factor of the all calculated parameters
	$Delay = round(($Multiplier - $Sensitivity + 1) * $CurrentLoad);
	if ($Delay <= 0)
		$Delay = 0;

	// go to sleep
	sleep($Delay);
	return($Delay);
}

// Restart the script details fom live or dev
function ScriptRestart($DB, $ScriptCode)
{
	$SC = new Script($DB, $ScriptCode);
	$SC->Close();
	return 1;
}

/*
CREATE TABLE script_log (
  log_id int(11) NOT NULL auto_increment,
  script_code varchar(16) NOT NULL default '',
  active_status tinyint(4) NOT NULL default '0',
  date_time datetime NOT NULL default '0000-00-00 00:00:00',
  is_restart tinyint(4) NOT NULL default '0',
  process_id int(11) NOT NULL default '0',
  process_name varchar(128) NOT NULL default '',
  PRIMARY KEY  (log_id),
  KEY idx_li_sc (log_id,script_code),
  KEY idx_sc_li (script_code,log_id)
) TYPE=MyISAM;
*/

?>
