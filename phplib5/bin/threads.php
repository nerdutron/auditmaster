<?php
// threads.php - thread processing
// runs: every minute

require_once("definitions.inc");
require_once(PHPLIB_DIR."threads.inc");
$Max = $argv[1];
$P = new threads();
$P->run($Max);

?>
