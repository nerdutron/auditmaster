<?php

set_time_limit(0);
define("PHPLIB_DIR", "../");
require_once(PHPLIB_DIR."auth.inc");
define("PROCESS_DB", "mysql://".$MY_USER.":".$MY_PASSWORD."@localhost/SW_MARKET_TEST");
require_once(PHPLIB_DIR."process.inc");
ini_set("soap.wsdl_cache_enabled", "0"); // disabling WSDL cache

class SoapProcess
{
	function SupplyProcesses($HostName)
	{
		$P = new Process;
		return($P->SupplyProcesses($HostName));
	}

	function TransferredProcess($ID)
	{
		$P = new Process;
		return($P->ProcessDelete($ID));
	}
}

$server = new SoapServer("wsdl/process.wsdl");
$server->setClass("SoapProcess");
$server->handle();

?>
