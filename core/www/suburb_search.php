<?php

require_once("../local.inc");
require_once("../../audit/includes/tools.inc");
require_once(PHPLIB_DIR."jsonwrapper.php");

$Term = strtolower($_GET["term"]);
if (!$Term) return;

$Data = tools::GetSuburb($Term);

echo json_encode($Data);


?>
