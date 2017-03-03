<?php

require_once("../local.inc");
require_once("../../audit/includes/operator.inc");

$Term = strtolower($_GET["term"]);
if (!$Term) return;

$Data = operator::OperatorSearch($Term);

echo json_encode($Data);


?>
